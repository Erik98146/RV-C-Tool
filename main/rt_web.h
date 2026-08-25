#pragma once

// Stand-alone RV-C Tools web stack: Wi-Fi AP+STA manager, HTTP server,
// SPIFFS asset serving, the /ws delta stream, and every /api/* endpoint the
// two pages (rvc-tools.html, network.html) call. Carved from hub_core's
// lc_web.c — same handler code, minus everything hub-specific (auth/PIN,
// publish config, cards, MQTT, integrations).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#include "rvc_spec_generated.h"
#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Brings up NVS-backed Wi-Fi (AP + saved-network STA), mounts SPIFFS, starts
// the HTTP server and the /ws pump task. Call once from app_main, after
// lc_can_init()/rvc_print_task exist.
esp_err_t rt_web_init(void);

// /ws push events, called from the CAN dispatch task (main.c).
void ws_enqueue_clear(void);
void ws_enqueue_activity_event(uint32_t dgn, const char *dgn_name, uint32_t count,
                               bool has_source_id, uint8_t source_id);
void ws_enqueue_field_event(uint32_t dgn, const char *dgn_name, int16_t instance,
                            const rvc_rx_frame_t *frame, const field_desc_t *field,
                            bool changed, uint32_t seq);

// Closed-loop transmit confirm: called from the CAN dispatch task for every
// extended data frame. One volatile read when no confirm watch is armed.
void lc_web_tx_confirm_check(uint32_t can_id, const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
