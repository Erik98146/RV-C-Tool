#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Raw RV-C bus capture for troubleshooting — ported from hub_core's
// lc_rvclog. Deliberately undecoded: stores exactly what came off the bus
// (CAN ID + data bytes + a capture-relative timestamp); all filtering, DGN
// grouping and change-highlighting happens client-side in rvc-tools.html's
// viewer, so this module has zero knowledge of the RV-C spec.
//
// Buffer-then-download, not live streaming: capture writes into a flat,
// fixed-size buffer and stops (does NOT wrap) once it fills, so a downloaded
// log is always a complete, gapless window. Download is only available once
// capture is stopped (enforced in rt_web.c's handler).
//
// ⚠ SIZING DIFFERS FROM THE HUB. The hub parks 50,000 records (~1 MB) in
// PSRAM; the Copperhill ESP32-WROOM-32 has no PSRAM, so the buffer lives in
// internal heap instead and is ALLOCATED LAZILY on the first capture start
// (kept for the life of the boot after that). 4,000 records × 20 B = 80 KB —
// at a typical coach bus rate of 100-300 frames/s that is roughly 15-40 s of
// dense traffic, or much longer on a quiet bus. If the allocation fails,
// lc_rvclog_start() returns ESP_ERR_NO_MEM and the UI reports it; boot-time
// RAM is never affected. Raise/lower LC_RVCLOG_MAX_RECORDS against the
// measured free heap on real hardware, not by guesswork.
#define LC_RVCLOG_MAX_RECORDS      4000u
#define LC_RVCLOG_MIN_MAX_SECS     30u
#define LC_RVCLOG_DEFAULT_MAX_SECS 300u
#define LC_RVCLOG_MAX_MAX_SECS     1800u

// Sentinel can_id marking an inline "user pressed Mark" record rather than a
// real bus frame. Safe: extended CAN IDs are 29 bits (max 0x1FFFFFFF).
#define LC_RVCLOG_MARK_ID          0xFFFFFFFFu

// Frame direction. The TWAI controller does NOT receive its own
// transmissions, so frames this tool sends are recorded explicitly at the
// transmit choke point (lc_can_transmit_rvc). TX_FAIL is kept distinct from
// TX: "tried to send and the driver refused" is exactly what you need to see
// when a multi-frame action only half-lands.
#define LC_RVCLOG_DIR_RX      0
#define LC_RVCLOG_DIR_TX      1
#define LC_RVCLOG_DIR_TX_FAIL 2

typedef struct {
    uint32_t t_us;      // microseconds since this capture's start
    uint32_t can_id;    // full 29-bit extended CAN ID, or LC_RVCLOG_MARK_ID
    uint8_t  len;       // 0-8 for a real frame; unused (0) for a marker
    uint8_t  dir;       // LC_RVCLOG_DIR_* (markers are recorded as RX)
    uint8_t  data[8];   // for a marker: seq number packed little-endian into data[0..3]
} lc_rvclog_record_t;

// Call once at boot, before rvc_print_task starts (creates the reservation
// mutex only — the record buffer is allocated on first capture start).
void lc_rvclog_init(void);

// Call from main.c's rvc_print_task for every frame dequeued from the TWAI
// RX queue, ahead of any RV-C-specific filtering. Near-zero cost when no
// capture is running: a single bool check before anything else.
void lc_rvclog_capture_frame(uint32_t can_id, const uint8_t *data, uint8_t len);

// Records a locally-originated frame (called from lc_can_transmit_rvc).
// `ok` selects LC_RVCLOG_DIR_TX vs LC_RVCLOG_DIR_TX_FAIL.
void lc_rvclog_capture_tx(uint32_t can_id, const uint8_t *data, uint8_t len, bool ok);

// Clears the buffer and starts capturing. max_secs is clamped to
// [LC_RVCLOG_MIN_MAX_SECS, LC_RVCLOG_MAX_MAX_SECS]; capture auto-stops at
// that elapsed time or when the buffer fills, whichever happens first.
// Returns ESP_ERR_NO_MEM if the record buffer cannot be allocated.
esp_err_t lc_rvclog_start(uint32_t max_secs);

// No-op (returns ESP_OK) if a capture isn't currently running.
esp_err_t lc_rvclog_stop(void);

// Inserts an inline marker at the current capture position. Returns
// ESP_ERR_INVALID_STATE if no capture is running, ESP_ERR_NO_MEM if the
// buffer is already full. *out_seq is a 1-based sequence number the browser
// matches back up by after downloading the log — marker label text is never
// sent to or stored on the device.
esp_err_t lc_rvclog_mark(uint32_t *out_seq);

typedef struct {
    bool     capturing;
    bool     buffer_full;   // capture auto-stopped because the buffer filled
    uint32_t count;
    uint32_t capacity;
    uint32_t elapsed_s;
    uint32_t max_secs;
} lc_rvclog_status_t;

void lc_rvclog_get_status(lc_rvclog_status_t *out);

// Copy-out of a single record by index (0..count-1). Returns false if index
// is out of range. Only called from the httpd task while capturing is false
// (enforced in rt_web.c's download handler), so there is no writer to race.
bool lc_rvclog_get_record_copy(size_t index, lc_rvclog_record_t *out);

#ifdef __cplusplus
}
#endif
