#include "lc_rvclog.h"

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "lc_config.h"

// Flat, fixed-size capture buffer — see the header for why this never wraps,
// and why it is heap-allocated on first use here (no PSRAM on this board)
// instead of the hub's static EXT_RAM_BSS_ATTR array.
static lc_rvclog_record_t *s_records = NULL;

static volatile bool     s_capturing   = false;
static volatile uint32_t s_count       = 0;
static volatile bool     s_buffer_full = false;
static int64_t           s_start_us    = 0;
static uint32_t          s_max_secs    = LC_RVCLOG_DEFAULT_MAX_SECS;
static uint32_t          s_mark_seq    = 0;

// Guards only the "reserve the next slot index" step — both rvc_print_task
// (the hot-path writer) and the httpd task (Mark button) can call into this
// concurrently, but neither holds the lock across the actual record write.
static SemaphoreHandle_t s_reserve_lock;

void lc_rvclog_init(void)
{
    s_reserve_lock = xSemaphoreCreateMutex();
}

// Returns the reserved index, or -1 if capture isn't running or the buffer
// is already full (which also stops capture in the latter case).
static int32_t reserve_slot(void)
{
    if (!s_capturing) return -1;

    xSemaphoreTake(s_reserve_lock, portMAX_DELAY);
    int32_t idx = -1;
    if (s_capturing) {
        if (s_count < LC_RVCLOG_MAX_RECORDS) {
            idx = (int32_t)s_count++;
        } else {
            s_buffer_full = true;
            s_capturing = false;
        }
    }
    xSemaphoreGive(s_reserve_lock);
    return idx;
}

static void capture_with_dir(uint32_t can_id, const uint8_t *data, uint8_t len, uint8_t dir)
{
    if (!s_capturing) return;   // fast path: no lock, no work, when idle

    int64_t now_us = esp_timer_get_time();
    if ((uint32_t)((now_us - s_start_us) / 1000000) >= s_max_secs) {
        s_capturing = false;
        return;
    }

    int32_t idx = reserve_slot();
    if (idx < 0) return;

    lc_rvclog_record_t *rec = &s_records[idx];
    rec->t_us = (uint32_t)(now_us - s_start_us);
    rec->can_id = can_id;
    rec->dir = dir;
    rec->len = (len > 8) ? 8 : len;
    if (rec->len > 0) memcpy(rec->data, data, rec->len);
    if (rec->len < 8) memset(rec->data + rec->len, 0, 8 - rec->len);
}

void lc_rvclog_capture_frame(uint32_t can_id, const uint8_t *data, uint8_t len)
{
    capture_with_dir(can_id, data, len, LC_RVCLOG_DIR_RX);
}

void lc_rvclog_capture_tx(uint32_t can_id, const uint8_t *data, uint8_t len, bool ok)
{
    capture_with_dir(can_id, data, len, ok ? LC_RVCLOG_DIR_TX : LC_RVCLOG_DIR_TX_FAIL);
}

esp_err_t lc_rvclog_start(uint32_t max_secs)
{
    if (max_secs < LC_RVCLOG_MIN_MAX_SECS) max_secs = LC_RVCLOG_MIN_MAX_SECS;
    if (max_secs > LC_RVCLOG_MAX_MAX_SECS) max_secs = LC_RVCLOG_MAX_MAX_SECS;

    // Lazy one-time allocation, kept for the life of the boot. Internal-heap
    // cost is LC_RVCLOG_MAX_RECORDS × 20 B (80 KB at the shipped 4,000) and
    // is paid the first time someone actually starts a capture — never at
    // boot, so the Wi-Fi/httpd bring-up heap budget is unaffected.
    if (s_records == NULL) {
        s_records = heap_caps_malloc(LC_RVCLOG_MAX_RECORDS * sizeof(lc_rvclog_record_t),
                                     MALLOC_CAP_8BIT);
        if (s_records == NULL) {
            ESP_LOGE(TAG, "rvclog: cannot allocate %u-record capture buffer (%u B)",
                     (unsigned)LC_RVCLOG_MAX_RECORDS,
                     (unsigned)(LC_RVCLOG_MAX_RECORDS * sizeof(lc_rvclog_record_t)));
            return ESP_ERR_NO_MEM;
        }
        ESP_LOGI(TAG, "rvclog: allocated %u-record capture buffer (%u B)",
                 (unsigned)LC_RVCLOG_MAX_RECORDS,
                 (unsigned)(LC_RVCLOG_MAX_RECORDS * sizeof(lc_rvclog_record_t)));
    }

    xSemaphoreTake(s_reserve_lock, portMAX_DELAY);
    s_capturing   = false;   // pause the writer while state resets
    s_count       = 0;
    s_buffer_full = false;
    s_mark_seq    = 0;
    s_max_secs    = max_secs;
    s_start_us    = esp_timer_get_time();
    s_capturing   = true;
    xSemaphoreGive(s_reserve_lock);
    return ESP_OK;
}

esp_err_t lc_rvclog_stop(void)
{
    s_capturing = false;
    return ESP_OK;
}

esp_err_t lc_rvclog_mark(uint32_t *out_seq)
{
    if (!s_capturing) return ESP_ERR_INVALID_STATE;

    int32_t idx = reserve_slot();
    if (idx < 0) return ESP_ERR_NO_MEM;

    // No lock needed around s_mark_seq: the only caller is the httpd mark
    // handler, and httpd is single-threaded, so at most one mark request is
    // ever in flight.
    uint32_t seq = ++s_mark_seq;

    lc_rvclog_record_t *rec = &s_records[idx];
    rec->t_us = (uint32_t)(esp_timer_get_time() - s_start_us);
    rec->can_id = LC_RVCLOG_MARK_ID;
    rec->dir = LC_RVCLOG_DIR_RX;
    rec->len = 0;
    memset(rec->data, 0, sizeof(rec->data));
    rec->data[0] = (uint8_t)(seq & 0xFF);
    rec->data[1] = (uint8_t)((seq >> 8) & 0xFF);
    rec->data[2] = (uint8_t)((seq >> 16) & 0xFF);
    rec->data[3] = (uint8_t)((seq >> 24) & 0xFF);

    if (out_seq != NULL) *out_seq = seq;
    return ESP_OK;
}

void lc_rvclog_get_status(lc_rvclog_status_t *out)
{
    if (out == NULL) return;

    xSemaphoreTake(s_reserve_lock, portMAX_DELAY);
    out->capturing   = s_capturing;
    out->buffer_full = s_buffer_full;
    out->count       = s_count;
    out->capacity    = LC_RVCLOG_MAX_RECORDS;
    out->max_secs    = s_max_secs;
    out->elapsed_s   = s_count > 0 ? (uint32_t)((esp_timer_get_time() - s_start_us) / 1000000) : 0;
    xSemaphoreGive(s_reserve_lock);
}

bool lc_rvclog_get_record_copy(size_t index, lc_rvclog_record_t *out)
{
    if (out == NULL || s_records == NULL || index >= s_count) return false;
    *out = s_records[index];
    return true;
}
