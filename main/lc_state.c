#include <stddef.h>
#include <string.h>

#include "esp_err.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "lc_can.h"
#include "lc_state.h"

static SemaphoreHandle_t s_state_mutex = NULL;
static bool s_select_all_mode = false;

active_dgn_state_t active_dgns[MAX_ACTIVE_DGNS];
active_dgn_state_t unknown_active_dgns[MAX_UNKNOWN_ACTIVE_DGNS];
discovery_instance_state_t discovery_active_instances[MAX_DISCOVERY_ACTIVE_INSTANCES];
tracked_state_t tracked_states[MAX_TRACKED_STATES];
uint32_t global_change_seq = 0;

esp_err_t lc_state_init(void)
{
    if (s_state_mutex != NULL) {
        return ESP_OK;
    }

    s_state_mutex = xSemaphoreCreateMutex();
    if (s_state_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create state mutex");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

void lc_state_lock(void)
{
    if (s_state_mutex != NULL) {
        xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    }
}

void lc_state_unlock(void)
{
    if (s_state_mutex != NULL) {
        xSemaphoreGive(s_state_mutex);
    }
}

bool lc_state_is_select_all_mode(void)
{
    return s_select_all_mode;
}

void lc_state_set_select_all_mode(bool enabled)
{
    s_select_all_mode = enabled;
}

void lc_state_increment_change_seq(void)
{
    global_change_seq++;
}

void lc_state_clear_tracked(void)
{
    memset(tracked_states, 0, sizeof(tracked_states));
    lc_state_increment_change_seq();

    if (lc_can_rx_queue != NULL) {
        xQueueReset(lc_can_rx_queue);
    }
}

void lc_state_clear_discovery(size_t watched_dgn_count)
{
    // active_dgns is now a compact slot pool (not indexed by watched-DGN position),
    // so clear the whole array regardless of the watched-DGN count.
    (void)watched_dgn_count;

    memset(active_dgns, 0, sizeof(active_dgns));
    memset(unknown_active_dgns, 0, sizeof(unknown_active_dgns));
    memset(discovery_active_instances, 0, sizeof(discovery_active_instances));
}

void lc_state_clear_all(size_t watched_dgn_count)
{
    lc_state_clear_discovery(watched_dgn_count);
    lc_state_clear_tracked();
}


void lc_state_note_discovery_instance(uint32_t dgn, const char *name, uint8_t source_id, int16_t instance)
{
    if (instance < 0) {
        return;
    }

    discovery_instance_state_t *slot = NULL;
    discovery_instance_state_t *empty = NULL;
    discovery_instance_state_t *oldest = NULL;
    int64_t now_us = esp_timer_get_time();

    for (size_t i = 0; i < MAX_DISCOVERY_ACTIVE_INSTANCES; i++) {
        discovery_instance_state_t *candidate = &discovery_active_instances[i];
        if (candidate->seen) {
            if (candidate->dgn == dgn &&
                candidate->source_id == source_id &&
                candidate->instance == instance) {
                slot = candidate;
                break;
            }
            if (oldest == NULL || candidate->last_seen_us < oldest->last_seen_us) {
                oldest = candidate;
            }
        } else if (empty == NULL) {
            empty = candidate;
        }
    }

    if (slot == NULL) {
        slot = empty != NULL ? empty : oldest;
    }
    if (slot == NULL) {
        return;
    }

    if (!slot->seen || slot->dgn != dgn || slot->source_id != source_id || slot->instance != instance) {
        memset(slot, 0, sizeof(*slot));
        slot->seen = true;
        slot->dgn = dgn;
        slot->name = name;
        slot->source_id = source_id;
        slot->instance = instance;
    }

    slot->count++;
    slot->last_seen_us = now_us;
}

active_dgn_state_t *get_unknown_active_state(uint32_t dgn)
{
    active_dgn_state_t *empty = NULL;

    for (size_t i = 0; i < MAX_UNKNOWN_ACTIVE_DGNS; i++) {
        if (unknown_active_dgns[i].seen && unknown_active_dgns[i].dgn == dgn) {
            return &unknown_active_dgns[i];
        }

        if (!unknown_active_dgns[i].seen && empty == NULL) {
            empty = &unknown_active_dgns[i];
        }
    }

    if (empty != NULL) {
        memset(empty, 0, sizeof(*empty));
        empty->seen = true;
        empty->dgn = dgn;
    }

    return empty;
}

// Compact allocator for the discovery/sniffer activity view. Returns the existing
// slot for `dgn` (preserving its count/timestamps), else the first empty slot,
// else evicts the least-recently-active slot (smallest last_sent_us). Only returns
// NULL if MAX_ACTIVE_DGNS is 0. Mirrors get_unknown_active_state with oldest-
// eviction so a busy bus shows the most-recent active DGNs rather than dropping new
// ones. Callers hold lc_state_lock().
active_dgn_state_t *get_active_dgn_state(uint32_t dgn)
{
    active_dgn_state_t *empty  = NULL;
    active_dgn_state_t *oldest = NULL;

    for (size_t i = 0; i < MAX_ACTIVE_DGNS; i++) {
        active_dgn_state_t *c = &active_dgns[i];
        if (c->seen) {
            if (c->dgn == dgn) {
                return c;  // existing entry — preserve count/last_sent_us
            }
            if (oldest == NULL || c->last_sent_us < oldest->last_sent_us) {
                oldest = c;
            }
        } else if (empty == NULL) {
            empty = c;
        }
    }

    active_dgn_state_t *slot = (empty != NULL) ? empty : oldest;
    if (slot == NULL) {
        return NULL;
    }

    memset(slot, 0, sizeof(*slot));
    slot->seen = true;
    slot->dgn  = dgn;
    return slot;
}

static uint32_t s_tracked_full_events = 0;

size_t lc_state_tracked_used(void)
{
    size_t n = 0;
    lc_state_lock();
    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        if (tracked_states[i].valid) n++;
    }
    lc_state_unlock();
    return n;
}

uint32_t lc_state_tracked_full_events(void) { return s_tracked_full_events; }

// Called from the CAN dispatch loop when a frame is dropped because no slot
// was free. Plain increment: it is only ever written from that one task.
void lc_state_note_tracked_full(void) { s_tracked_full_events++; }

tracked_state_t *get_tracked_state(uint32_t dgn, int16_t instance, uint8_t source_id)
{
    tracked_state_t *empty = NULL;

    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        if (tracked_states[i].valid) {
            if (tracked_states[i].dgn == dgn &&
                tracked_states[i].instance == instance &&
                tracked_states[i].source_id == source_id) {
                return &tracked_states[i];
            }
        } else if (empty == NULL) {
            empty = &tracked_states[i];
        }
    }

    if (empty != NULL) {
        memset(empty, 0, sizeof(*empty));
        empty->valid = true;
        empty->dgn = dgn;
        empty->instance = instance;
        empty->source_id = source_id;
        return empty;
    }

    return NULL;
}

bool frame_changed(tracked_state_t *state, const rvc_rx_frame_t *frame)
{
    if (state == NULL) {
        return true;
    }

    if (state->len != frame->len) {
        return true;
    }

    return memcmp(state->data, frame->data, frame->len) != 0;
}

void update_tracked_state(tracked_state_t *state, const rvc_rx_frame_t *frame)
{
    if (state == NULL) {
        return;
    }

    state->source_id = (uint8_t)(frame->id & 0xFF);
    state->len = frame->len;
    memset(state->data, 0, sizeof(state->data));
    memcpy(state->data, frame->data, frame->len);
    state->has_snapshot = true;
    state->last_update_us = esp_timer_get_time();
}
