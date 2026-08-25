#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_err.h"

#include "lc_config.h"
#include "lc_types.h"

extern active_dgn_state_t active_dgns[MAX_ACTIVE_DGNS];
extern active_dgn_state_t unknown_active_dgns[MAX_UNKNOWN_ACTIVE_DGNS];
extern discovery_instance_state_t discovery_active_instances[MAX_DISCOVERY_ACTIVE_INSTANCES];
extern tracked_state_t tracked_states[MAX_TRACKED_STATES];
extern uint32_t global_change_seq;

esp_err_t lc_state_init(void);
void lc_state_lock(void);
void lc_state_unlock(void);

bool lc_state_is_select_all_mode(void);
void lc_state_set_select_all_mode(bool enabled);
void lc_state_increment_change_seq(void);
void lc_state_clear_tracked(void);
void lc_state_clear_discovery(size_t watched_dgn_count);
void lc_state_clear_all(size_t watched_dgn_count);

active_dgn_state_t *get_unknown_active_state(uint32_t dgn);
active_dgn_state_t *get_active_dgn_state(uint32_t dgn);
void lc_state_note_discovery_instance(uint32_t dgn, const char *name, uint8_t source_id, int16_t instance);
tracked_state_t *get_tracked_state(uint32_t dgn, int16_t instance, uint8_t source_id);

// ── tracked_states occupancy, surfaced in /api/state's diag block ──────────
// The table never evicts: slots are first-come-first-served for the life of a
// boot, so once it fills, a device that has not yet transmitted can NEVER
// appear. A switch/dimmer only broadcasts on change, so it is exactly the kind
// of device that loses that race — and the visible symptom is a control
// silently vanishing from a Mini display. This happened for real (2026-08-11,
// 81 tuples against 64 slots) and recurred, and both times it was hard to
// confirm because NEITHER number below was observable from the UI: the
// full-counter existed but was written and never read.
//
// ⚠ lc_state_tracked_used() takes the state lock itself — do NOT call it with
// lc_state_lock() already held.
size_t   lc_state_tracked_used(void);
uint32_t lc_state_tracked_full_events(void);
void     lc_state_note_tracked_full(void);
bool frame_changed(tracked_state_t *state, const rvc_rx_frame_t *frame);
void update_tracked_state(tracked_state_t *state, const rvc_rx_frame_t *frame);
