#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lc_config.h"

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
    bool is_extended;
    bool is_rtr;
} rvc_rx_frame_t;

typedef struct {
    char text[WS_EVENT_MAX_BYTES];
} ws_event_t;

typedef struct {
    uint32_t dgn;
    const char *name;
    bool enabled;
} watched_dgn_t;

typedef struct {
    bool seen;
    uint32_t dgn;
    const char *name;
    uint32_t count;
    bool has_source_id;
    uint8_t source_id;
    bool has_data;
    uint8_t len;
    uint8_t data[8];
    int64_t last_sent_us;
} active_dgn_state_t;

typedef struct {
    bool seen;
    uint32_t dgn;
    const char *name;
    uint8_t source_id;
    int16_t instance;
    uint32_t count;
    int64_t last_seen_us;
} discovery_instance_state_t;

typedef struct {
    bool valid;
    bool has_snapshot;
    uint32_t dgn;
    int16_t instance;
    uint8_t source_id;
    uint8_t len;
    uint8_t data[8];
    uint32_t dgn_change_seq;
    uint32_t field_change_seq[32];
    int64_t last_update_us;
} tracked_state_t;
