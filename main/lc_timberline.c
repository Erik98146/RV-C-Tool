#include "lc_timberline.h"

// Temperature encoding is RV-C's standard uint16: conv 0.03125, offset -273
// (Table 5.3 in the Timberline document, i.e. the ordinary RV-C scale) —
// copied from the generated 1FF9C "Temperature" row so a Timberline
// temperature decodes identically to every other RV-C temperature and needs
// no special handling anywhere downstream.
#define TL_TEMP_CONV    0.03125
#define TL_TEMP_OFFSET  (-273)

// ── 0x84 — Extension status (broadcast every 5 s) ───────────────────────────
// Byte 0 is the message type; the layout below starts at byte 1.
static const value_desc_t TL_84_SOLENOID_VALUES[] = {
    { 0ULL, "Off" },
    { 1ULL, "On" },
};

static const value_desc_t TL_84_SENSOR_VALUES[] = {
    { 0ULL, "External Sensor" },
    { 1ULL, "Panel Sensor" },
};

static const field_desc_t TL_84_FIELDS[] = {
    { "Message Type",           0, 0, 0, 7, FIELD_UINT, "hex",   "False", NULL, 0, 1, 0, false },
    { "Solenoid",               1, 1, 0, 1, FIELD_UINT, "",      "False", TL_84_SOLENOID_VALUES, 2, 1, 0, false },
    { "Temperature Sensor",     1, 1, 2, 3, FIELD_UINT, "",      "False", TL_84_SENSOR_VALUES,   2, 1, 0, false },
    { "Tank Temperature",       2, 3, 0, 7, FIELD_UINT, "Deg C", "True",  NULL, 0, TL_TEMP_CONV, TL_TEMP_OFFSET, false },
    { "Heater Temperature",     4, 5, 0, 7, FIELD_UINT, "Deg C", "True",  NULL, 0, TL_TEMP_CONV, TL_TEMP_OFFSET, false },
    { "Fan Manual Percent",     6, 6, 0, 7, FIELD_UINT, "Pct",   "False", NULL, 0, 1, 0, false },
};

// ── 0x85 — Timers (broadcast every 1 s) ─────────────────────────────────────
// Included because the hydronic card's shutdown behaviour is timer-driven and
// "why did it turn off" is otherwise unanswerable. ⚠ 1 Hz is fast for a
// tracked DGN, so these are decoded but the card should treat them as
// secondary — see the note in the header about broadcast rates.
static const field_desc_t TL_85_FIELDS[] = {
    { "Message Type",           0, 0, 0, 7, FIELD_UINT, "hex", "False", NULL, 0, 1, 0, false },
    { "System Timer",           1, 3, 0, 7, FIELD_UINT, "s",   "True",  NULL, 0, 1, 0, false },
    { "Domestic Water Timer",   4, 5, 0, 7, FIELD_UINT, "s",   "False", NULL, 0, 1, 0, false },
    { "Pump Override Timer",    6, 7, 0, 7, FIELD_UINT, "s",   "False", NULL, 0, 1, 0, false },
};

// ── 0x8A — Timer setup status (5 s / on change) ─────────────────────────────
static const field_desc_t TL_8A_FIELDS[] = {
    { "Message Type",           0, 0, 0, 7, FIELD_UINT, "hex", "False", NULL, 0, 1, 0, false },
    { "System Limitation",      1, 2, 0, 7, FIELD_UINT, "min", "False", NULL, 0, 1, 0, false },
    { "Water Limitation",       3, 3, 0, 7, FIELD_UINT, "min", "False", NULL, 0, 1, 0, false },
};

// One descriptor per decoded variant. `dgn` stays 1EF65 for all of them —
// tracked state, publish config and card bindings key on (DGN, instance,
// source), and every variant genuinely comes from the same DGN on the same
// device. What separates them is the field NAMES, which are unique across
// variants precisely so a binding can never resolve to the wrong one.
static const dgn_desc_t TL_DESC_84 = {
    0x1EF65u, "TIMBERLINE_EXT_STATUS", false, "", TL_84_FIELDS,
    sizeof(TL_84_FIELDS) / sizeof(TL_84_FIELDS[0])
};
static const dgn_desc_t TL_DESC_85 = {
    0x1EF65u, "TIMBERLINE_TIMERS", false, "", TL_85_FIELDS,
    sizeof(TL_85_FIELDS) / sizeof(TL_85_FIELDS[0])
};
static const dgn_desc_t TL_DESC_8A = {
    0x1EF65u, "TIMBERLINE_TIMER_SETUP", false, "", TL_8A_FIELDS,
    sizeof(TL_8A_FIELDS) / sizeof(TL_8A_FIELDS[0])
};

const dgn_desc_t *lc_timberline_desc_for_frame(const uint8_t *data, uint8_t len)
{
    if (data == NULL || len < 1) return NULL;
    switch (data[0]) {
    case 0x84: return &TL_DESC_84;
    case 0x85: return &TL_DESC_85;
    case 0x8A: return &TL_DESC_8A;
    // 0x81/0x83/0x89 are commands (we send, never receive) and 0x86/0x87/0x88
    // are version strings with nothing worth tracking. Falling through to NULL
    // leaves the generic descriptor in place, which still decodes Message Type
    // — so an unknown variant shows up honestly in the sniffer rather than
    // being silently dropped or, worse, decoded with the wrong layout.
    default:   return NULL;
    }
}
