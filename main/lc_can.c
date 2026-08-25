#include "lc_can.h"

#include <string.h>

#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#include "lc_config.h"
#include "lc_rvclog.h"

QueueHandle_t lc_can_rx_queue = NULL;

// Serializes lc_can_transmit_rvc(). Five separate tasks now reach it: the
// httpd (/api/rvc/send), the MQTT client (command intake), the 1 Hz clock task
// (sleep timer + card off-timers), and the control-WS command task. Without
// this, two of them can interleave transmit / wait_all_done -- and
// twai_node_transmit_wait_all_done() waits for the whole TX queue to drain,
// not for *your* frame, so concurrent callers can each report a result that
// actually belongs to the other's frame. Holding the lock across both calls
// restores "wait_all_done means my frame went out", which is the entire basis
// for the caller's success/failure answer.
static SemaphoreHandle_t s_tx_mutex = NULL;

// Worst case a caller waits: the 200+200ms of the transmit already in
// progress, plus a little slack. Bounded rather than portMAX_DELAY so a wedged
// bus can never park the httpd task indefinitely.
#define LC_CAN_TX_LOCK_TIMEOUT_MS 500

volatile uint32_t diag_rx_frames = 0;
volatile uint32_t diag_rx_dropped_isr = 0;
uint64_t diag_rx_busy_us = 0;

static twai_node_handle_t s_twai_node = NULL;

static bool IRAM_ATTR lc_can_rx_done_callback(
    twai_node_handle_t handle,
    const twai_rx_done_event_data_t *edata,
    void *user_ctx
) {
    uint8_t rx_buffer[8];

    twai_frame_t rx_frame = {
        .buffer = rx_buffer,
        .buffer_len = sizeof(rx_buffer),
    };

    if (twai_node_receive_from_isr(handle, &rx_frame) != ESP_OK) {
        return false;
    }

    rvc_rx_frame_t out = {
        .id = rx_frame.header.id,
        .len = rx_frame.buffer_len,
        .is_extended = rx_frame.header.ide,
        .is_rtr = rx_frame.header.rtr,
    };

    if (out.len > 8) {
        out.len = 8;
    }

    memcpy(out.data, rx_buffer, out.len);

    BaseType_t high_task_wakeup = pdFALSE;

    if (lc_can_rx_queue != NULL) {
        if (xQueueSendFromISR(lc_can_rx_queue, &out, &high_task_wakeup) != pdTRUE) {
            diag_rx_dropped_isr++;
        }
    }

    return high_task_wakeup == pdTRUE;
}

esp_err_t lc_can_init(void)
{
    if (s_twai_node != NULL) {
        return ESP_OK;
    }

    lc_can_rx_queue = xQueueCreate(RX_QUEUE_LENGTH, sizeof(rvc_rx_frame_t));
    if (lc_can_rx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create RX queue");
        return ESP_ERR_NO_MEM;
    }

    s_tx_mutex = xSemaphoreCreateMutex();
    if (s_tx_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create TWAI TX mutex");
        return ESP_ERR_NO_MEM;
    }

    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = CAN_TX_GPIO,
        .io_cfg.rx = CAN_RX_GPIO,
        .bit_timing.bitrate = 250000,
        .tx_queue_depth = 8,
        .flags.enable_listen_only = false,
    };

    esp_err_t ret = twai_new_node_onchip(&node_config, &s_twai_node);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create TWAI node: %s", esp_err_to_name(ret));
        return ret;
    }

    twai_event_callbacks_t callbacks = {
        .on_rx_done = lc_can_rx_done_callback,
    };

    ret = twai_node_register_event_callbacks(s_twai_node, &callbacks, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register TWAI callbacks: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = twai_node_enable(s_twai_node);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable TWAI node: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

bool lc_can_is_ready(void)
{
    return s_twai_node != NULL;
}

uint32_t lc_can_build_rvc_ext_id_from_dgn(uint32_t dgn, uint8_t source_address)
{
    const uint8_t priority = 6;
    uint32_t raw_dgn = dgn & 0x3FFFF;

    if (dgn <= 0xFFFF) {
        uint8_t pf = dgn & 0xFF;
        if (pf < 240) raw_dgn = ((uint32_t)pf << 8) | 0xFF;
    }

    return ((uint32_t)priority << 26) | ((raw_dgn & 0x3FFFF) << 8) | source_address;
}

esp_err_t lc_can_transmit_rvc(uint32_t dgn, uint8_t source_address, const uint8_t *bytes, size_t len, uint32_t *out_can_id)
{
    if (s_twai_node == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (bytes == NULL || len != 8) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t can_id = lc_can_build_rvc_ext_id_from_dgn(dgn, source_address);
    if (out_can_id != NULL) {
        *out_can_id = can_id;
    }

    twai_frame_t tx_frame = {
        .header.id = can_id,
        .header.ide = true,
        .header.rtr = false,
        .buffer = (uint8_t *)bytes,
        .buffer_len = len,
    };

    // See s_tx_mutex: transmit and wait_all_done must be one atomic unit, or a
    // concurrent caller's frame can satisfy our wait (and vice versa).
    if (s_tx_mutex != NULL &&
        xSemaphoreTake(s_tx_mutex, pdMS_TO_TICKS(LC_CAN_TX_LOCK_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGW(TAG, "TWAI TX busy: another task held the bus for >%dms",
                 LC_CAN_TX_LOCK_TIMEOUT_MS);
        return ESP_ERR_TIMEOUT;
    }

    // Keep these timeouts short: callers include the single-threaded httpd task
    // (/api/rvc/send) and the MQTT task (command intake) — a dead or error-
    // passive bus at 1000ms each blocked the entire web UI for up to 2s per
    // command. A full 8-byte extended frame at 250 kbps is ~0.5ms on the wire;
    // 200ms still covers heavy bus contention with two orders of margin.
    esp_err_t ret = twai_node_transmit(s_twai_node, &tx_frame, 200);
    if (ret == ESP_OK) {
        ret = twai_node_transmit_wait_all_done(s_twai_node, 200);
    }

    if (s_tx_mutex != NULL) xSemaphoreGive(s_tx_mutex);

    // Record into the RV-C capture log (troubleshooting tool). Done here
    // because the controller never receives its own transmissions, so these
    // frames are otherwise invisible to a capture -- see lc_rvclog.h. Placed
    // AFTER the mutex is released on purpose: this must not extend the window
    // in which other transmitting tasks are blocked. No-op (one bool check)
    // when no capture is running.
    lc_rvclog_capture_tx(can_id, bytes, (uint8_t)len, ret == ESP_OK);

    return ret;
}
