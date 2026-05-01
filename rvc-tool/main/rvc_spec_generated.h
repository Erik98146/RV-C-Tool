#ifndef RVC_SPEC_GENERATED_H
#define RVC_SPEC_GENERATED_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    FIELD_UINT,
    FIELD_BITS,
    FIELD_ASCII,
    FIELD_RAW
} field_type_t;

typedef struct {
    uint64_t value;
    const char *name;
} value_desc_t;

typedef struct {
    const char *name;
    uint8_t byte_start;
    uint8_t byte_end;
    uint8_t bit_start;
    uint8_t bit_end;
    field_type_t type;
    const char *unit;
    const value_desc_t *values;
    uint16_t value_count;
    double conv;
    double offset;
    bool inv;
} field_desc_t;

typedef struct {
    uint32_t dgn;
    const char *name;
    const field_desc_t *fields;
    uint16_t field_count;
} dgn_desc_t;

static const value_desc_t RVC_DGN_000E8_FIELD_0_VALUES[] = {
    { 0ULL, "ack. command will be executed" },
    { 1ULL, "nak. command will not be executed" },
    { 2ULL, "command is not acceptable from this source" },
    { 3ULL, "conditions do not allow command to be executed" },
    { 4ULL, "command is not formatted properly" },
    { 5ULL, "command parameters are out of range" },
    { 6ULL, "command requires a security password" },
    { 7ULL, "command requires more time to execute" },
    { 8ULL, "command overridden by user" },
    { 9ULL, "reserved" },
    { 127ULL, "reserved" },
    { 128ULL, "command-specific response" },
    { 254ULL, "command-specific response" },
};
static const field_desc_t RVC_DGN_000E8_FIELDS[] = {
    { "acknowledgment code", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_000E8_FIELD_0_VALUES, 13, 1, 0, false },
    { "instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "instance bank", 2, 2, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "source address", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "DGN Acknowledged", 5, 7, 0, 7, FIELD_UINT, "hex", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_000EA_FIELDS[] = {
    { "desired dgn", 0, 2, 0, 7, FIELD_UINT, "hex", NULL, 0, 1, 0, false },
    { "instance", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "instance bank or secondary instance", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_0016C_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "driver index", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "on cycle count", 2, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "channel on time", 4, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_0016F_FIELD_4_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_0016F_FIELD_5_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_0016F_FIELD_6_VALUES[] = {
    { 0ULL, "ok" },
    { 1ULL, "output disabled" },
};
static const value_desc_t RVC_DGN_0016F_FIELD_7_VALUES[] = {
    { 0ULL, "manual" },
    { 1ULL, "automatic" },
};
static const value_desc_t RVC_DGN_0016F_FIELD_8_VALUES[] = {
    { 0ULL, "output not disabled" },
    { 1ULL, "over current" },
    { 2ULL, "short circuit" },
};
static const field_desc_t RVC_DGN_0016F_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "driver index", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "voltage", 2, 3, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "current", 4, 5, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "output status", 6, 6, 0, 1, FIELD_UINT, "", RVC_DGN_0016F_FIELD_4_VALUES, 2, 1, 0, false },
    { "desired status", 6, 6, 2, 3, FIELD_UINT, "", RVC_DGN_0016F_FIELD_5_VALUES, 2, 1, 0, false },
    { "shutdown status", 6, 6, 4, 5, FIELD_UINT, "", RVC_DGN_0016F_FIELD_6_VALUES, 2, 1, 0, false },
    { "reset type", 6, 6, 6, 7, FIELD_UINT, "", RVC_DGN_0016F_FIELD_7_VALUES, 2, 1, 0, false },
    { "reason for shutdown", 7, 7, 0, 7, FIELD_UINT, "", RVC_DGN_0016F_FIELD_8_VALUES, 3, 1, 0, false },
};

static const field_desc_t RVC_DGN_0017D_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0017E_FIELDS[] = {
    { "msg", 0, 7, 0, 7, FIELD_ASCII, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_0017F_FIELD_0_VALUES[] = {
    { 0ULL, "no action" },
    { 1ULL, "Reboot" },
};
static const value_desc_t RVC_DGN_0017F_FIELD_1_VALUES[] = {
    { 0ULL, "no action" },
    { 1ULL, "clear faults" },
};
static const value_desc_t RVC_DGN_0017F_FIELD_2_VALUES[] = {
    { 0ULL, "no action" },
    { 1ULL, "reset to default settings" },
};
static const value_desc_t RVC_DGN_0017F_FIELD_3_VALUES[] = {
    { 0ULL, "no action" },
    { 1ULL, "reset statistics" },
};
static const field_desc_t RVC_DGN_0017F_FIELDS[] = {
    { "reboot", 0, 0, 0, 1, FIELD_BITS, "", RVC_DGN_0017F_FIELD_0_VALUES, 2, 1, 0, false },
    { "clear faults", 0, 0, 2, 3, FIELD_BITS, "", RVC_DGN_0017F_FIELD_1_VALUES, 2, 1, 0, false },
    { "reset to default settings", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_0017F_FIELD_2_VALUES, 2, 1, 0, false },
    { "reset statistics", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_0017F_FIELD_3_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_0BFC1_FIELD_0_VALUES[] = {
    { 1ULL, "fresh water" },
    { 2ULL, "gray waste" },
    { 3ULL, "black waste" },
    { 4ULL, "second gray waste" },
};
static const field_desc_t RVC_DGN_0BFC1_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_0BFC1_FIELD_0_VALUES, 4, 1, 0, false },
    { "tank level", 1, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_0EBFF_FIELDS[] = {
    { "packet number", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "data", 1, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_0ECFF_FIELDS[] = {
    { "reserved_byte0", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "message length", 1, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "packet count", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "reserved_byte4", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dgn lsb", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dgn isb", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dgn msb", 7, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_0EE00_FIELD_12_VALUES[] = {
    { 0ULL, "node supports address claiming" },
    { 1ULL, "node uses fixed source address" },
};
static const field_desc_t RVC_DGN_0EE00_FIELDS[] = {
    { "serial number-lsb", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "serial number", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "serial number-msb", 2, 2, 0, 4, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "manufacturer code-lsb", 2, 2, 5, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "manufacturer code", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "node instance", 4, 4, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "function instance", 4, 4, 3, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "compatibility field 0", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "reserved", 6, 6, 0, 0, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "compatibility field 1", 6, 6, 1, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "compatibility field 2", 7, 7, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "compatibility field 3", 7, 7, 4, 6, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "arbitrary address capable", 7, 7, 7, 7, FIELD_UINT, "", RVC_DGN_0EE00_FIELD_12_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_0EEFF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0EF70_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0EF80_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0F004_FIELDS[] = {
    { "output current ?", 4, 5, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_0FC08_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0FECA_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0FED5_FIELDS[] = {
    { "alternator speed", 0, 1, 0, 7, FIELD_UINT, "j1939rpm", NULL, 0, 1, 0, false },
    { "alternator 1 status", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_0FEEB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0FEF3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_0FF01_FIELDS[] = {
};

static const field_desc_t RVC_DGN_10FFD_FIELDS[] = {
};

static const value_desc_t RVC_DGN_15FCE_FIELD_0_VALUES[] = {
    { 1ULL, "involves aes enabled" },
    { 3ULL, "involves aes enabled" },
    { 5ULL, "aes state machine" },
    { 0ULL, "time at start volts" },
    { 0ULL, "stop at voltage" },
    { 0ULL, "time at stop volts" },
    { 15ULL, "ags config (gen start retries / mode)" },
    { 16ULL, "max engine run time" },
    { 0ULL, "quiet time start" },
    { 0ULL, "quiet time stop" },
    { 0ULL, "ags low volts trigger" },
    { 31ULL, "start at voltage" },
    { 0ULL, "involves aes enabled" },
    { 0ULL, "0%-33% threshold" },
    { 0ULL, "33%-67% threshold" },
    { 0ULL, "67%-100% threshold" },
    { 0ULL, "number of go power! controllers" },
    { 0ULL, "number of batteries" },
    { 0ULL, "cargo/bath light ch.25" },
    { 0ULL, "bunk accent" },
    { 0ULL, "progressive inverter" },
    { 0ULL, "bath fan" },
    { 0ULL, "black tank" },
    { 0ULL, "generator/aes mode" },
    { 0ULL, "selected floorplan" },
    { 0ULL, "ags time between retries (sec)" },
};
static const field_desc_t RVC_DGN_15FCE_FIELDS[] = {
    { "message type", 0, 0, 0, 7, FIELD_UINT, "hex", RVC_DGN_15FCE_FIELD_0_VALUES, 26, 1, 0, false },
};

static const field_desc_t RVC_DGN_1AAB7_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1AADC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1AAFD_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1EE00_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1EF65_FIELD_0_VALUES[] = {
    { 81ULL, "clear errors command" },
    { 83ULL, "extension command" },
    { 84ULL, "extension status" },
    { 85ULL, "timers" },
    { 86ULL, "heater info" },
    { 87ULL, "panel info" },
    { 88ULL, "hcu info" },
    { 89ULL, "timers setup command" },
    { 0ULL, "timers setup status" },
};
static const field_desc_t RVC_DGN_1EF65_FIELDS[] = {
    { "message type", 0, 0, 0, 7, FIELD_UINT, "hex", RVC_DGN_1EF65_FIELD_0_VALUES, 9, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FACE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FACF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FBDA_FIELDS[] = {
    { "active_input_code", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "aux_12v_active", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDCB_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "control/power board temperature", 1, 2, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "capacitor temperature", 3, 4, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "ambient temperature", 5, 6, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDE2_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FDE3_FIELD_1_VALUES[] = {
    { 0ULL, "stopped" },
    { 1ULL, "opening" },
    { 2ULL, "closing" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_3_VALUES[] = {
    { 0ULL, "no rain detected" },
    { 1ULL, "rain detected" },
    { 10ULL, "sensor error" },
    { 11ULL, "rain sensor not installed" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_4_VALUES[] = {
    { 0ULL, "rain sensor used (auto)" },
    { 1ULL, "rain sensor not used" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_5_VALUES[] = {
    { 0ULL, "dome not automatically controlled" },
    { 1ULL, "dome automatically controlled" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_6_VALUES[] = {
    { 0ULL, "dome will stay open when fan shuts off" },
    { 1ULL, "dome will automatically close" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_7_VALUES[] = {
    { 0ULL, "fan will not automatically shut off when dome closes" },
    { 1ULL, "fan will automatically shut off when dome closes" },
};
static const value_desc_t RVC_DGN_1FDE3_FIELD_8_VALUES[] = {
    { 0ULL, "1/2 percent step resolution" },
    { 1ULL, "1 percent step resolution" },
    { 111111ULL, "fan speed increment/decrement not supported" },
};
static const field_desc_t RVC_DGN_1FDE3_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dome mode", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FDE3_FIELD_1_VALUES, 3, 1, 0, false },
    { "dome position", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "rain sensor", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_3_VALUES, 4, 1, 0, false },
    { "rain sensor override", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_4_VALUES, 2, 1, 0, false },
    { "setpoint controlled dome state", 3, 3, 4, 5, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_5_VALUES, 2, 1, 0, false },
    { "auto close dome on fan off", 3, 3, 6, 7, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_6_VALUES, 2, 1, 0, false },
    { "auto fan off on dome close", 4, 4, 0, 1, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_7_VALUES, 2, 1, 0, false },
    { "fan steps supported", 4, 4, 2, 7, FIELD_BITS, "", RVC_DGN_1FDE3_FIELD_8_VALUES, 3, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDF9_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FDFA_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "battery high voltage limit return voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "battery low voltage limit return voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "battery low voltage limit time delay", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "absorption duration", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "temperature compensation factor", 7, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDFB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FDFC_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "under-voltage warning voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "battery high voltage limit voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "battery low voltage limit voltage", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDFD_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FDFE_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "bulk-absorption voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "float voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "charge return voltage", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FDFF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "solar array measured voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "solar array measured current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE80_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charger priority", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "measured voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "measured current", 5, 6, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "measured temperature", 7, 7, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE81_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "total number of operating days", 1, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "solar charge controller measured temperature", 3, 4, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE82_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "last 7 days amp-hours to battery", 1, 2, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "cumulative power generation", 3, 6, 0, 7, FIELD_UINT, "kwh", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE83_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "today's amp-hours to battery", 1, 2, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "yesterday's amp-hours to battery", 3, 4, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "day before yesterday's amp-hours to battery", 5, 6, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE84_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "rated solar input voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "rated solar input current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "rated solar over-power", 5, 6, 0, 7, FIELD_UINT, "w", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE85_FIELD_5_VALUES[] = {
    { 0ULL, "battery type does not support user definable charge parameters" },
    { 1ULL, "battery type supports user definable charge parameters" },
};
static const value_desc_t RVC_DGN_1FE85_FIELD_6_VALUES[] = {
    { 0ULL, "battery type does not support user definable charge parameters" },
    { 1ULL, "battery type supports user definable charge parameters" },
};
static const field_desc_t RVC_DGN_1FE85_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "rated battery voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "rated battery current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "supported battery types", 5, 5, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "supported battery types", 6, 6, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "vendor-defined proprietary type 1", 7, 7, 0, 1, FIELD_UINT, "", RVC_DGN_1FE85_FIELD_5_VALUES, 2, 1, 0, false },
    { "vendor-defined proprietary type 2", 7, 7, 2, 3, FIELD_UINT, "", RVC_DGN_1FE85_FIELD_6_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE8B_FIELD_2_VALUES[] = {
    { 0ULL, "battery discharge bus disconnected" },
    { 1ULL, "battery discharge bus connected" },
};
static const value_desc_t RVC_DGN_1FE8B_FIELD_3_VALUES[] = {
    { 0ULL, "charge bus disconnected" },
    { 1ULL, "charge bus connected" },
};
static const value_desc_t RVC_DGN_1FE8B_FIELD_4_VALUES[] = {
    { 0ULL, "no charge detected" },
    { 1ULL, "charge detected" },
};
static const value_desc_t RVC_DGN_1FE8B_FIELD_5_VALUES[] = {
    { 0ULL, "battery charge is above reserve level" },
    { 1ULL, "battery charge is at or below reserve level" },
};
static const field_desc_t RVC_DGN_1FE8B_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "discharge on/off status", 2, 2, 0, 1, FIELD_BITS, "", RVC_DGN_1FE8B_FIELD_2_VALUES, 2, 1, 0, false },
    { "charge on/off status", 2, 2, 2, 3, FIELD_BITS, "", RVC_DGN_1FE8B_FIELD_3_VALUES, 2, 1, 0, false },
    { "charge detected", 2, 2, 4, 5, FIELD_BITS, "", RVC_DGN_1FE8B_FIELD_4_VALUES, 2, 1, 0, false },
    { "reserve status", 2, 2, 6, 7, FIELD_BITS, "", RVC_DGN_1FE8B_FIELD_5_VALUES, 2, 1, 0, false },
    { "full capacity", 3, 4, 0, 7, FIELD_UINT, "Ah", NULL, 0, 1, 0, false },
    { "dc power", 5, 6, 0, 7, FIELD_UINT, "W", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE8C_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "last 7 days input amp hours", 2, 3, 0, 7, FIELD_UINT, "Ah", NULL, 0, 1, 0, false },
    { "last 7 days output amp hours", 4, 5, 0, 7, FIELD_UINT, "Ah", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE8D_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FE8E_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FE8F_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FE90_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FE91_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FE92_FIELD_5_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium-iron-phosphate" },
};
static const field_desc_t RVC_DGN_1FE92_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "desired charge state", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "desired dc voltage", 3, 4, 0, 7, FIELD_UINT, "V", NULL, 0, 1, 0, false },
    { "desired dc current", 5, 6, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "battery type", 7, 7, 0, 3, FIELD_BITS, "", RVC_DGN_1FE92_FIELD_5_VALUES, 4, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE93_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FE94_FIELD_5_VALUES[] = {
    { 0ULL, "time to empty" },
    { 1ULL, "time to full" },
    { 11ULL, "time to empty" },
};
static const field_desc_t RVC_DGN_1FE94_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "source temperature", 2, 3, 0, 7, FIELD_UINT, "C", NULL, 0, 1, 0, false },
    { "state of charge", 4, 4, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "time remaining", 5, 6, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "time remaining interpretation", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FE94_FIELD_5_VALUES, 3, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FE95_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc voltage", 2, 3, 0, 7, FIELD_UINT, "V", NULL, 0, 1, 0, false },
    { "dc current", 4, 7, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE96_FIELD_1_VALUES[] = {
    { 0ULL, "off (end of test)" },
    { 65ULL, "test (forced on)" },
};
static const field_desc_t RVC_DGN_1FE96_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "output mode", 1, 1, 0, 3, FIELD_BITS, "", RVC_DGN_1FE96_FIELD_1_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE97_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
    { 65ULL, "test (forced on)" },
};
static const value_desc_t RVC_DGN_1FE97_FIELD_2_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FE97_FIELD_3_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FE97_FIELD_4_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const field_desc_t RVC_DGN_1FE97_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "output status", 1, 1, 0, 3, FIELD_BITS, "", RVC_DGN_1FE97_FIELD_1_VALUES, 3, 1, 0, false },
    { "pump overcurrent status", 2, 2, 0, 1, FIELD_BITS, "", RVC_DGN_1FE97_FIELD_2_VALUES, 2, 1, 0, false },
    { "pump undercurrent status", 2, 2, 2, 3, FIELD_BITS, "", RVC_DGN_1FE97_FIELD_3_VALUES, 2, 1, 0, false },
    { "pump temperature status", 2, 2, 4, 5, FIELD_BITS, "", RVC_DGN_1FE97_FIELD_4_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE98_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
    { 65ULL, "test (forced on)" },
};
static const value_desc_t RVC_DGN_1FE98_FIELD_2_VALUES[] = {
    { 0ULL, "electric low enable" },
    { 1ULL, "electric low disable" },
    { 2ULL, "electric low toggle" },
    { 3ULL, "electric high enable" },
    { 4ULL, "electric high disable" },
    { 5ULL, "electric high toggle" },
    { 6ULL, "burner enable" },
    { 7ULL, "burner disable" },
    { 8ULL, "burner toggle" },
    { 9ULL, "electric cycle (low high off)" },
    { 10ULL, "electric cycle (high low off)" },
    { 11ULL, "electric low test toggle" },
    { 12ULL, "electric high test toggle" },
    { 13ULL, "burner test toggle" },
};
static const field_desc_t RVC_DGN_1FE98_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "engine preheat", 1, 1, 0, 3, FIELD_BITS, "", RVC_DGN_1FE98_FIELD_1_VALUES, 3, 1, 0, false },
    { "command", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FE98_FIELD_2_VALUES, 14, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE99_FIELD_3_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
    { 65ULL, "test (forced on)" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_4_VALUES[] = {
    { 0ULL, "coolant level sufficient" },
    { 1ULL, "coolant level low" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_5_VALUES[] = {
    { 0ULL, "domestic water priority" },
    { 1ULL, "heating priority" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_6_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_7_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_8_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_9_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_10_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_11_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_12_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_13_VALUES[] = {
    { 0ULL, "off (inactive)" },
    { 1ULL, "on (active)" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_14_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_15_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_16_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_17_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_18_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_19_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_20_VALUES[] = {
    { 0ULL, "off (inactive)" },
    { 1ULL, "on (active)" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_21_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_22_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_23_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const value_desc_t RVC_DGN_1FE99_FIELD_24_VALUES[] = {
    { 0ULL, "off (inactive)" },
    { 1ULL, "on (active)" },
};
static const field_desc_t RVC_DGN_1FE99_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "electric element level", 1, 1, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "max electric element level", 1, 1, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "engine preheat", 2, 2, 0, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_3_VALUES, 3, 1, 0, false },
    { "coolant level warning", 2, 2, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_4_VALUES, 2, 1, 0, false },
    { "hot water priority", 2, 2, 6, 7, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_5_VALUES, 2, 1, 0, false },
    { "output status burner", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_6_VALUES, 2, 1, 0, false },
    { "output status burner indicator", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_7_VALUES, 2, 1, 0, false },
    { "output status electric low", 3, 3, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_8_VALUES, 2, 1, 0, false },
    { "output status electric high", 3, 3, 6, 7, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_9_VALUES, 2, 1, 0, false },
    { "burner overcurrent status", 4, 4, 0, 1, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_10_VALUES, 2, 1, 0, false },
    { "burner undercurrent status", 4, 4, 2, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_11_VALUES, 2, 1, 0, false },
    { "burner temperature status", 4, 4, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_12_VALUES, 2, 1, 0, false },
    { "burner input status", 4, 4, 6, 7, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_13_VALUES, 2, 1, 0, false },
    { "burner indicator overcurrent status", 5, 5, 0, 1, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_14_VALUES, 2, 1, 0, false },
    { "burner indicator undercurrent status", 5, 5, 2, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_15_VALUES, 2, 1, 0, false },
    { "burner indicator temperature status", 5, 5, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_16_VALUES, 2, 1, 0, false },
    { "electric low element overcurrent status", 6, 6, 0, 1, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_17_VALUES, 2, 1, 0, false },
    { "electric low element undercurrent status", 6, 6, 2, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_18_VALUES, 2, 1, 0, false },
    { "electric low element temperature status", 6, 6, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_19_VALUES, 2, 1, 0, false },
    { "electric low element input status", 6, 6, 6, 7, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_20_VALUES, 2, 1, 0, false },
    { "electric high element overcurrent status", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_21_VALUES, 2, 1, 0, false },
    { "electric high element undercurrent status", 7, 7, 2, 3, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_22_VALUES, 2, 1, 0, false },
    { "electric high element temperature status", 7, 7, 4, 5, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_23_VALUES, 2, 1, 0, false },
    { "electric high element input status", 7, 7, 6, 7, FIELD_BITS, "", RVC_DGN_1FE99_FIELD_24_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FE9F_FIELD_2_VALUES[] = {
    { 0ULL, "alarm not triggered" },
    { 1ULL, "alarm is triggered" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_3_VALUES[] = {
    { 0ULL, "alarm condition is not being monitored" },
    { 1ULL, "alarm condition is being monitored" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_4_VALUES[] = {
    { 0ULL, "alarm has not been acknowledged" },
    { 1ULL, "alarm has been acknowledged" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_5_VALUES[] = {
    { 0ULL, "alarm will not automatically return to the ready state" },
    { 1ULL, "alarm will automatically return to the ready state once an \"ok\" condition is observed" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_8_VALUES[] = {
    { 0ULL, "full-time monitoring of an alarm is not required" },
    { 1ULL, "full-time monitoring of an alarm is required" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_9_VALUES[] = {
    { 0ULL, "do not log alarm now" },
    { 1ULL, "alarm should be logged" },
};
static const value_desc_t RVC_DGN_1FE9F_FIELD_10_VALUES[] = {
    { 0ULL, "do not notify user" },
    { 1ULL, "user notification required" },
};
static const field_desc_t RVC_DGN_1FE9F_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "DSA", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "alarm triggered", 2, 2, 0, 1, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_2_VALUES, 2, 1, 0, false },
    { "alarm ready", 2, 2, 2, 3, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_3_VALUES, 2, 1, 0, false },
    { "alarm is acknowledged", 2, 2, 4, 5, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_4_VALUES, 2, 1, 0, false },
    { "automatic reset enabled", 2, 2, 6, 7, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_5_VALUES, 2, 1, 0, false },
    { "elapsed time", 3, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "device instance", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "full time monitoring required", 6, 6, 0, 1, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_8_VALUES, 2, 1, 0, false },
    { "alarm for logging", 6, 6, 2, 3, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_9_VALUES, 2, 1, 0, false },
    { "alarm for user notification", 6, 6, 4, 5, FIELD_UINT, "", RVC_DGN_1FE9F_FIELD_10_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEA3_FIELDS[] = {
    { "charger instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charger priority", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charging voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "charging current", 5, 6, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "charger temperature", 7, 7, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEA5_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FEA6_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FEA6_FIELD_2_VALUES[] = {
    { 0ULL, "auto" },
    { 1ULL, "force on" },
};
static const value_desc_t RVC_DGN_1FEA6_FIELD_3_VALUES[] = {
    { 0ULL, "auto" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FEA6_FIELD_4_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FEA6_FIELD_6_VALUES[] = {
    { 0ULL, "air out" },
    { 1ULL, "air in" },
};
static const value_desc_t RVC_DGN_1FEA6_FIELD_7_VALUES[] = {
    { 0ULL, "close" },
    { 1ULL, "1/4 open" },
    { 8ULL, "1/2 open" },
    { 9ULL, "3/4 open" },
    { 64ULL, "open" },
    { 65ULL, "stop" },
};
static const field_desc_t RVC_DGN_1FEA6_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "system status", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_1_VALUES, 2, 1, 0, false },
    { "fan mode", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_2_VALUES, 2, 1, 0, false },
    { "speed mode", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_3_VALUES, 2, 1, 0, false },
    { "light", 1, 1, 6, 7, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_4_VALUES, 2, 1, 0, false },
    { "fan speed setting", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "wind direction setting", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_6_VALUES, 2, 1, 0, false },
    { "dome position", 3, 3, 2, 5, FIELD_BITS, "", RVC_DGN_1FEA6_FIELD_7_VALUES, 6, 1, 0, false },
    { "rain sensor", 3, 3, 6, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "ambient temperature", 4, 5, 0, 7, FIELD_UINT, "deg C", NULL, 0, 1, 0, false },
    { "set point", 6, 7, 0, 7, FIELD_UINT, "deg C", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEA7_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FEA7_FIELD_2_VALUES[] = {
    { 0ULL, "auto" },
    { 1ULL, "forced on" },
};
static const value_desc_t RVC_DGN_1FEA7_FIELD_3_VALUES[] = {
    { 0ULL, "auto" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FEA7_FIELD_4_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FEA7_FIELD_6_VALUES[] = {
    { 0ULL, "air out" },
    { 1ULL, "air in" },
};
static const value_desc_t RVC_DGN_1FEA7_FIELD_7_VALUES[] = {
    { 0ULL, "closed" },
    { 1ULL, "1/4 open" },
    { 8ULL, "1/2 open" },
    { 9ULL, "3/4 open" },
    { 64ULL, "open" },
};
static const field_desc_t RVC_DGN_1FEA7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "system status", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_1_VALUES, 2, 1, 0, false },
    { "fan status", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_2_VALUES, 2, 1, 0, false },
    { "speed mode", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_3_VALUES, 2, 1, 0, false },
    { "light", 1, 1, 6, 7, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_4_VALUES, 2, 1, 0, false },
    { "fan speed setting", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "wind direction setting", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_6_VALUES, 2, 1, 0, false },
    { "dome position", 3, 3, 2, 5, FIELD_BITS, "", RVC_DGN_1FEA7_FIELD_7_VALUES, 5, 1, 0, false },
    { "ambient temperature", 4, 5, 0, 7, FIELD_UINT, "deg C", NULL, 0, 1, 0, false },
    { "set point", 6, 7, 0, 7, FIELD_UINT, "deg C", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEAD_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEAE_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "equalization voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "equalization time", 3, 4, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "equalization interval", 5, 6, 0, 7, FIELD_UINT, "days", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEAF_FIELD_2_VALUES[] = {
    { 0ULL, "pre-sharging is not in process" },
    { 1ULL, "charger is charging the batteries to prepare for equalization" },
};
static const field_desc_t RVC_DGN_1FEAF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "time remaining", 1, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "pre-charging status", 3, 3, 0, 1, FIELD_UINT, "", RVC_DGN_1FEAF_FIELD_2_VALUES, 2, 1, 0, false },
    { "time since last equalization", 4, 4, 0, 7, FIELD_UINT, "days", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEB0_FIELD_3_VALUES[] = {
    { 0ULL, "no battery temperature sensor in use" },
    { 1ULL, "battery temperature sensor is present and active" },
};
static const value_desc_t RVC_DGN_1FEB0_FIELD_4_VALUES[] = {
    { 0ULL, "independent" },
    { 1ULL, "linked to dc source" },
};
static const value_desc_t RVC_DGN_1FEB0_FIELD_5_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium-iron-phosphate" },
    { 12ULL, "reserved for vendor-defined preprietary types" },
    { 13ULL, "reserved for vendor-defined preprietary types" },
};
static const field_desc_t RVC_DGN_1FEB0_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charging algorithm", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charger mode", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "default state on power-up", 3, 3, 0, 1, FIELD_UINT, "uint2", RVC_DGN_1FEB0_FIELD_3_VALUES, 2, 1, 0, false },
    { "linkage mode", 3, 3, 2, 3, FIELD_UINT, "uint2", RVC_DGN_1FEB0_FIELD_4_VALUES, 2, 1, 0, false },
    { "battery type", 3, 3, 4, 7, FIELD_UINT, "uint4", RVC_DGN_1FEB0_FIELD_5_VALUES, 6, 1, 0, false },
    { "battery bank size", 4, 5, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "maximum charge current", 7, 7, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEB1_FIELD_1_VALUES[] = {
    { 0ULL, "disable controller" },
    { 1ULL, "enable controller" },
    { 2ULL, "start equalization" },
    { 3ULL, "top up battery" },
};
static const value_desc_t RVC_DGN_1FEB1_FIELD_2_VALUES[] = {
    { 0ULL, "controller disabled on power-up" },
    { 1ULL, "controller enabled on power-up" },
};
static const value_desc_t RVC_DGN_1FEB1_FIELD_3_VALUES[] = {
    { 0ULL, "do nothing" },
    { 1ULL, "clear history" },
};
static const value_desc_t RVC_DGN_1FEB1_FIELD_4_VALUES[] = {
    { 0ULL, "charging is not forced" },
    { 1ULL, "force charge to bulk" },
    { 2ULL, "force charge to float" },
};
static const field_desc_t RVC_DGN_1FEB1_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "solar charge controller status", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FEB1_FIELD_1_VALUES, 4, 1, 0, false },
    { "default state on power-up", 2, 2, 0, 1, FIELD_UINT, "uint2", RVC_DGN_1FEB1_FIELD_2_VALUES, 2, 1, 0, false },
    { "clear history", 2, 2, 2, 3, FIELD_UINT, "uint2", RVC_DGN_1FEB1_FIELD_3_VALUES, 2, 1, 0, false },
    { "force charge", 2, 2, 4, 7, FIELD_UINT, "uint4", RVC_DGN_1FEB1_FIELD_4_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEB2_FIELD_3_VALUES[] = {
    { 0ULL, "no battery temperature sensor in use" },
    { 1ULL, "temperature sensor present and active" },
};
static const value_desc_t RVC_DGN_1FEB2_FIELD_4_VALUES[] = {
    { 0ULL, "independent" },
    { 1ULL, "linked to dc source" },
};
static const value_desc_t RVC_DGN_1FEB2_FIELD_5_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium-iron-phosphate" },
    { 12ULL, "reserved for vendor-defined preprietary types" },
    { 13ULL, "reserved for vendor-defined preprietary types" },
};
static const field_desc_t RVC_DGN_1FEB2_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charging algorithm", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "controller mode", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "battery sensor present", 3, 3, 0, 1, FIELD_UINT, "uint2", RVC_DGN_1FEB2_FIELD_3_VALUES, 2, 1, 0, false },
    { "linkage mode", 3, 3, 2, 3, FIELD_UINT, "uint2", RVC_DGN_1FEB2_FIELD_4_VALUES, 2, 1, 0, false },
    { "battery type", 3, 3, 4, 7, FIELD_UINT, "uint4", RVC_DGN_1FEB2_FIELD_5_VALUES, 6, 1, 0, false },
    { "battery bank size", 4, 5, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "maximum charge current", 7, 7, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEB3_FIELD_4_VALUES[] = {
    { 0ULL, "undefined" },
    { 1ULL, "do not charge" },
    { 2ULL, "bulk" },
    { 3ULL, "absorption" },
    { 4ULL, "overcharge" },
    { 5ULL, "equalize" },
    { 6ULL, "float" },
    { 7ULL, "constant voltage / current" },
};
static const value_desc_t RVC_DGN_1FEB3_FIELD_5_VALUES[] = {
    { 0ULL, "controller disabled" },
    { 1ULL, "controller enabled" },
};
static const value_desc_t RVC_DGN_1FEB3_FIELD_6_VALUES[] = {
    { 0ULL, "history not clear" },
    { 1ULL, "history cleared" },
};
static const value_desc_t RVC_DGN_1FEB3_FIELD_7_VALUES[] = {
    { 0ULL, "charging not forced" },
    { 1ULL, "force charge to bulk" },
    { 2ULL, "force charge to float" },
};
static const field_desc_t RVC_DGN_1FEB3_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charge voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "charge current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "charge current percent of maximum", 5, 5, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "operating state", 6, 6, 0, 7, FIELD_UINT, "uint8", RVC_DGN_1FEB3_FIELD_4_VALUES, 8, 1, 0, false },
    { "power-up state", 7, 7, 0, 1, FIELD_UINT, "uint2", RVC_DGN_1FEB3_FIELD_5_VALUES, 2, 1, 0, false },
    { "clear history", 7, 7, 2, 3, FIELD_UINT, "uint2", RVC_DGN_1FEB3_FIELD_6_VALUES, 2, 1, 0, false },
    { "force charge", 7, 7, 4, 7, FIELD_UINT, "uint2", RVC_DGN_1FEB3_FIELD_7_VALUES, 3, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEB4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEB5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEB6_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEB7_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEB8_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEB9_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEBA_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEBB_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEBC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEBD_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "fet 1 temperature", 1, 2, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "transformer temperature", 3, 4, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "fet 2 temperature", 5, 6, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEBE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEBF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "bulk time", 1, 2, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "absorption time", 3, 4, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "float time", 5, 6, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEC0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC1_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC2_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEC6_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charge voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "charge current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "charge current percent of maximum", 5, 5, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "operating state", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEC7_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FEC7_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const value_desc_t RVC_DGN_1FEC7_FIELD_2_VALUES[] = {
    { 0ULL, "not reached" },
    { 1ULL, "limit reached" },
};
static const value_desc_t RVC_DGN_1FEC7_FIELD_3_VALUES[] = {
    { 0ULL, "connected" },
    { 1ULL, "charge bus disconnected" },
};
static const value_desc_t RVC_DGN_1FEC7_FIELD_4_VALUES[] = {
    { 0ULL, "not reached" },
    { 1ULL, "limit reached" },
};
static const value_desc_t RVC_DGN_1FEC7_FIELD_5_VALUES[] = {
    { 0ULL, "connected" },
    { 1ULL, "charge bus disconnected" },
};
static const field_desc_t RVC_DGN_1FEC7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC7_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC7_FIELD_1_VALUES, 6, 1, 0, false },
    { "high voltage limit status", 2, 2, 0, 1, FIELD_BITS, "", RVC_DGN_1FEC7_FIELD_2_VALUES, 2, 1, 0, false },
    { "high voltage disconnect status", 2, 2, 2, 3, FIELD_BITS, "", RVC_DGN_1FEC7_FIELD_3_VALUES, 2, 1, 0, false },
    { "low voltage limit status", 2, 2, 4, 5, FIELD_BITS, "", RVC_DGN_1FEC7_FIELD_4_VALUES, 2, 1, 0, false },
    { "low voltage disconnect status", 2, 2, 6, 7, FIELD_BITS, "", RVC_DGN_1FEC7_FIELD_5_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEC8_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FEC8_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const field_desc_t RVC_DGN_1FEC8_FIELDS[] = {
    { "dc instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC8_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC8_FIELD_1_VALUES, 6, 1, 0, false },
    { "hp dc voltage", 2, 5, 0, 7, FIELD_UINT, "V", NULL, 0, 1, 0, false },
    { "DEPRECATED", 6, 7, 0, 7, FIELD_UINT, "mV/s", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEC9_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FEC9_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const value_desc_t RVC_DGN_1FEC9_FIELD_2_VALUES[] = {
    { 0ULL, "undefined" },
    { 1ULL, "do not charge" },
    { 2ULL, "bulk" },
    { 3ULL, "absorbtion" },
    { 4ULL, "overcharge" },
    { 5ULL, "equalize" },
    { 6ULL, "float" },
    { 7ULL, "constant voltage/current" },
};
static const value_desc_t RVC_DGN_1FEC9_FIELD_5_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium iron phosphate" },
};
static const field_desc_t RVC_DGN_1FEC9_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC9_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC9_FIELD_1_VALUES, 6, 1, 0, false },
    { "desired charge state", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FEC9_FIELD_2_VALUES, 8, 1, 0, false },
    { "desired dc voltage", 3, 4, 0, 7, FIELD_UINT, "V", NULL, 0, 1, 0, false },
    { "desired dc current", 5, 6, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "battery type", 7, 7, 0, 3, FIELD_UINT, "", RVC_DGN_1FEC9_FIELD_5_VALUES, 4, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FECA_FIELD_0_VALUES[] = {
    { 0ULL, "off fault" },
    { 1ULL, "off manual override running" },
    { 64ULL, "off normal" },
    { 65ULL, "on normal" },
};
static const value_desc_t RVC_DGN_1FECA_FIELD_6_VALUES[] = {
    { 0ULL, "Datum value above normal range" },
    { 1ULL, "Datum value below normal range" },
    { 2ULL, "Datum value erratic or invalid" },
    { 3ULL, "short circuit to high voltage" },
    { 4ULL, "short circuit to low voltage" },
    { 5ULL, "open circuit or output current lower" },
    { 6ULL, "grounded circuit or output current high" },
    { 7ULL, "mechanical device not responding" },
    { 8ULL, "datum value showing error of freq, pulse width, or period" },
    { 9ULL, "datum not updating at proper rate" },
    { 10ULL, "datum value fluctuating at abnormal rate" },
    { 11ULL, "failure not identifiable" },
    { 12ULL, "bad intelligent RV-C node" },
    { 13ULL, "calibration required" },
    { 14ULL, "none of the above" },
    { 15ULL, "datum valid but above normal mild" },
    { 16ULL, "datum valid but above normal moderate" },
    { 17ULL, "datum valid but below normal mild" },
    { 18ULL, "datum valid but below normal moderate" },
    { 19ULL, "received invalid network datum" },
    { 20ULL, "network device not responding" },
    { 21ULL, "networked device not responding" },
    { 22ULL, "reverse polarity" },
    { 23ULL, "reserved" },
    { 24ULL, "reserved" },
    { 25ULL, "reserved" },
    { 26ULL, "reserved" },
    { 27ULL, "reserved" },
    { 28ULL, "reserved" },
    { 29ULL, "reserved" },
    { 30ULL, "reserved" },
    { 31ULL, "failure mode not available" },
};
static const field_desc_t RVC_DGN_1FECA_FIELDS[] = {
    { "operating status", 0, 0, 0, 3, FIELD_BITS, "", RVC_DGN_1FECA_FIELD_0_VALUES, 4, 1, 0, false },
    { "yellow lamp status", 0, 0, 4, 5, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "red lamp status", 0, 0, 6, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "dsa", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "spn-msb", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "spn-isb", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "fmi", 4, 4, 0, 4, FIELD_UINT, "", RVC_DGN_1FECA_FIELD_6_VALUES, 32, 1, 0, false },
    { "spn-lsb", 4, 4, 5, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "occurrence count", 5, 5, 0, 6, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dsa extension", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "bank select", 7, 7, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FECB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FECC_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "bulk voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "absorption voltage", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "float voltage", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "temperature compensation constant", 7, 7, 0, 7, FIELD_UINT, "mV/K", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FECD_FIELD_2_VALUES[] = {
    { 0ULL, "stand-alone" },
    { 1ULL, "master" },
    { 2ULL, "slave" },
    { 3ULL, "line 2 master" },
};
static const field_desc_t RVC_DGN_1FECD_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source shutdown delay", 1, 2, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
    { "stack mode", 3, 3, 0, 7, FIELD_UINT, "", RVC_DGN_1FECD_FIELD_2_VALUES, 4, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FECE_FIELD_2_VALUES[] = {
    { 0ULL, "stand-alone" },
    { 1ULL, "master" },
    { 2ULL, "slave" },
    { 3ULL, "line 2 master" },
};
static const field_desc_t RVC_DGN_1FECE_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source shutdown delay", 1, 2, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
    { "stack mode", 3, 3, 0, 7, FIELD_UINT, "", RVC_DGN_1FECE_FIELD_2_VALUES, 4, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FECF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED1_FIELDS[] = {
    { "suspension system supply pressure", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "steer axle left air spring", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "steer axle right air spring", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "drive axle left air spring", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "drive axle right air spring", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "tag axle left air spring", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "tag axle left right spring", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FED2_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FED6_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FED7_FIELD_5_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FED7_FIELD_6_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FED7_FIELD_7_VALUES[] = {
    { 0ULL, "set brightness for led1 and led2" },
    { 1ULL, "led1 off, led2 off" },
    { 2ULL, "led1 on, led2 off" },
    { 3ULL, "led1 off, led2 on" },
    { 4ULL, "led1 on, led1 on" },
    { 17ULL, "ramp brightness to new value" },
    { 51ULL, "flash alternate" },
};
static const field_desc_t RVC_DGN_1FED7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "brightness", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "bank select", 3, 3, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "delay/duration", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "led1 status", 5, 5, 0, 1, FIELD_UINT, "", RVC_DGN_1FED7_FIELD_5_VALUES, 2, 1, 0, false },
    { "led2 status", 5, 5, 2, 3, FIELD_UINT, "", RVC_DGN_1FED7_FIELD_6_VALUES, 2, 1, 0, false },
    { "last command", 6, 6, 0, 7, FIELD_UINT, "", RVC_DGN_1FED7_FIELD_7_VALUES, 7, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FED8_FIELDS[] = {
    { "manufacturer code (LSB)", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "manufacturer code (MS bits)", 1, 1, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "function instance", 1, 1, 3, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "function", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "firmware revision", 3, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "configuration type (LSB)", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "configuration type", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "configuration type (MSB)", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "configuration revision", 7, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FED9_FIELD_5_VALUES[] = {
    { 0ULL, "set brightness for led1 and led2" },
    { 1ULL, "led1 off, led2 off" },
    { 2ULL, "led1 on, led2 off" },
    { 3ULL, "led1 off, led2 on" },
    { 4ULL, "led1 on, led1 on" },
    { 17ULL, "ramp brightness to new value" },
    { 51ULL, "flash alternate" },
};
static const field_desc_t RVC_DGN_1FED9_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "brightness", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "bank select", 3, 3, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "duration", 4, 4, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
    { "function", 6, 6, 0, 7, FIELD_UINT, "", RVC_DGN_1FED9_FIELD_5_VALUES, 7, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDA_FIELD_3_VALUES[] = {
    { 0ULL, "load is unlocked" },
    { 1ULL, "load is locked" },
    { 11ULL, "not supported" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_4_VALUES[] = {
    { 0ULL, "not overcurrent" },
    { 1ULL, "overcurrent" },
    { 11ULL, "unavailable" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_5_VALUES[] = {
    { 0ULL, "override inactive" },
    { 1ULL, "override active" },
    { 11ULL, "unavailable" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_6_VALUES[] = {
    { 0ULL, "enabled" },
    { 1ULL, "disabled" },
    { 11ULL, "status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_8_VALUES[] = {
    { 0ULL, "set brightness" },
    { 1ULL, "on duration" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 17ULL, "ramp brightness" },
    { 18ULL, "ramp toggle" },
    { 19ULL, "ramp up" },
    { 20ULL, "ramp down" },
    { 21ULL, "ramp up/down" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
    { 50ULL, "flash momentarily" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_9_VALUES[] = {
    { 0ULL, "not active" },
    { 1ULL, "active" },
    { 11ULL, "not supported" },
};
static const value_desc_t RVC_DGN_1FEDA_FIELD_10_VALUES[] = {
    { 0ULL, "operating status is zero" },
    { 1ULL, "operating status is non-zero or flashing" },
    { 11ULL, "command is not supported" },
};
static const field_desc_t RVC_DGN_1FEDA_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "operating status brightness", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 0.5, 0, false },
    { "lock status", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_3_VALUES, 3, 1, 0, false },
    { "overcurrent status", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_4_VALUES, 3, 1, 0, false },
    { "override status", 3, 3, 4, 5, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_5_VALUES, 3, 1, 0, false },
    { "enable status", 3, 3, 6, 7, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_6_VALUES, 3, 1, 0, false },
    { "delay/duration", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "last command", 5, 5, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDA_FIELD_8_VALUES, 16, 1, 0, false },
    { "interlock status", 6, 6, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_9_VALUES, 3, 1, 0, false },
    { "load status", 6, 6, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDA_FIELD_10_VALUES, 3, 1, 0, false },
    { "master memory value", 7, 7, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDB_FIELD_3_VALUES[] = {
    { 0ULL, "set brightness" },
    { 1ULL, "on duration" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 17ULL, "ramp brightness" },
    { 18ULL, "ramp toggle" },
    { 19ULL, "ramp up" },
    { 20ULL, "ramp down" },
    { 21ULL, "ramp up/down" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
    { 50ULL, "flash momentarily" },
};
static const value_desc_t RVC_DGN_1FEDB_FIELD_5_VALUES[] = {
    { 0ULL, "no interlock active" },
    { 1ULL, "interlock a" },
    { 10ULL, "interlock b" },
};
static const field_desc_t RVC_DGN_1FEDB_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "desired level", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "command", 3, 3, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDB_FIELD_3_VALUES, 16, 1, 0, false },
    { "delay/duration", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "interlock", 5, 5, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDB_FIELD_5_VALUES, 3, 1, 0, false },
    { "ramp time", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDC_FIELD_1_VALUES[] = {
    { 0ULL, "unlocked" },
    { 1ULL, "locked" },
    { 11ULL, "lock command not supported" },
};
static const value_desc_t RVC_DGN_1FEDC_FIELD_2_VALUES[] = {
    { 0ULL, "not in overcurrent" },
    { 1ULL, "in overcurrent" },
    { 11ULL, "status not available" },
};
static const value_desc_t RVC_DGN_1FEDC_FIELD_3_VALUES[] = {
    { 0ULL, "override inactive" },
    { 1ULL, "override active" },
    { 11ULL, "override status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDC_FIELD_4_VALUES[] = {
    { 0ULL, "enabled" },
    { 1ULL, "disabled" },
    { 11ULL, "enable status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDC_FIELD_5_VALUES[] = {
    { 0ULL, "set level" },
    { 1ULL, "True" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
};
static const value_desc_t RVC_DGN_1FEDC_FIELD_6_VALUES[] = {
    { 0ULL, "interlock not active" },
    { 1ULL, "interlock active" },
    { 2ULL, "interlock not supported" },
};
static const field_desc_t RVC_DGN_1FEDC_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "lock status", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDC_FIELD_1_VALUES, 3, 1, 0, false },
    { "overcurrent status", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDC_FIELD_2_VALUES, 3, 1, 0, false },
    { "override status", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FEDC_FIELD_3_VALUES, 3, 1, 0, false },
    { "enable status", 1, 1, 6, 7, FIELD_BITS, "", RVC_DGN_1FEDC_FIELD_4_VALUES, 3, 1, 0, false },
    { "last command", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDC_FIELD_5_VALUES, 10, 1, 0, false },
    { "interlock status", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDC_FIELD_6_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDD_FIELD_1_VALUES[] = {
    { 0ULL, "unlocked" },
    { 1ULL, "locked" },
    { 11ULL, "lock command not supported" },
};
static const value_desc_t RVC_DGN_1FEDD_FIELD_2_VALUES[] = {
    { 0ULL, "not in overcurrent" },
    { 1ULL, "in overcurrent" },
    { 11ULL, "status not available" },
};
static const value_desc_t RVC_DGN_1FEDD_FIELD_3_VALUES[] = {
    { 0ULL, "override inactive" },
    { 1ULL, "override active" },
    { 11ULL, "override status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDD_FIELD_4_VALUES[] = {
    { 0ULL, "enabled" },
    { 1ULL, "disabled" },
    { 11ULL, "enable status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDD_FIELD_5_VALUES[] = {
    { 0ULL, "set level" },
    { 1ULL, "True" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
};
static const value_desc_t RVC_DGN_1FEDD_FIELD_6_VALUES[] = {
    { 0ULL, "interlock not active" },
    { 1ULL, "interlock active" },
    { 2ULL, "interlock not supported" },
};
static const field_desc_t RVC_DGN_1FEDD_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "lock status", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDD_FIELD_1_VALUES, 3, 1, 0, false },
    { "overcurrent status", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDD_FIELD_2_VALUES, 3, 1, 0, false },
    { "override status", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FEDD_FIELD_3_VALUES, 3, 1, 0, false },
    { "enable status", 1, 1, 6, 7, FIELD_BITS, "", RVC_DGN_1FEDD_FIELD_4_VALUES, 3, 1, 0, false },
    { "last command", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDD_FIELD_5_VALUES, 10, 1, 0, false },
    { "interlock status", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDD_FIELD_6_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDE_FIELD_3_VALUES[] = {
    { 0ULL, "unlocked" },
    { 1ULL, "locked" },
    { 11ULL, "lock command not supported" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_4_VALUES[] = {
    { 0ULL, "inactive" },
    { 1ULL, "active" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_5_VALUES[] = {
    { 0ULL, "inactive" },
    { 1ULL, "active" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_6_VALUES[] = {
    { 0ULL, "inactive" },
    { 1ULL, "active" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_8_VALUES[] = {
    { 4ULL, "stop" },
    { 129ULL, "forward" },
    { 65ULL, "reverse" },
    { 133ULL, "toggle forward" },
    { 69ULL, "toggle reverse" },
    { 16ULL, "tilt" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_9_VALUES[] = {
    { 0ULL, "not in overcurrent" },
    { 1ULL, "has drawn overcurrent" },
    { 11ULL, "overcurrent status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_10_VALUES[] = {
    { 0ULL, "external override inactive" },
    { 1ULL, "external override active" },
    { 11ULL, "override status unavailable" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_11_VALUES[] = {
    { 0ULL, "inactive" },
    { 1ULL, "active" },
    { 11ULL, "not supported" },
};
static const value_desc_t RVC_DGN_1FEDE_FIELD_12_VALUES[] = {
    { 0ULL, "inactive" },
    { 1ULL, "active" },
    { 11ULL, "not supported" },
};
static const field_desc_t RVC_DGN_1FEDE_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "motor duty", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "lock status", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_3_VALUES, 3, 1, 0, false },
    { "motor status", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_4_VALUES, 2, 1, 0, false },
    { "forward status", 3, 3, 4, 5, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_5_VALUES, 2, 1, 0, false },
    { "reverse status", 3, 3, 6, 7, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_6_VALUES, 2, 1, 0, false },
    { "duration", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "last command", 5, 5, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDE_FIELD_8_VALUES, 8, 1, 0, false },
    { "overcurrent status", 6, 6, 0, 1, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_9_VALUES, 3, 1, 0, false },
    { "override status", 6, 6, 2, 3, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_10_VALUES, 3, 1, 0, false },
    { "disable1 status", 6, 6, 4, 5, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_11_VALUES, 3, 1, 0, false },
    { "disable2 status", 6, 6, 6, 7, FIELD_BITS, "", RVC_DGN_1FEDE_FIELD_12_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FEDF_FIELD_3_VALUES[] = {
    { 4ULL, "stop" },
    { 129ULL, "forward" },
    { 65ULL, "reverse" },
    { 133ULL, "toggle forward" },
    { 69ULL, "toggle reverse" },
    { 16ULL, "tilt" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
};
static const field_desc_t RVC_DGN_1FEDF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "motor duty", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "command", 3, 3, 0, 7, FIELD_UINT, "", RVC_DGN_1FEDF_FIELD_3_VALUES, 8, 1, 0, false },
    { "duration", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "interlock", 5, 5, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEE0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEE1_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEE2_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEE3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEE4_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FEE5_FIELD_1_VALUES[] = {
    { 0ULL, "unlocked" },
    { 1ULL, "locked" },
};
static const field_desc_t RVC_DGN_1FEE5_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "lock status", 1, 1, 0, 1, FIELD_UINT, "", RVC_DGN_1FEE5_FIELD_1_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEE8_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "dc amperage", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEE9_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEEA_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEEB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEEC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEED_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEEE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEEF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF1_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF2_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEF6_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FEF7_FIELD_1_VALUES[] = {
    { 0ULL, "sleep" },
    { 1ULL, "wake" },
    { 2ULL, "away" },
    { 3ULL, "return" },
    { 0ULL, "additional instances" },
    { 250ULL, "storage" },
};
static const value_desc_t RVC_DGN_1FEF7_FIELD_2_VALUES[] = {
    { 0ULL, "12:00 am" },
    { 1ULL, "1:00 am" },
    { 2ULL, "2:00 am" },
    { 3ULL, "3:00 am" },
    { 4ULL, "4:00 am" },
    { 5ULL, "5:00 am" },
    { 6ULL, "6:00 am" },
    { 7ULL, "7:00 am" },
    { 8ULL, "8:00 am" },
    { 9ULL, "9:00 am" },
    { 10ULL, "10:00 am" },
    { 11ULL, "11:00 am" },
    { 12ULL, "12:00 pm" },
    { 13ULL, "1:00 pm" },
    { 14ULL, "2:00 pm" },
    { 15ULL, "3:00 pm" },
    { 16ULL, "4:00 pm" },
    { 17ULL, "5:00 pm" },
    { 18ULL, "6:00 pm" },
    { 19ULL, "7:00 pm" },
    { 20ULL, "8:00 pm" },
    { 21ULL, "9:00 pm" },
    { 22ULL, "10:00 pm" },
    { 23ULL, "11:00 pm" },
};
static const field_desc_t RVC_DGN_1FEF7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "schedule mode instance", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FEF7_FIELD_1_VALUES, 6, 1, 0, false },
    { "start hour", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FEF7_FIELD_2_VALUES, 24, 1, 0, false },
    { "start minute", 3, 3, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "setpoint temp heat", 4, 5, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "setpoint temp cool", 6, 7, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEF8_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "current schedule instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEF9_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FEFA_FIELD_3_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const field_desc_t RVC_DGN_1FEFA_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "current schedule instance", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "number of schedule instances", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "reduced noise mode", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FEFA_FIELD_3_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FEFB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEFC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEFD_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEFE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FEFF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF80_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF81_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF82_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF83_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF84_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FF85_FIELD_1_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const value_desc_t RVC_DGN_1FF85_FIELD_2_VALUES[] = {
    { 0ULL, "primary" },
    { 1ULL, "secondary" },
    { 111ULL, "invalid" },
};
static const value_desc_t RVC_DGN_1FF85_FIELD_3_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const field_desc_t RVC_DGN_1FF85_FIELDS[] = {
    { "instance", 0, 0, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "input output type", 0, 0, 3, 3, FIELD_BITS, "", RVC_DGN_1FF85_FIELD_1_VALUES, 2, 1, 0, false },
    { "source", 0, 0, 4, 6, FIELD_BITS, "", RVC_DGN_1FF85_FIELD_2_VALUES, 3, 1, 0, false },
    { "leg", 0, 0, 7, 7, FIELD_BITS, "", RVC_DGN_1FF85_FIELD_3_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF86_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF87_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF88_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF89_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FF8A_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FF8A_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FF8A_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FF8A_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FF8A_FIELD_2_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF8B_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF8C_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF8D_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF8E_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FF8F_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FF8F_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FF8F_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FF8F_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FF8F_FIELD_2_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF94_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF95_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF96_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "maximum charge current as percent", 1, 1, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "charge rate limit as percent of bank size", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "shore breaker size", 3, 3, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "default battery temperature", 4, 4, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "recharge voltage", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF97_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF98_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "equalization voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "equalization time", 3, 4, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FF99_FIELD_2_VALUES[] = {
    { 0ULL, "pre-charging not in process" },
    { 1ULL, "charging batteries to prepare for equalization" },
};
static const field_desc_t RVC_DGN_1FF99_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "time remaining", 1, 2, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "pre-charging status", 3, 3, 0, 1, FIELD_UINT, "", RVC_DGN_1FF99_FIELD_2_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF9A_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF9B_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FF9C_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "ambient temp", 1, 2, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FF9F_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA1_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA2_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA3_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFA9_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFAA_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFAB_FIELD_1_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const value_desc_t RVC_DGN_1FFAB_FIELD_2_VALUES[] = {
    { 0ULL, "primary" },
    { 1ULL, "secondary" },
    { 111ULL, "invalid" },
};
static const value_desc_t RVC_DGN_1FFAB_FIELD_3_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const field_desc_t RVC_DGN_1FFAB_FIELDS[] = {
    { "instance", 0, 0, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "input output type", 0, 0, 3, 3, FIELD_BITS, "", RVC_DGN_1FFAB_FIELD_1_VALUES, 2, 1, 0, false },
    { "source", 0, 0, 4, 6, FIELD_BITS, "", RVC_DGN_1FFAB_FIELD_2_VALUES, 3, 1, 0, false },
    { "leg", 0, 0, 7, 7, FIELD_BITS, "", RVC_DGN_1FFAB_FIELD_3_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFAC_FIELD_1_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const value_desc_t RVC_DGN_1FFAC_FIELD_2_VALUES[] = {
    { 0ULL, "primary" },
    { 1ULL, "secondary" },
    { 111ULL, "invalid" },
};
static const value_desc_t RVC_DGN_1FFAC_FIELD_3_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const field_desc_t RVC_DGN_1FFAC_FIELDS[] = {
    { "instance", 0, 0, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "input output type", 0, 0, 3, 3, FIELD_BITS, "", RVC_DGN_1FFAC_FIELD_1_VALUES, 2, 1, 0, false },
    { "source", 0, 0, 4, 6, FIELD_BITS, "", RVC_DGN_1FFAC_FIELD_2_VALUES, 3, 1, 0, false },
    { "leg", 0, 0, 7, 7, FIELD_BITS, "", RVC_DGN_1FFAC_FIELD_3_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFAD_FIELD_1_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const value_desc_t RVC_DGN_1FFAD_FIELD_2_VALUES[] = {
    { 0ULL, "primary" },
    { 1ULL, "secondary" },
    { 111ULL, "invalid" },
};
static const value_desc_t RVC_DGN_1FFAD_FIELD_3_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const field_desc_t RVC_DGN_1FFAD_FIELDS[] = {
    { "instance", 0, 0, 0, 2, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "input output type", 0, 0, 3, 3, FIELD_BITS, "", RVC_DGN_1FFAD_FIELD_1_VALUES, 2, 1, 0, false },
    { "source", 0, 0, 4, 6, FIELD_BITS, "", RVC_DGN_1FFAD_FIELD_2_VALUES, 3, 1, 0, false },
    { "leg", 0, 0, 7, 7, FIELD_BITS, "", RVC_DGN_1FFAD_FIELD_3_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFAE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFAF_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFB0_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFB1_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFB2_FIELD_0_VALUES[] = {
    { 0ULL, "disable pump" },
    { 1ULL, "enable pump" },
};
static const field_desc_t RVC_DGN_1FFB2_FIELDS[] = {
    { "command", 0, 0, 0, 1, FIELD_BITS, "", RVC_DGN_1FFB2_FIELD_0_VALUES, 2, 1, 0, false },
    { "pump pressure setting", 1, 2, 0, 7, FIELD_UINT, "Pa", NULL, 0, 1, 0, false },
    { "regulator pressure setting", 3, 4, 0, 7, FIELD_UINT, "Pa", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFB3_FIELD_0_VALUES[] = {
    { 0ULL, "pump disabled" },
    { 1ULL, "pump enabled" },
};
static const value_desc_t RVC_DGN_1FFB3_FIELD_1_VALUES[] = {
    { 0ULL, "pump not running" },
    { 1ULL, "pump running" },
};
static const value_desc_t RVC_DGN_1FFB3_FIELD_2_VALUES[] = {
    { 0ULL, "outside water connected" },
    { 1ULL, "no outside water" },
};
static const field_desc_t RVC_DGN_1FFB3_FIELDS[] = {
    { "operating status", 0, 0, 0, 1, FIELD_BITS, "", RVC_DGN_1FFB3_FIELD_0_VALUES, 2, 1, 0, false },
    { "pump status", 0, 0, 2, 3, FIELD_BITS, "", RVC_DGN_1FFB3_FIELD_1_VALUES, 2, 1, 0, false },
    { "water hookup detected", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFB3_FIELD_2_VALUES, 2, 1, 0, false },
    { "current system pressure", 1, 2, 0, 7, FIELD_UINT, "Pa", NULL, 0, 1, 0, false },
    { "pump pressure setting", 3, 4, 0, 7, FIELD_UINT, "Pa", NULL, 0, 1, 0, false },
    { "regulator pressure setting", 5, 6, 0, 7, FIELD_UINT, "Pa", NULL, 0, 1, 0, false },
    { "operating current", 7, 7, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFB4_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFB5_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFB6_FIELD_0_VALUES[] = {
    { 0ULL, "fresh water" },
    { 1ULL, "black waste" },
    { 2ULL, "gray waste" },
    { 3ULL, "lpg" },
    { 16ULL, "second fresh water" },
    { 17ULL, "second black waste" },
    { 18ULL, "second gray waste" },
    { 19ULL, "second lpg" },
};
static const field_desc_t RVC_DGN_1FFB6_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFB6_FIELD_0_VALUES, 8, 1, 0, false },
    { "relative level", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "resolution", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "absolute level", 3, 4, 0, 7, FIELD_UINT, "Liter", NULL, 0, 1, 0, false },
    { "tank size", 5, 6, 0, 7, FIELD_UINT, "Liter", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFB7_FIELD_0_VALUES[] = {
    { 0ULL, "fresh water" },
    { 1ULL, "black waste" },
    { 2ULL, "gray waste" },
    { 3ULL, "lpg" },
    { 16ULL, "second fresh water" },
    { 17ULL, "second black waste" },
    { 18ULL, "second gray waste" },
    { 19ULL, "second lpg" },
};
static const field_desc_t RVC_DGN_1FFB7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFB7_FIELD_0_VALUES, 8, 1, 0, false },
    { "relative level", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "resolution", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "absolute level", 3, 4, 0, 7, FIELD_UINT, "Liter", NULL, 0, 1, 0, false },
    { "tank size", 5, 6, 0, 7, FIELD_UINT, "Liter", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFB8_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFB9_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFBA_FIELD_5_VALUES[] = {
    { 0ULL, "no fault" },
    { 1ULL, "burnt bulb" },
};
static const value_desc_t RVC_DGN_1FFBA_FIELD_6_VALUES[] = {
    { 0ULL, "no fault" },
    { 1ULL, "burnt bulb" },
};
static const value_desc_t RVC_DGN_1FFBA_FIELD_7_VALUES[] = {
    { 0ULL, "no fault" },
    { 1ULL, "Burnt bulb" },
};
static const value_desc_t RVC_DGN_1FFBA_FIELD_8_VALUES[] = {
    { 0ULL, "no fault" },
    { 1ULL, "Burnt bulb" },
};
static const field_desc_t RVC_DGN_1FFBA_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "master current", 1, 1, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "red current", 2, 2, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "green current", 3, 3, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "blue current", 4, 4, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "master fault", 5, 5, 0, 1, FIELD_BITS, "", RVC_DGN_1FFBA_FIELD_5_VALUES, 2, 1, 0, false },
    { "red fault", 5, 5, 2, 3, FIELD_BITS, "", RVC_DGN_1FFBA_FIELD_6_VALUES, 2, 1, 0, false },
    { "green fault", 5, 5, 4, 5, FIELD_BITS, "", RVC_DGN_1FFBA_FIELD_7_VALUES, 2, 1, 0, false },
    { "blue fault", 5, 5, 6, 7, FIELD_BITS, "", RVC_DGN_1FFBA_FIELD_8_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFBB_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "master brightness", 1, 1, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "red brightness", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "green brightness", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "blue brightness", 4, 4, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "on duration", 5, 5, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "off duration", 5, 5, 4, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFBC_FIELD_3_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FFBC_FIELD_4_VALUES[] = {
    { 0ULL, "no interlock active" },
    { 1ULL, "interlock a" },
    { 10ULL, "interlock b" },
};
static const value_desc_t RVC_DGN_1FFBC_FIELD_5_VALUES[] = {
    { 0ULL, "set level" },
    { 1ULL, "on duration" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
};
static const field_desc_t RVC_DGN_1FFBC_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "desired level", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "desired operating mode", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FFBC_FIELD_3_VALUES, 2, 1, 0, false },
    { "interlock", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FFBC_FIELD_4_VALUES, 3, 1, 0, false },
    { "command", 4, 4, 0, 7, FIELD_UINT, "", RVC_DGN_1FFBC_FIELD_5_VALUES, 10, 1, 0, false },
    { "delay/duration", 5, 5, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFBD_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFBE_FIELD_3_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FFBE_FIELD_4_VALUES[] = {
    { 0ULL, "no interlock active" },
    { 1ULL, "interlock a" },
    { 10ULL, "interlock b" },
};
static const value_desc_t RVC_DGN_1FFBE_FIELD_5_VALUES[] = {
    { 0ULL, "highest priority" },
    { 1101ULL, "lowest priority" },
    { 1110ULL, "error" },
    { 1111ULL, "no data" },
};
static const value_desc_t RVC_DGN_1FFBE_FIELD_6_VALUES[] = {
    { 0ULL, "set level" },
    { 1ULL, "on duration" },
    { 2ULL, "on delay" },
    { 3ULL, "False" },
    { 4ULL, "stop" },
    { 5ULL, "toggle" },
    { 6ULL, "memory off" },
    { 17ULL, "ramp brightness" },
    { 18ULL, "ramp toggle" },
    { 19ULL, "ramp up" },
    { 20ULL, "ramp down" },
    { 21ULL, "ramp up/down" },
    { 22ULL, "increment one level" },
    { 23ULL, "decrement one level" },
    { 33ULL, "lock" },
    { 34ULL, "unlock" },
    { 49ULL, "flash" },
    { 50ULL, "flash momentarily" },
};
static const field_desc_t RVC_DGN_1FFBE_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "desired level", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "desired operating mode", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FFBE_FIELD_3_VALUES, 2, 1, 0, false },
    { "interlock", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FFBE_FIELD_4_VALUES, 3, 1, 0, false },
    { "priority", 3, 3, 4, 7, FIELD_BITS, "", RVC_DGN_1FFBE_FIELD_5_VALUES, 4, 1, 0, false },
    { "command", 4, 4, 0, 7, FIELD_UINT, "", RVC_DGN_1FFBE_FIELD_6_VALUES, 18, 1, 0, false },
    { "delay/duration", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFBF_FIELD_3_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FFBF_FIELD_4_VALUES[] = {
    { 0ULL, "not variable" },
    { 1ULL, "variable" },
};
static const value_desc_t RVC_DGN_1FFBF_FIELD_5_VALUES[] = {
    { 0ULL, "highest priority" },
    { 1101ULL, "lowest priority" },
    { 1110ULL, "error" },
    { 1111ULL, "no data" },
};
static const field_desc_t RVC_DGN_1FFBF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "group", 1, 1, 0, 7, FIELD_UINT, "bitmap", NULL, 0, 1, 0, false },
    { "operating status", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "operating mode", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FFBF_FIELD_3_VALUES, 2, 1, 0, false },
    { "variable level capability", 3, 3, 2, 3, FIELD_BITS, "", RVC_DGN_1FFBF_FIELD_4_VALUES, 2, 1, 0, false },
    { "priority", 3, 3, 4, 7, FIELD_BITS, "", RVC_DGN_1FFBF_FIELD_5_VALUES, 4, 1, 0, false },
    { "delay", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "demanded current", 5, 5, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
    { "present current", 6, 7, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC4_FIELD_1_VALUES[] = {
    { 0ULL, "constant voltage" },
    { 1ULL, "constant current" },
    { 2ULL, "3-stage" },
    { 3ULL, "2-stage" },
    { 4ULL, "trickle" },
    { 249ULL, "custom 2" },
    { 250ULL, "custom 1" },
};
static const value_desc_t RVC_DGN_1FFC4_FIELD_2_VALUES[] = {
    { 0ULL, "stand-alone" },
    { 1ULL, "primary" },
    { 2ULL, "secondary" },
    { 3ULL, "linked to dc source" },
};
static const value_desc_t RVC_DGN_1FFC4_FIELD_3_VALUES[] = {
    { 0ULL, "no sensor in use" },
    { 1ULL, "sensor present and active" },
};
static const value_desc_t RVC_DGN_1FFC4_FIELD_4_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFC4_FIELD_6_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium iron phosphate" },
};
static const field_desc_t RVC_DGN_1FFC4_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charging algorithm", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC4_FIELD_1_VALUES, 7, 1, 0, false },
    { "charger mode", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC4_FIELD_2_VALUES, 4, 1, 0, false },
    { "battery sensor present", 3, 3, 0, 1, FIELD_UINT, "", RVC_DGN_1FFC4_FIELD_3_VALUES, 2, 1, 0, false },
    { "charger installation line", 3, 3, 2, 3, FIELD_UINT, "", RVC_DGN_1FFC4_FIELD_4_VALUES, 2, 1, 0, false },
    { "battery bank size", 4, 5, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "battery type", 6, 6, 0, 3, FIELD_UINT, "", RVC_DGN_1FFC4_FIELD_6_VALUES, 4, 1, 0, false },
    { "maximum charging current", 7, 7, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC5_FIELD_1_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable charger" },
    { 2ULL, "start equalization" },
};
static const value_desc_t RVC_DGN_1FFC5_FIELD_2_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFC5_FIELD_3_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFC5_FIELD_4_VALUES[] = {
    { 0ULL, "cancel forcing" },
    { 1ULL, "force charge to bulk" },
    { 2ULL, "force charge to float" },
};
static const field_desc_t RVC_DGN_1FFC5_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "status", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC5_FIELD_1_VALUES, 3, 1, 0, false },
    { "default state on power-up", 2, 2, 0, 1, FIELD_UINT, "", RVC_DGN_1FFC5_FIELD_2_VALUES, 2, 1, 0, false },
    { "auto recharge enable", 2, 2, 2, 3, FIELD_UINT, "", RVC_DGN_1FFC5_FIELD_3_VALUES, 2, 1, 0, false },
    { "force charge", 2, 2, 4, 7, FIELD_UINT, "", RVC_DGN_1FFC5_FIELD_4_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC6_FIELD_1_VALUES[] = {
    { 0ULL, "constant voltage" },
    { 1ULL, "constant current" },
    { 2ULL, "3-stage" },
    { 3ULL, "2-stage" },
    { 4ULL, "trickle" },
    { 249ULL, "custom 2" },
    { 250ULL, "custom 1" },
};
static const value_desc_t RVC_DGN_1FFC6_FIELD_2_VALUES[] = {
    { 0ULL, "stand-alone" },
    { 1ULL, "primary" },
    { 2ULL, "secondary" },
    { 3ULL, "linked to dc source" },
};
static const value_desc_t RVC_DGN_1FFC6_FIELD_3_VALUES[] = {
    { 0ULL, "no sensor in use" },
    { 1ULL, "sensor present and active" },
};
static const value_desc_t RVC_DGN_1FFC6_FIELD_4_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFC6_FIELD_5_VALUES[] = {
    { 0ULL, "flooded" },
    { 1ULL, "gel" },
    { 2ULL, "agm" },
    { 3ULL, "lithium iron phosphate" },
};
static const field_desc_t RVC_DGN_1FFC6_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charging algorithm", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC6_FIELD_1_VALUES, 7, 1, 0, false },
    { "charger mode", 2, 2, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC6_FIELD_2_VALUES, 4, 1, 0, false },
    { "battery sensor present", 3, 3, 0, 1, FIELD_UINT, "", RVC_DGN_1FFC6_FIELD_3_VALUES, 2, 1, 0, false },
    { "charger installation line", 3, 3, 2, 3, FIELD_UINT, "", RVC_DGN_1FFC6_FIELD_4_VALUES, 2, 1, 0, false },
    { "battery type", 3, 3, 4, 7, FIELD_UINT, "", RVC_DGN_1FFC6_FIELD_5_VALUES, 4, 1, 0, false },
    { "battery bank size", 4, 5, 0, 7, FIELD_UINT, "ah", NULL, 0, 1, 0, false },
    { "maximum charging current", 6, 7, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC7_FIELD_4_VALUES[] = {
    { 0ULL, "undefined" },
    { 1ULL, "do not charge" },
    { 2ULL, "bulk" },
    { 3ULL, "absorption" },
    { 4ULL, "overcharge" },
    { 5ULL, "equalize" },
    { 6ULL, "float" },
    { 7ULL, "constant voltage/current" },
};
static const value_desc_t RVC_DGN_1FFC7_FIELD_5_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFC7_FIELD_6_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFC7_FIELD_7_VALUES[] = {
    { 0ULL, "charging not forced" },
    { 1ULL, "force charge to bulk" },
    { 2ULL, "force charge to float" },
};
static const field_desc_t RVC_DGN_1FFC7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "charge voltage", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "charge current", 3, 4, 0, 7, FIELD_UINT, "a", NULL, 0, 1, 0, false },
    { "charge current percent of maximum", 5, 5, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "operating state", 6, 6, 0, 7, FIELD_UINT, "", RVC_DGN_1FFC7_FIELD_4_VALUES, 8, 1, 0, false },
    { "default state on power-up", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FFC7_FIELD_5_VALUES, 2, 1, 0, false },
    { "auto recharge enable", 7, 7, 2, 3, FIELD_BITS, "", RVC_DGN_1FFC7_FIELD_6_VALUES, 2, 1, 0, false },
    { "force charge", 7, 7, 4, 7, FIELD_UINT, "", RVC_DGN_1FFC7_FIELD_7_VALUES, 3, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC8_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFC8_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFC8_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFC8_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFC8_FIELD_2_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFC9_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFC9_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFC9_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFC9_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFC9_FIELD_2_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFCA_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFCA_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFCA_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFCA_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFCA_FIELD_2_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFCB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFCC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFCD_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFCE_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFCF_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage maximum", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "dc source warning voltage minimum", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage maximum", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFD0_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "load sense power threshold", 1, 2, 0, 7, FIELD_UINT, "w", NULL, 0, 1, 0, false },
    { "load sense interval", 3, 4, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage minimum", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFD1_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage maximum", 1, 2, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "dc source warning voltage minimum", 3, 4, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage maximum", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD2_FIELD_4_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFD2_FIELD_5_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const value_desc_t RVC_DGN_1FFD2_FIELD_6_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const field_desc_t RVC_DGN_1FFD2_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "load sense power threshold", 1, 2, 0, 7, FIELD_UINT, "w", NULL, 0, 1, 0, false },
    { "load sense interval", 3, 4, 0, 7, FIELD_UINT, "sec", NULL, 0, 1, 0, false },
    { "dc source shutdown voltage minimum", 5, 6, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
    { "inverter enable on startup", 7, 7, 0, 1, FIELD_UINT, "", RVC_DGN_1FFD2_FIELD_4_VALUES, 2, 1, 0, false },
    { "load sense enable on startup", 7, 7, 2, 3, FIELD_UINT, "", RVC_DGN_1FFD2_FIELD_5_VALUES, 2, 1, 0, false },
    { "ac pass-through enable on startup", 7, 7, 4, 5, FIELD_UINT, "", RVC_DGN_1FFD2_FIELD_6_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD3_FIELD_1_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const value_desc_t RVC_DGN_1FFD3_FIELD_2_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const value_desc_t RVC_DGN_1FFD3_FIELD_3_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const value_desc_t RVC_DGN_1FFD3_FIELD_4_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const value_desc_t RVC_DGN_1FFD3_FIELD_5_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const value_desc_t RVC_DGN_1FFD3_FIELD_6_VALUES[] = {
    { 0ULL, "disable" },
    { 1ULL, "enable" },
};
static const field_desc_t RVC_DGN_1FFD3_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "inverter enable", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_1_VALUES, 2, 1, 0, false },
    { "load sense enable", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_2_VALUES, 2, 1, 0, false },
    { "pass-through enable", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_3_VALUES, 2, 1, 0, false },
    { "inverter enable on startup", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_4_VALUES, 2, 1, 0, false },
    { "load sense enable on startup", 7, 7, 2, 3, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_5_VALUES, 2, 1, 0, false },
    { "pass-through enable on startup", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FFD3_FIELD_6_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD4_FIELD_1_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "invert" },
    { 2ULL, "ac passthru" },
    { 3ULL, "aps only" },
    { 4ULL, "load sense" },
    { 5ULL, "waiting to invert" },
    { 6ULL, "generator support" },
};
static const value_desc_t RVC_DGN_1FFD4_FIELD_2_VALUES[] = {
    { 0ULL, "no sensor in use" },
    { 1ULL, "sensor present and active" },
};
static const value_desc_t RVC_DGN_1FFD4_FIELD_3_VALUES[] = {
    { 0ULL, "load sense disabled" },
    { 1ULL, "load sense enabled" },
};
static const value_desc_t RVC_DGN_1FFD4_FIELD_4_VALUES[] = {
    { 0ULL, "inverter disabled" },
    { 1ULL, "inverter enabled" },
};
static const value_desc_t RVC_DGN_1FFD4_FIELD_5_VALUES[] = {
    { 0ULL, "pass-through disabled" },
    { 1ULL, "pass-through enabled" },
};
static const value_desc_t RVC_DGN_1FFD4_FIELD_6_VALUES[] = {
    { 0ULL, "generator support disabled" },
    { 1ULL, "generator support enabled" },
};
static const field_desc_t RVC_DGN_1FFD4_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "status", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFD4_FIELD_1_VALUES, 7, 1, 0, false },
    { "battery temperature sensor present", 2, 2, 0, 1, FIELD_BITS, "", RVC_DGN_1FFD4_FIELD_2_VALUES, 2, 1, 0, false },
    { "load sense enabled", 2, 2, 2, 3, FIELD_BITS, "", RVC_DGN_1FFD4_FIELD_3_VALUES, 2, 1, 0, false },
    { "inverter enabled", 2, 2, 4, 5, FIELD_BITS, "", RVC_DGN_1FFD4_FIELD_4_VALUES, 2, 1, 0, false },
    { "pass-through enabled", 2, 2, 6, 7, FIELD_BITS, "", RVC_DGN_1FFD4_FIELD_5_VALUES, 2, 1, 0, false },
    { "generator support enabled", 3, 3, 0, 1, FIELD_BITS, "", RVC_DGN_1FFD4_FIELD_6_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD5_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFD5_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFD5_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFD5_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFD5_FIELD_2_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD6_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFD6_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFD6_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFD6_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFD6_FIELD_2_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFD7_FIELD_1_VALUES[] = {
    { 0ULL, "1" },
    { 1ULL, "2" },
};
static const value_desc_t RVC_DGN_1FFD7_FIELD_2_VALUES[] = {
    { 0ULL, "input" },
    { 1ULL, "output" },
};
static const field_desc_t RVC_DGN_1FFD7_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 5, FIELD_BITS, "", RVC_DGN_1FFD7_FIELD_1_VALUES, 2, 1, 0, false },
    { "input/output", 0, 0, 6, 7, FIELD_BITS, "", RVC_DGN_1FFD7_FIELD_2_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFD8_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFD9_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFDA_FIELD_0_VALUES[] = {
    { 0ULL, "stop" },
    { 1ULL, "start" },
    { 2ULL, "manual prime" },
    { 3ULL, "manual preheat" },
};
static const field_desc_t RVC_DGN_1FFDA_FIELDS[] = {
    { "command", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFDA_FIELD_0_VALUES, 4, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFDB_FIELD_0_VALUES[] = {
    { 0ULL, "temperature shutdown not active" },
    { 1ULL, "temperature shutdown active" },
};
static const value_desc_t RVC_DGN_1FFDB_FIELD_1_VALUES[] = {
    { 0ULL, "oil pressure shutdown not active" },
    { 1ULL, "oil pressure shutdown active" },
};
static const value_desc_t RVC_DGN_1FFDB_FIELD_2_VALUES[] = {
    { 0ULL, "oil level switch not active" },
    { 1ULL, "low oil level detected" },
};
static const value_desc_t RVC_DGN_1FFDB_FIELD_3_VALUES[] = {
    { 0ULL, "caution light off" },
    { 1ULL, "caution light on" },
};
static const field_desc_t RVC_DGN_1FFDB_FIELDS[] = {
    { "temperature shutdown switch", 0, 0, 0, 1, FIELD_UINT, "", RVC_DGN_1FFDB_FIELD_0_VALUES, 2, 1, 0, false },
    { "oil pressure shutdown switch", 0, 0, 2, 3, FIELD_UINT, "", RVC_DGN_1FFDB_FIELD_1_VALUES, 2, 1, 0, false },
    { "oil level shutdown switch", 0, 0, 4, 5, FIELD_UINT, "", RVC_DGN_1FFDB_FIELD_2_VALUES, 2, 1, 0, false },
    { "caution light", 0, 0, 6, 7, FIELD_UINT, "", RVC_DGN_1FFDB_FIELD_3_VALUES, 2, 1, 0, false },
    { "engine coolant temperature", 1, 1, 0, 7, FIELD_UINT, "deg c", NULL, 0, 1, 0, false },
    { "engine oil pressure", 2, 2, 0, 7, FIELD_UINT, "kPa", NULL, 0, 1, 0, false },
    { "engine rpm", 3, 4, 0, 7, FIELD_UINT, "RPM", NULL, 0, 1, 0, false },
    { "fuel rate", 5, 6, 0, 7, FIELD_UINT, "lph", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFDC_FIELD_0_VALUES[] = {
    { 0ULL, "stopped" },
    { 1ULL, "preheat" },
    { 2ULL, "cranking" },
    { 3ULL, "running" },
    { 4ULL, "priming" },
    { 5ULL, "fault" },
    { 6ULL, "engine run only" },
    { 7ULL, "test mode" },
    { 8ULL, "voltage adjust mode" },
    { 9ULL, "fault bypass mode" },
    { 10ULL, "configuration mode" },
};
static const field_desc_t RVC_DGN_1FFDC_FIELDS[] = {
    { "status", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFDC_FIELD_0_VALUES, 11, 1, 0, false },
    { "engine run time", 1, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "engine load", 5, 5, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "start battery voltage", 6, 7, 0, 7, FIELD_UINT, "v", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFDD_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFDE_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFDF_FIELDS[] = {
    { "instance", 0, 0, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
    { "line", 0, 0, 4, 7, FIELD_BITS, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFE0_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFE1_FIELD_1_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const field_desc_t RVC_DGN_1FFE1_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating mode", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFE1_FIELD_1_VALUES, 2, 1, 0, false },
    { "max fan speed", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "max air conditioning output level", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "fan speed", 4, 4, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "air conditioning output level", 5, 5, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "dead band", 6, 6, 0, 7, FIELD_UINT, "uint8", NULL, 0, 1, 0, false },
    { "second stage dead band", 7, 7, 0, 7, FIELD_UINT, "uint8", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFE2_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "cool" },
    { 8ULL, "heat" },
    { 9ULL, "auto" },
    { 64ULL, "fan only" },
    { 65ULL, "aux heat" },
    { 72ULL, "window defrost/dehumidify" },
};
static const value_desc_t RVC_DGN_1FFE2_FIELD_2_VALUES[] = {
    { 0ULL, "auto" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FFE2_FIELD_3_VALUES[] = {
    { 0ULL, "disabled" },
    { 1ULL, "enabled" },
};
static const field_desc_t RVC_DGN_1FFE2_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating mode", 1, 1, 0, 3, FIELD_BITS, "", RVC_DGN_1FFE2_FIELD_1_VALUES, 7, 1, 0, false },
    { "fan mode", 1, 1, 4, 5, FIELD_BITS, "", RVC_DGN_1FFE2_FIELD_2_VALUES, 2, 1, 0, false },
    { "schedule mode", 1, 1, 6, 7, FIELD_BITS, "", RVC_DGN_1FFE2_FIELD_3_VALUES, 2, 1, 0, false },
    { "fan speed", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "setpoint temp heat", 3, 4, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "setpoint temp cool", 5, 6, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFE3_FIELD_1_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FFE3_FIELD_2_VALUES[] = {
    { 0ULL, "combustion" },
    { 1ULL, "AC power primary" },
    { 2ULL, "AC power secondary" },
    { 3ULL, "engine heat" },
    { 4ULL, "hydronic heat - combustion" },
    { 5ULL, "hydronic heat - electric" },
    { 6ULL, "hydronic heat - gas/electric (both)" },
};
static const field_desc_t RVC_DGN_1FFE3_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating mode", 1, 1, 0, 1, FIELD_UINT, "", RVC_DGN_1FFE3_FIELD_1_VALUES, 2, 1, 0, false },
    { "heat source", 1, 1, 2, 7, FIELD_UINT, "", RVC_DGN_1FFE3_FIELD_2_VALUES, 7, 1, 0, false },
    { "circulation fan speed", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "heat output level", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "dead band", 4, 4, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "second stage dead band", 5, 5, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFE4_FIELD_1_VALUES[] = {
    { 0ULL, "automatic" },
    { 1ULL, "manual" },
};
static const value_desc_t RVC_DGN_1FFE4_FIELD_2_VALUES[] = {
    { 0ULL, "combustion" },
    { 1ULL, "AC power primary" },
    { 2ULL, "AC power secondary" },
    { 3ULL, "engine heat" },
    { 4ULL, "hydronic heat - combustion" },
    { 5ULL, "hydronic heat - electric" },
    { 6ULL, "hydronic heat - gas/electric (both)" },
};
static const value_desc_t RVC_DGN_1FFE4_FIELD_7_VALUES[] = {
    { 0ULL, "no overcurrent detected" },
    { 1ULL, "overcurrent detected" },
};
static const value_desc_t RVC_DGN_1FFE4_FIELD_8_VALUES[] = {
    { 0ULL, "no undercurrent detected" },
    { 1ULL, "undercurrent detected" },
};
static const value_desc_t RVC_DGN_1FFE4_FIELD_9_VALUES[] = {
    { 0ULL, "temperature normal" },
    { 1ULL, "temperature warning" },
};
static const value_desc_t RVC_DGN_1FFE4_FIELD_10_VALUES[] = {
    { 0ULL, "off (inactive)" },
    { 1ULL, "on (active)" },
};
static const field_desc_t RVC_DGN_1FFE4_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating mode", 1, 1, 0, 1, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_1_VALUES, 2, 1, 0, false },
    { "heat source", 1, 1, 2, 7, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_2_VALUES, 7, 1, 0, false },
    { "circulation fan speed", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "heat output level", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "dead band", 4, 4, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "second stage dead band", 5, 5, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "zone overcurrent status", 6, 6, 0, 1, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_7_VALUES, 2, 1, 0, false },
    { "zone undercurrent status", 6, 6, 2, 3, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_8_VALUES, 2, 1, 0, false },
    { "zone temperature status", 6, 6, 4, 5, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_9_VALUES, 2, 1, 0, false },
    { "zone analog input status", 6, 6, 6, 7, FIELD_UINT, "", RVC_DGN_1FFE4_FIELD_10_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFE5_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFE6_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFE7_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFE8_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFE9_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFEA_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFEB_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFEC_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFED_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFEE_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFEF_FIELD_0_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_1_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_2_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_3_VALUES[] = {
    { 0ULL, "no transition" },
    { 1ULL, "transition was made" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_6_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_7_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_8_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_9_VALUES[] = {
    { 0ULL, "no transition" },
    { 1ULL, "transition was made" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_14_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_15_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_16_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_17_VALUES[] = {
    { 0ULL, "no transition" },
    { 1ULL, "transition was made" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_20_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_21_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_22_VALUES[] = {
    { 0ULL, "under set point" },
    { 1ULL, "over set point" },
};
static const value_desc_t RVC_DGN_1FFEF_FIELD_23_VALUES[] = {
    { 0ULL, "no transition" },
    { 1ULL, "transition was made" },
};
static const field_desc_t RVC_DGN_1FFEF_FIELDS[] = {
    { "left front sensor status low ride", 0, 0, 0, 1, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_0_VALUES, 2, 1, 0, false },
    { "left front sensor status medium ride", 0, 0, 2, 3, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_1_VALUES, 2, 1, 0, false },
    { "left front sensor status high ride", 0, 0, 4, 5, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_2_VALUES, 2, 1, 0, false },
    { "left front sensor transition", 0, 0, 6, 7, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_3_VALUES, 2, 1, 0, false },
    { "left front quadrature encoder status channel A", 1, 1, 0, 1, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "left front quadrature encoder status channel B", 1, 1, 2, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "right front sensor status low ride", 2, 2, 0, 1, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_6_VALUES, 2, 1, 0, false },
    { "right front sensor status medium ride", 2, 2, 2, 3, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_7_VALUES, 2, 1, 0, false },
    { "right front sensor status high ride", 2, 2, 4, 5, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_8_VALUES, 2, 1, 0, false },
    { "right front sensor transition", 2, 2, 6, 7, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_9_VALUES, 2, 1, 0, false },
    { "right front quadrature encoder status channel A", 3, 3, 0, 1, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "right front quadrature encoder status channel B", 3, 3, 2, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "steering sensor quadrature encoder status channel A", 3, 3, 4, 5, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "steering sensor quadrature encoder status channel B", 3, 3, 6, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "left rear sensor status low ride", 4, 4, 0, 1, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_14_VALUES, 2, 1, 0, false },
    { "left rear sensor status medium ride", 4, 4, 2, 3, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_15_VALUES, 2, 1, 0, false },
    { "left rear sensor status high ride", 4, 4, 4, 5, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_16_VALUES, 2, 1, 0, false },
    { "left rear sensor transition", 4, 4, 6, 7, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_17_VALUES, 2, 1, 0, false },
    { "left rear quadrature encoder status channel A", 5, 5, 0, 1, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "left rear quadrature encoder status channel B", 5, 5, 2, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "right rear sensor status low ride", 6, 6, 0, 1, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_20_VALUES, 2, 1, 0, false },
    { "right rear sensor status medium ride", 6, 6, 2, 3, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_21_VALUES, 2, 1, 0, false },
    { "right rear sensor status high ride", 6, 6, 4, 5, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_22_VALUES, 2, 1, 0, false },
    { "right rear sensor transition", 6, 6, 6, 7, FIELD_UINT, "", RVC_DGN_1FFEF_FIELD_23_VALUES, 2, 1, 0, false },
    { "right rear quadrature encoder status channel A", 7, 7, 0, 1, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "right rear quadrature encoder status channel B", 7, 7, 2, 3, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF0_FIELD_0_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FFF0_FIELD_1_VALUES[] = {
    { 0ULL, "normal" },
    { 1ULL, "fly mode" },
    { 8ULL, "reset" },
    { 1110ULL, "error" },
    { 1111ULL, "na" },
};
static const value_desc_t RVC_DGN_1FFF0_FIELD_6_VALUES[] = {
    { 0ULL, "down" },
    { 1ULL, "up" },
};
static const field_desc_t RVC_DGN_1FFF0_FIELDS[] = {
    { "aas operating status", 0, 0, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF0_FIELD_0_VALUES, 2, 1, 0, false },
    { "aas mode", 0, 0, 2, 5, FIELD_UINT, "", RVC_DGN_1FFF0_FIELD_1_VALUES, 5, 1, 0, false },
    { "left front sensor mode", 1, 1, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "right front sensor mode", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "right rear sensor mode", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "left rear sensor mode", 4, 4, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "tag axle position", 5, 5, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF0_FIELD_6_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFF1_FIELDS[] = {
};

static const field_desc_t RVC_DGN_1FFF2_FIELDS[] = {
    { "left front sensor mode", 0, 0, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "right front sensor mode", 1, 1, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "right rear sensor mode", 2, 2, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "left rear sensor mode", 3, 3, 0, 7, FIELD_UINT, "Pct", NULL, 0, 1, 0, false },
    { "height correction threshold time", 4, 4, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
    { "solenoid on time", 5, 5, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF3_FIELD_0_VALUES[] = {
    { 0ULL, "please release chassis" },
    { 1ULL, "please immobilize chassis" },
    { 10ULL, "error" },
    { 11ULL, "do not care about status" },
};
static const value_desc_t RVC_DGN_1FFF3_FIELD_1_VALUES[] = {
    { 0ULL, "normal priority" },
    { 1ULL, "override" },
    { 10ULL, "error" },
    { 11ULL, "na" },
};
static const value_desc_t RVC_DGN_1FFF3_FIELD_2_VALUES[] = {
    { 0ULL, "please release transmission" },
    { 1ULL, "please lock transmission" },
    { 10ULL, "error" },
    { 11ULL, "do not care about status" },
};
static const value_desc_t RVC_DGN_1FFF3_FIELD_3_VALUES[] = {
    { 0ULL, "normal priority" },
    { 1ULL, "override" },
    { 10ULL, "error" },
    { 11ULL, "na" },
};
static const value_desc_t RVC_DGN_1FFF3_FIELD_4_VALUES[] = {
    { 0ULL, "please release engine" },
    { 1ULL, "please lock engine" },
    { 10ULL, "error" },
    { 11ULL, "do not care about status" },
};
static const value_desc_t RVC_DGN_1FFF3_FIELD_5_VALUES[] = {
    { 0ULL, "normal priority" },
    { 1ULL, "override" },
    { 10ULL, "error" },
    { 11ULL, "na" },
};
static const field_desc_t RVC_DGN_1FFF3_FIELDS[] = {
    { "park brake command", 0, 0, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_0_VALUES, 4, 1, 0, false },
    { "park brake user override", 0, 0, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_1_VALUES, 4, 1, 0, false },
    { "transmission command", 1, 1, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_2_VALUES, 4, 1, 0, false },
    { "transmission lock user override", 1, 1, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_3_VALUES, 4, 1, 0, false },
    { "engine lock command", 2, 2, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_4_VALUES, 4, 1, 0, false },
    { "engine lock user override", 2, 2, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF3_FIELD_5_VALUES, 4, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF4_FIELD_2_VALUES[] = {
    { 0ULL, "park brake released" },
    { 1ULL, "park brake engaged" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_3_VALUES[] = {
    { 0ULL, "transmission not locked" },
    { 1ULL, "transmission locked" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_4_VALUES[] = {
    { 0ULL, "engine free to start" },
    { 1ULL, "engine locked" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_5_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_6_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "True" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_7_VALUES[] = {
    { 124ULL, "reverse 1" },
    { 125ULL, "neutral" },
    { 126ULL, "forward 1" },
    { 127ULL, "forward 2" },
    { 128ULL, "forward 3" },
    { 129ULL, "forward 4" },
    { 130ULL, "forward 5" },
    { 131ULL, "forward 6" },
    { 132ULL, "forward 7" },
    { 251ULL, "park" },
};
static const value_desc_t RVC_DGN_1FFF4_FIELD_8_VALUES[] = {
    { 124ULL, "reverse 1" },
    { 125ULL, "neutral" },
    { 126ULL, "forward 1" },
    { 127ULL, "forward 2" },
    { 128ULL, "forward 3" },
    { 129ULL, "forward 4" },
    { 130ULL, "forward 5" },
    { 131ULL, "forward 6" },
    { 132ULL, "forward 7" },
    { 251ULL, "park" },
};
static const field_desc_t RVC_DGN_1FFF4_FIELDS[] = {
    { "engine rpms", 0, 1, 0, 7, FIELD_UINT, "rpm", NULL, 0, 1, 0, false },
    { "vehicle speed", 2, 3, 0, 7, FIELD_UINT, "kph", NULL, 0, 1, 0, false },
    { "park brake status", 4, 4, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF4_FIELD_2_VALUES, 2, 1, 0, false },
    { "transmission lock status", 4, 4, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF4_FIELD_3_VALUES, 2, 1, 0, false },
    { "engine lock status", 4, 4, 4, 5, FIELD_BITS, "", RVC_DGN_1FFF4_FIELD_4_VALUES, 2, 1, 0, false },
    { "ignition switch status", 5, 5, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF4_FIELD_5_VALUES, 2, 1, 0, false },
    { "accessory switch status", 5, 5, 2, 4, FIELD_BITS, "", RVC_DGN_1FFF4_FIELD_6_VALUES, 2, 1, 0, false },
    { "transmission current gear", 6, 6, 0, 7, FIELD_UINT, "", RVC_DGN_1FFF4_FIELD_7_VALUES, 10, 1, 0, false },
    { "transmission gear selected", 7, 7, 0, 7, FIELD_UINT, "", RVC_DGN_1FFF4_FIELD_8_VALUES, 10, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF5_FIELD_1_VALUES[] = {
    { 1ULL, "co" },
    { 2ULL, "lp gas" },
    { 3ULL, "smoke" },
};
static const value_desc_t RVC_DGN_1FFF5_FIELD_3_VALUES[] = {
    { 0ULL, "no alarm" },
    { 1ULL, "alarm" },
};
static const value_desc_t RVC_DGN_1FFF5_FIELD_4_VALUES[] = {
    { 0ULL, "no warning" },
    { 1ULL, "warning" },
};
static const value_desc_t RVC_DGN_1FFF5_FIELD_5_VALUES[] = {
    { 0ULL, "no failure" },
    { 1ULL, "failure" },
};
static const value_desc_t RVC_DGN_1FFF5_FIELD_6_VALUES[] = {
    { 0ULL, "no failure" },
    { 1ULL, "failure" },
};
static const field_desc_t RVC_DGN_1FFF5_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "type", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFF5_FIELD_1_VALUES, 3, 1, 0, false },
    { "ppm", 2, 3, 0, 7, FIELD_UINT, "ppm", NULL, 0, 1, 0, false },
    { "alarm", 4, 4, 0, 1, FIELD_UINT, "bit", RVC_DGN_1FFF5_FIELD_3_VALUES, 2, 1, 0, false },
    { "warning", 4, 4, 2, 3, FIELD_UINT, "bit", RVC_DGN_1FFF5_FIELD_4_VALUES, 2, 1, 0, false },
    { "dc supply failure", 4, 4, 4, 5, FIELD_UINT, "bit", RVC_DGN_1FFF5_FIELD_5_VALUES, 2, 1, 0, false },
    { "sensor failure", 4, 4, 6, 7, FIELD_UINT, "bit", RVC_DGN_1FFF5_FIELD_6_VALUES, 2, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF6_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "combustion" },
    { 2ULL, "electric" },
    { 3ULL, "gas/electric (both)" },
    { 4ULL, "automatic" },
    { 5ULL, "test combustion" },
    { 6ULL, "test electric" },
};
static const field_desc_t RVC_DGN_1FFF6_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating modes", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFF6_FIELD_1_VALUES, 7, 1, 0, false },
    { "set point temperature", 2, 3, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "electric element level", 6, 6, 0, 3, FIELD_BITS, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFF7_FIELD_1_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "combustion" },
    { 2ULL, "electric" },
    { 3ULL, "gas/electric (both)" },
    { 4ULL, "automatic" },
    { 5ULL, "test combustion" },
    { 6ULL, "test electric" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_4_VALUES[] = {
    { 0ULL, "set point met" },
    { 1ULL, "set point not met" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_5_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "burner lit" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_6_VALUES[] = {
    { 0ULL, "False" },
    { 1ULL, "ac element active" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_7_VALUES[] = {
    { 0ULL, "limit switch not tripped" },
    { 1ULL, "limit switch tripped" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_8_VALUES[] = {
    { 0ULL, "no failure" },
    { 1ULL, "failed to ignite" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_9_VALUES[] = {
    { 0ULL, "ac power present" },
    { 1ULL, "ac power not present" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_10_VALUES[] = {
    { 0ULL, "dc power present" },
    { 1ULL, "dc power not present" },
};
static const value_desc_t RVC_DGN_1FFF7_FIELD_11_VALUES[] = {
    { 0ULL, "dc power sufficient" },
    { 1ULL, "dc power warning" },
};
static const field_desc_t RVC_DGN_1FFF7_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "operating modes", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFF7_FIELD_1_VALUES, 7, 1, 0, false },
    { "set point temperature", 2, 3, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "water temperature", 4, 5, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "thermostat status", 6, 6, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_4_VALUES, 2, 1, 0, false },
    { "burner status", 6, 6, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_5_VALUES, 2, 1, 0, false },
    { "ac element status", 6, 6, 4, 5, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_6_VALUES, 2, 1, 0, false },
    { "high temperature limit switch status", 6, 6, 6, 7, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_7_VALUES, 2, 1, 0, false },
    { "failure to ignite status", 7, 7, 0, 1, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_8_VALUES, 2, 1, 0, false },
    { "ac power failure status", 7, 7, 2, 3, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_9_VALUES, 2, 1, 0, false },
    { "dc power failure status", 7, 7, 4, 5, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_10_VALUES, 2, 1, 0, false },
    { "dc power warning status", 7, 7, 6, 7, FIELD_BITS, "", RVC_DGN_1FFF7_FIELD_11_VALUES, 2, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFF8_FIELDS[] = {
    { "bus-off error count", 0, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "receive frames dropped count", 2, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "transmit frames dropped count", 4, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFF9_FIELDS[] = {
    { "transmitted frames count", 0, 3, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "received frames count", 4, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFFA_FIELDS[] = {
    { "timer count", 0, 3, 0, 7, FIELD_UINT, "ms", NULL, 0, 1, 0, false },
    { "receive error count", 4, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "transmit error count", 6, 7, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFFB_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FFFB_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const field_desc_t RVC_DGN_1FFFB_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFB_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFB_FIELD_1_VALUES, 6, 1, 0, false },
    { "state of health", 2, 2, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "capacity remaining", 3, 4, 0, 7, FIELD_UINT, "Ah", NULL, 0, 1, 0, false },
    { "relative capacity", 5, 5, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "ac rms ripple", 6, 7, 0, 7, FIELD_UINT, "mV", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFFC_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FFFC_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter Charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const field_desc_t RVC_DGN_1FFFC_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFC_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFC_FIELD_1_VALUES, 6, 1, 0, false },
    { "source temperature", 2, 3, 0, 7, FIELD_UINT, "Deg C", NULL, 0, 1, 0, false },
    { "state of charge", 4, 4, 0, 7, FIELD_UINT, "pct", NULL, 0, 1, 0, false },
    { "time remaining", 5, 6, 0, 7, FIELD_UINT, "min", NULL, 0, 1, 0, false },
};

static const value_desc_t RVC_DGN_1FFFD_FIELD_0_VALUES[] = {
    { 0ULL, "invalid" },
    { 1ULL, "main house battery bank" },
    { 2ULL, "chassis start battery" },
    { 3ULL, "secondary house battery bank" },
};
static const value_desc_t RVC_DGN_1FFFD_FIELD_1_VALUES[] = {
    { 120ULL, "battery soc device" },
    { 100ULL, "inverter charger" },
    { 80ULL, "charger" },
    { 60ULL, "inverter" },
    { 40ULL, "voltmeter ammeter" },
    { 20ULL, "voltmeter" },
};
static const field_desc_t RVC_DGN_1FFFD_FIELDS[] = {
    { "instance", 0, 0, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFD_FIELD_0_VALUES, 4, 1, 0, false },
    { "device priority", 1, 1, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFD_FIELD_1_VALUES, 6, 1, 0, false },
    { "dc voltage", 2, 3, 0, 7, FIELD_UINT, "V", NULL, 0, 1, 0, false },
    { "dc current", 4, 7, 0, 7, FIELD_UINT, "A", NULL, 0, 1, 0, false },
};

static const field_desc_t RVC_DGN_1FFFE_FIELDS[] = {
};

static const value_desc_t RVC_DGN_1FFFF_FIELD_3_VALUES[] = {
    { 1ULL, "Sunday" },
    { 2ULL, "Monday" },
    { 3ULL, "Tuesday" },
    { 4ULL, "Wednesday" },
    { 5ULL, "Thursday" },
    { 6ULL, "Friday" },
    { 7ULL, "Saturday" },
};
static const value_desc_t RVC_DGN_1FFFF_FIELD_7_VALUES[] = {
    { 0ULL, "Greenwich Mean Time" },
    { 4ULL, "Eastern Daylight Time" },
    { 5ULL, "Eastern Standard Time" },
    { 7ULL, "Pacific Daylight Time" },
    { 8ULL, "Pacific Standard Time" },
    { 22ULL, "Central European Summer Time" },
};
static const field_desc_t RVC_DGN_1FFFF_FIELDS[] = {
    { "year", 0, 0, 0, 7, FIELD_UINT, "year", NULL, 0, 1, 0, false },
    { "month", 1, 1, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "date", 2, 2, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "day of week", 3, 3, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFF_FIELD_3_VALUES, 7, 1, 0, false },
    { "hour", 4, 4, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "minute", 5, 5, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "second", 6, 6, 0, 7, FIELD_UINT, "", NULL, 0, 1, 0, false },
    { "time zone", 7, 7, 0, 7, FIELD_UINT, "", RVC_DGN_1FFFF_FIELD_7_VALUES, 6, 1, 0, false },
};

static const dgn_desc_t RVC_DGN_DESCS[] = {
    { 0xE8, "ACKNOWLEDGMENT", RVC_DGN_000E8_FIELDS, 5 },
    { 0xEA, "REQUEST_FOR_DGN", RVC_DGN_000EA_FIELDS, 3 },
    { 0x16C, "DC_COMPONENT_DRIVER_STATUS_4", RVC_DGN_0016C_FIELDS, 4 },
    { 0x16F, "DC_COMPONENT_DRIVER_STATUS_1", RVC_DGN_0016F_FIELDS, 9 },
    { 0x17D, "DOWNLOAD", RVC_DGN_0017D_FIELDS, 0 },
    { 0x17E, "TERMINAL", RVC_DGN_0017E_FIELDS, 1 },
    { 0x17F, "GENERAL_RESET", RVC_DGN_0017F_FIELDS, 4 },
    { 0xBFC1, "G12_TANK_LEVEL_SENSOR", RVC_DGN_0BFC1_FIELDS, 2 },
    { 0xEBFF, "DATA_PACKET", RVC_DGN_0EBFF_FIELDS, 2 },
    { 0xECFF, "INITIAL_PACKET", RVC_DGN_0ECFF_FIELDS, 7 },
    { 0xEE00, "ADDRESS_CLAIM", RVC_DGN_0EE00_FIELDS, 13 },
    { 0xEEFF, "ADDRESS_CLAIM", RVC_DGN_0EEFF_FIELDS, 0 },
    { 0xEF70, "WAKESPEED_BMS_QUERY", RVC_DGN_0EF70_FIELDS, 0 },
    { 0xEF80, "RENOGY_BMS_RESPONSE", RVC_DGN_0EF80_FIELDS, 0 },
    { 0xF004, "UNKNOWN-0F004", RVC_DGN_0F004_FIELDS, 1 },
    { 0xFC08, "UNKNOWN-0FC08", RVC_DGN_0FC08_FIELDS, 0 },
    { 0xFECA, "DM_1", RVC_DGN_0FECA_FIELDS, 0 },
    { 0xFED5, "J1939_ALTERNATOR_INFORMATION_1", RVC_DGN_0FED5_FIELDS, 2 },
    { 0xFEEB, "PRODUCT_IDENTIFICATION", RVC_DGN_0FEEB_FIELDS, 0 },
    { 0xFEF3, "GPS_POSITION", RVC_DGN_0FEF3_FIELDS, 0 },
    { 0xFF01, "UNKNOWN-0FF01", RVC_DGN_0FF01_FIELDS, 0 },
    { 0x10FFD, "DC_SOURCE_STATUS_SPYDER", RVC_DGN_10FFD_FIELDS, 0 },
    { 0x15FCE, "G12_CONFIGURATION", RVC_DGN_15FCE_FIELDS, 1 },
    { 0x1AAB7, "UNKNOWN-1AAB7", RVC_DGN_1AAB7_FIELDS, 0 },
    { 0x1AADC, "UNKNOWN-1AADC", RVC_DGN_1AADC_FIELDS, 0 },
    { 0x1AAFD, "DC_SOURCE_STATUS_G12", RVC_DGN_1AAFD_FIELDS, 0 },
    { 0x1EE00, "UNKNOWN-1EE00", RVC_DGN_1EE00_FIELDS, 0 },
    { 0x1EF65, "TIMBERLINE_PROPRIETARY", RVC_DGN_1EF65_FIELDS, 1 },
    { 0x1FACE, "UNKNOWN-1FACE", RVC_DGN_1FACE_FIELDS, 0 },
    { 0x1FACF, "UNKNOWN-1FACF", RVC_DGN_1FACF_FIELDS, 0 },
    { 0x1FBDA, "G12_INPUT_STATUS", RVC_DGN_1FBDA_FIELDS, 2 },
    { 0x1FDCB, "INVERTER_TEMPERATURE_STATUS_2", RVC_DGN_1FDCB_FIELDS, 4 },
    { 0x1FDE2, "ROOF_FAN_COMMAND_2", RVC_DGN_1FDE2_FIELDS, 0 },
    { 0x1FDE3, "ROOF_FAN_STATUS_2", RVC_DGN_1FDE3_FIELDS, 9 },
    { 0x1FDF9, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_4", RVC_DGN_1FDF9_FIELDS, 0 },
    { 0x1FDFA, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_4", RVC_DGN_1FDFA_FIELDS, 6 },
    { 0x1FDFB, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_3", RVC_DGN_1FDFB_FIELDS, 0 },
    { 0x1FDFC, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_3", RVC_DGN_1FDFC_FIELDS, 4 },
    { 0x1FDFD, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_2", RVC_DGN_1FDFD_FIELDS, 0 },
    { 0x1FDFE, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_2", RVC_DGN_1FDFE_FIELDS, 4 },
    { 0x1FDFF, "SOLAR_CONTROLLER_SOLAR_ARRAY_STATUS", RVC_DGN_1FDFF_FIELDS, 3 },
    { 0x1FE80, "SOLAR_CONTROLLER_BATTERY_STATUS", RVC_DGN_1FE80_FIELDS, 6 },
    { 0x1FE81, "SOLAR_CONTROLLER_STATUS_6", RVC_DGN_1FE81_FIELDS, 3 },
    { 0x1FE82, "SOLAR_CONTROLLER_STATUS_5", RVC_DGN_1FE82_FIELDS, 3 },
    { 0x1FE83, "SOLAR_CONTROLLER_STATUS_4", RVC_DGN_1FE83_FIELDS, 4 },
    { 0x1FE84, "SOLAR_CONTROLLER_STATUS_3", RVC_DGN_1FE84_FIELDS, 4 },
    { 0x1FE85, "SOLAR_CONTROLLER_STATUS_2", RVC_DGN_1FE85_FIELDS, 7 },
    { 0x1FE8B, "BATTERY_STATUS_11", RVC_DGN_1FE8B_FIELDS, 8 },
    { 0x1FE8C, "BATTERY_STATUS_10", RVC_DGN_1FE8C_FIELDS, 4 },
    { 0x1FE8D, "BATTERY_STATUS_9", RVC_DGN_1FE8D_FIELDS, 0 },
    { 0x1FE8E, "BATTERY_STATUS_8", RVC_DGN_1FE8E_FIELDS, 0 },
    { 0x1FE8F, "BATTERY_STATUS_7", RVC_DGN_1FE8F_FIELDS, 0 },
    { 0x1FE90, "BATTERY_STATUS_6", RVC_DGN_1FE90_FIELDS, 0 },
    { 0x1FE91, "BATTERY_STATUS_5", RVC_DGN_1FE91_FIELDS, 0 },
    { 0x1FE92, "BATTERY_STATUS_4", RVC_DGN_1FE92_FIELDS, 6 },
    { 0x1FE93, "BATTERY_STATUS_3", RVC_DGN_1FE93_FIELDS, 0 },
    { 0x1FE94, "BATTERY_STATUS_2", RVC_DGN_1FE94_FIELDS, 6 },
    { 0x1FE95, "BATTERY_STATUS_1", RVC_DGN_1FE95_FIELDS, 4 },
    { 0x1FE96, "CIRCULATION_PUMP_COMMAND", RVC_DGN_1FE96_FIELDS, 2 },
    { 0x1FE97, "CIRCULATION_PUMP_STATUS", RVC_DGN_1FE97_FIELDS, 5 },
    { 0x1FE98, "WATERHEATER_COMMAND2", RVC_DGN_1FE98_FIELDS, 3 },
    { 0x1FE99, "WATERHEATER_STATUS_2", RVC_DGN_1FE99_FIELDS, 25 },
    { 0x1FE9F, "GENERIC_ALARM_STATUS", RVC_DGN_1FE9F_FIELDS, 11 },
    { 0x1FEA3, "CHARGER_STATUS_2", RVC_DGN_1FEA3_FIELDS, 6 },
    { 0x1FEA5, "BATTERY_STATUS_11", RVC_DGN_1FEA5_FIELDS, 0 },
    { 0x1FEA6, "ROOF_FAN_COMMAND_1", RVC_DGN_1FEA6_FIELDS, 11 },
    { 0x1FEA7, "ROOF_FAN_STATUS_1", RVC_DGN_1FEA7_FIELDS, 10 },
    { 0x1FEAD, "SOLAR_EQUALIZATION_CONFIGURATION_COMMAND", RVC_DGN_1FEAD_FIELDS, 0 },
    { 0x1FEAE, "SOLAR_EQUALIZATION_CONFIGURATION_STATUS", RVC_DGN_1FEAE_FIELDS, 4 },
    { 0x1FEAF, "SOLAR_EQUALIZATION_STATUS", RVC_DGN_1FEAF_FIELDS, 4 },
    { 0x1FEB0, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND", RVC_DGN_1FEB0_FIELDS, 8 },
    { 0x1FEB1, "SOLAR_CONTROLLER_COMMAND", RVC_DGN_1FEB1_FIELDS, 5 },
    { 0x1FEB2, "SOLAR_CONTROLLER_CONFIGURATION_STATUS", RVC_DGN_1FEB2_FIELDS, 8 },
    { 0x1FEB3, "SOLAR_CONTROLLER_STATUS", RVC_DGN_1FEB3_FIELDS, 8 },
    { 0x1FEB4, "GENERIC_ACFAULT_CONFIGURATION_COMMAND_2", RVC_DGN_1FEB4_FIELDS, 0 },
    { 0x1FEB5, "GENERIC_ACFAULT_CONFIGURATION_COMMAND_1", RVC_DGN_1FEB5_FIELDS, 0 },
    { 0x1FEB6, "GENERIC_ACFAULT_CONFIGURATION_STATUS_2", RVC_DGN_1FEB6_FIELDS, 0 },
    { 0x1FEB7, "GENERIC_ACFAULT_CONFIGURATION_STATUS_1", RVC_DGN_1FEB7_FIELDS, 0 },
    { 0x1FEB8, "GENERIC_AC_STATUS_4", RVC_DGN_1FEB8_FIELDS, 2 },
    { 0x1FEB9, "GENERIC_AC_STATUS_3", RVC_DGN_1FEB9_FIELDS, 2 },
    { 0x1FEBA, "GENERIC_AC_STATUS_2", RVC_DGN_1FEBA_FIELDS, 2 },
    { 0x1FEBB, "GENERIC_AC_STATUS_1", RVC_DGN_1FEBB_FIELDS, 2 },
    { 0x1FEBC, "HYDRAULIC_PUMP_COMMAND", RVC_DGN_1FEBC_FIELDS, 0 },
    { 0x1FEBD, "INVERTER_TEMPERATURE_STATUS", RVC_DGN_1FEBD_FIELDS, 4 },
    { 0x1FEBE, "CHARGER_CONFIGURATION_COMMAND_4", RVC_DGN_1FEBE_FIELDS, 0 },
    { 0x1FEBF, "CHARGER_CONFIGURATION_STATUS_4", RVC_DGN_1FEBF_FIELDS, 4 },
    { 0x1FEC0, "GENERATOR_DC_EQUALIZATION_CONFIGURATION_COMMAND", RVC_DGN_1FEC0_FIELDS, 0 },
    { 0x1FEC1, "GENERATOR_DC_EQUALIZATION_CONFIGURATION_STATUS", RVC_DGN_1FEC1_FIELDS, 0 },
    { 0x1FEC2, "GENERATOR_DC_EQUALIZATION_STATUS", RVC_DGN_1FEC2_FIELDS, 0 },
    { 0x1FEC3, "GENERATOR_DC_CONFIGURATION_COMMAND", RVC_DGN_1FEC3_FIELDS, 0 },
    { 0x1FEC4, "GENERATOR_DC_COMMAND", RVC_DGN_1FEC4_FIELDS, 0 },
    { 0x1FEC5, "GENERATOR_DC_CONFIGURATION_STATUS", RVC_DGN_1FEC5_FIELDS, 0 },
    { 0x1FEC6, "GENERATOR_DC_STATUS_1", RVC_DGN_1FEC6_FIELDS, 5 },
    { 0x1FEC7, "DC_SOURCE_STATUS_6", RVC_DGN_1FEC7_FIELDS, 6 },
    { 0x1FEC8, "DC_SOURCE_STATUS_5", RVC_DGN_1FEC8_FIELDS, 4 },
    { 0x1FEC9, "DC_SOURCE_STATUS_4", RVC_DGN_1FEC9_FIELDS, 6 },
    { 0x1FECA, "DM_RV", RVC_DGN_1FECA_FIELDS, 11 },
    { 0x1FECB, "CHARGER_CONFIGURATION_COMMAND_3", RVC_DGN_1FECB_FIELDS, 0 },
    { 0x1FECC, "CHARGER_CONFIGURATION_STATUS_3", RVC_DGN_1FECC_FIELDS, 5 },
    { 0x1FECD, "INVERTER_CONFIGURATION_COMMAND_3", RVC_DGN_1FECD_FIELDS, 3 },
    { 0x1FECE, "INVERTER_CONFIGURATION_STATUS_3", RVC_DGN_1FECE_FIELDS, 3 },
    { 0x1FECF, "DC_DISCONNECT_COMMAND", RVC_DGN_1FECF_FIELDS, 0 },
    { 0x1FED0, "DC_DISCONNECT_STATUS", RVC_DGN_1FED0_FIELDS, 0 },
    { 0x1FED1, "SUSPENSION_AIR_PRESSURE_STATUS", RVC_DGN_1FED1_FIELDS, 7 },
    { 0x1FED2, "AGS_CRITERION_STATUS_2", RVC_DGN_1FED2_FIELDS, 0 },
    { 0x1FED3, "GPS_STATUS", RVC_DGN_1FED3_FIELDS, 0 },
    { 0x1FED4, "AGS_DEMAND_CONFIGURATION_COMMAND", RVC_DGN_1FED4_FIELDS, 0 },
    { 0x1FED5, "AGS_DEMAND_CONFIGURATION_STATUS", RVC_DGN_1FED5_FIELDS, 0 },
    { 0x1FED6, "MFG_SPECIFIC_CLAIM_REQUEST", RVC_DGN_1FED6_FIELDS, 0 },
    { 0x1FED7, "GENERIC_INDICATOR_STATUS", RVC_DGN_1FED7_FIELDS, 8 },
    { 0x1FED8, "GENERIC_CONFIGURATION_STATUS", RVC_DGN_1FED8_FIELDS, 9 },
    { 0x1FED9, "GENERIC_INDICATOR_COMMAND", RVC_DGN_1FED9_FIELDS, 6 },
    { 0x1FEDA, "DC_DIMMER_STATUS_3", RVC_DGN_1FEDA_FIELDS, 12 },
    { 0x1FEDB, "DC_DIMMER_COMMAND_2", RVC_DGN_1FEDB_FIELDS, 7 },
    { 0x1FEDC, "DC_LOAD_STATUS_2", RVC_DGN_1FEDC_FIELDS, 7 },
    { 0x1FEDD, "AC_LOAD_STATUS_2", RVC_DGN_1FEDD_FIELDS, 7 },
    { 0x1FEDE, "WINDOW_SHADE_CONTROL_STATUS", RVC_DGN_1FEDE_FIELDS, 13 },
    { 0x1FEDF, "WINDOW_SHADE_CONTROL_COMMAND", RVC_DGN_1FEDF_FIELDS, 6 },
    { 0x1FEE0, "DC_MOTOR_CONTROL_STATUS", RVC_DGN_1FEE0_FIELDS, 0 },
    { 0x1FEE1, "DC_MOTOR_CONTROL_COMMAND", RVC_DGN_1FEE1_FIELDS, 0 },
    { 0x1FEE2, "WINDOW_COMMAND", RVC_DGN_1FEE2_FIELDS, 0 },
    { 0x1FEE3, "WINDOW_STATUS", RVC_DGN_1FEE3_FIELDS, 0 },
    { 0x1FEE4, "LOCK_COMMAND", RVC_DGN_1FEE4_FIELDS, 0 },
    { 0x1FEE5, "LOCK_STATUS", RVC_DGN_1FEE5_FIELDS, 2 },
    { 0x1FEE8, "INVERTER_DC_STATUS", RVC_DGN_1FEE8_FIELDS, 3 },
    { 0x1FEE9, "TIRE_ID_COMMAND", RVC_DGN_1FEE9_FIELDS, 0 },
    { 0x1FEEA, "TIRE_ID_STATUS", RVC_DGN_1FEEA_FIELDS, 0 },
    { 0x1FEEB, "TIRE_TEMPERATURE_CONFIGURATION_COMMAND", RVC_DGN_1FEEB_FIELDS, 0 },
    { 0x1FEEC, "TIRE_PRESSURE_CONFIGURATION_COMMAND", RVC_DGN_1FEEC_FIELDS, 0 },
    { 0x1FEED, "TIRE_PRESSURE_CONFIGURATION_STATUS", RVC_DGN_1FEED_FIELDS, 0 },
    { 0x1FEEE, "TIRE_TEMPERATURE_CONFIGURATION_STATUS", RVC_DGN_1FEEE_FIELDS, 0 },
    { 0x1FEEF, "TIRE_SLOW_LEAK_ALARM", RVC_DGN_1FEEF_FIELDS, 0 },
    { 0x1FEF0, "TIRE_STATUS", RVC_DGN_1FEF0_FIELDS, 0 },
    { 0x1FEF1, "TIRE_RAW_STATUS", RVC_DGN_1FEF1_FIELDS, 0 },
    { 0x1FEF2, "AWNING_COMMAND", RVC_DGN_1FEF2_FIELDS, 0 },
    { 0x1FEF3, "AWNING_STATUS", RVC_DGN_1FEF3_FIELDS, 0 },
    { 0x1FEF4, "THERMOSTAT_SCHEDULE_COMMAND_2", RVC_DGN_1FEF4_FIELDS, 0 },
    { 0x1FEF5, "THERMOSTAT_SCHEDULE_COMMAND_1", RVC_DGN_1FEF5_FIELDS, 0 },
    { 0x1FEF6, "THERMOSTAT_SCHEDULE_STATUS_2", RVC_DGN_1FEF6_FIELDS, 0 },
    { 0x1FEF7, "THERMOSTAT_SCHEDULE_STATUS_1", RVC_DGN_1FEF7_FIELDS, 6 },
    { 0x1FEF8, "THERMOSTAT_COMMAND_2", RVC_DGN_1FEF8_FIELDS, 2 },
    { 0x1FEF9, "THERMOSTAT_COMMAND_1", RVC_DGN_1FEF9_FIELDS, 0 },
    { 0x1FEFA, "THERMOSTAT_STATUS_2", RVC_DGN_1FEFA_FIELDS, 4 },
    { 0x1FEFB, "FLOOR_HEAT_COMMAND", RVC_DGN_1FEFB_FIELDS, 0 },
    { 0x1FEFC, "FLOOR_HEAT_STATUS", RVC_DGN_1FEFC_FIELDS, 0 },
    { 0x1FEFD, "AGS_CRITERION_COMMAND", RVC_DGN_1FEFD_FIELDS, 0 },
    { 0x1FEFE, "AGS_CRITERION_STATUS", RVC_DGN_1FEFE_FIELDS, 0 },
    { 0x1FEFF, "GENERATOR_DEMAND_COMMAND", RVC_DGN_1FEFF_FIELDS, 0 },
    { 0x1FF80, "GENERATOR_DEMAND_STATUS", RVC_DGN_1FF80_FIELDS, 0 },
    { 0x1FF81, "ATS_ACFAULT_CONFIGURATION_COMMAND_2", RVC_DGN_1FF81_FIELDS, 0 },
    { 0x1FF82, "ATS_ACFAULT_CONFIGURATION_COMMAND_1", RVC_DGN_1FF82_FIELDS, 0 },
    { 0x1FF83, "ATS_ACFAULT_CONFIGURATION_STATUS_2", RVC_DGN_1FF83_FIELDS, 0 },
    { 0x1FF84, "ATS_ACFAULT_CONFIGURATION_STATUS_1", RVC_DGN_1FF84_FIELDS, 0 },
    { 0x1FF85, "ATS_AC_STATUS_4", RVC_DGN_1FF85_FIELDS, 4 },
    { 0x1FF86, "CHARGER_ACFAULT_CONFIGURATION_COMMAND_2", RVC_DGN_1FF86_FIELDS, 0 },
    { 0x1FF87, "CHARGER_ACFAULT_CONFIGURATION_COMMAND_1", RVC_DGN_1FF87_FIELDS, 0 },
    { 0x1FF88, "CHARGER_ACFAULT_CONFIGURATION_STATUS_2", RVC_DGN_1FF88_FIELDS, 0 },
    { 0x1FF89, "CHARGER_ACFAULT_CONFIGURATION_STATUS_1", RVC_DGN_1FF89_FIELDS, 0 },
    { 0x1FF8A, "CHARGER_AC_STATUS_4", RVC_DGN_1FF8A_FIELDS, 3 },
    { 0x1FF8B, "INVERTER_ACFAULT_CONFIGURATION_COMMAND_2", RVC_DGN_1FF8B_FIELDS, 0 },
    { 0x1FF8C, "INVERTER_ACFAULT_CONFIGURATION_COMMAND_1", RVC_DGN_1FF8C_FIELDS, 0 },
    { 0x1FF8D, "INVERTER_ACFAULT_CONFIGURATION_STATUS_2", RVC_DGN_1FF8D_FIELDS, 0 },
    { 0x1FF8E, "INVERTER_ACFAULT_CONFIGURATION_STATUS_1", RVC_DGN_1FF8E_FIELDS, 0 },
    { 0x1FF8F, "INVERTER_AC_STATUS_4", RVC_DGN_1FF8F_FIELDS, 3 },
    { 0x1FF94, "GENERATOR_AC_STATUS_4", RVC_DGN_1FF94_FIELDS, 2 },
    { 0x1FF95, "CHARGER_CONFIGURATION_COMMAND_2", RVC_DGN_1FF95_FIELDS, 0 },
    { 0x1FF96, "CHARGER_CONFIGURATION_STATUS_2", RVC_DGN_1FF96_FIELDS, 6 },
    { 0x1FF97, "CHARGER_EQUALIZATION_CONFIGURATION_COMMAND", RVC_DGN_1FF97_FIELDS, 0 },
    { 0x1FF98, "CHARGER_EQUALIZATION_CONFIGURATION_STATUS", RVC_DGN_1FF98_FIELDS, 3 },
    { 0x1FF99, "CHARGER_EQUALIZATION_STATUS", RVC_DGN_1FF99_FIELDS, 3 },
    { 0x1FF9A, "HEAT_PUMP_COMMAND", RVC_DGN_1FF9A_FIELDS, 0 },
    { 0x1FF9B, "HEAT_PUMP_STATUS", RVC_DGN_1FF9B_FIELDS, 0 },
    { 0x1FF9C, "THERMOSTAT_AMBIENT_STATUS", RVC_DGN_1FF9C_FIELDS, 2 },
    { 0x1FF9F, "COMPASS_CALIBRATE_COMMAND", RVC_DGN_1FF9F_FIELDS, 0 },
    { 0x1FFA0, "COMPASS_BEARING_STATUS", RVC_DGN_1FFA0_FIELDS, 0 },
    { 0x1FFA1, "WEATHER_CALIBRATE_COMMAND", RVC_DGN_1FFA1_FIELDS, 0 },
    { 0x1FFA2, "ALTIMETER_COMMAND", RVC_DGN_1FFA2_FIELDS, 0 },
    { 0x1FFA3, "ALTIMETER_STATUS", RVC_DGN_1FFA3_FIELDS, 0 },
    { 0x1FFA4, "WEATHER_STATUS_2", RVC_DGN_1FFA4_FIELDS, 0 },
    { 0x1FFA5, "WEATHER_STATUS_1", RVC_DGN_1FFA5_FIELDS, 0 },
    { 0x1FFA9, "ATS_COMMAND", RVC_DGN_1FFA9_FIELDS, 0 },
    { 0x1FFAA, "ATS_STATUS", RVC_DGN_1FFAA_FIELDS, 0 },
    { 0x1FFAB, "ATS_AC_STATUS_3", RVC_DGN_1FFAB_FIELDS, 4 },
    { 0x1FFAC, "ATS_AC_STATUS_2", RVC_DGN_1FFAC_FIELDS, 4 },
    { 0x1FFAD, "ATS_AC_STATUS_1", RVC_DGN_1FFAD_FIELDS, 4 },
    { 0x1FFAE, "WASTEDUMP_COMMAND", RVC_DGN_1FFAE_FIELDS, 0 },
    { 0x1FFAF, "WASTEDUMP_STATUS", RVC_DGN_1FFAF_FIELDS, 0 },
    { 0x1FFB0, "AUTOFILL_COMMAND", RVC_DGN_1FFB0_FIELDS, 0 },
    { 0x1FFB1, "AUTOFILL_STATUS", RVC_DGN_1FFB1_FIELDS, 0 },
    { 0x1FFB2, "WATER_PUMP_COMMAND", RVC_DGN_1FFB2_FIELDS, 3 },
    { 0x1FFB3, "WATER_PUMP_STATUS", RVC_DGN_1FFB3_FIELDS, 7 },
    { 0x1FFB4, "TANK_GEOMETRY_COMMAND", RVC_DGN_1FFB4_FIELDS, 0 },
    { 0x1FFB5, "TANK_GEOMETRY_STATUS", RVC_DGN_1FFB5_FIELDS, 0 },
    { 0x1FFB6, "TANK_CALIBRATION_COMMAND", RVC_DGN_1FFB6_FIELDS, 5 },
    { 0x1FFB7, "TANK_STATUS", RVC_DGN_1FFB7_FIELDS, 5 },
    { 0x1FFB8, "DIGITAL_INPUT_STATUS", RVC_DGN_1FFB8_FIELDS, 0 },
    { 0x1FFB9, "DC_DIMMER_COMMAND", RVC_DGN_1FFB9_FIELDS, 0 },
    { 0x1FFBA, "DC_DIMMER_STATUS_2", RVC_DGN_1FFBA_FIELDS, 9 },
    { 0x1FFBB, "DC_DIMMER_STATUS_1", RVC_DGN_1FFBB_FIELDS, 7 },
    { 0x1FFBC, "DC_LOAD_COMMAND", RVC_DGN_1FFBC_FIELDS, 7 },
    { 0x1FFBD, "DC_LOAD_STATUS", RVC_DGN_1FFBD_FIELDS, 0 },
    { 0x1FFBE, "AC_LOAD_COMMAND", RVC_DGN_1FFBE_FIELDS, 8 },
    { 0x1FFBF, "AC_LOAD_STATUS", RVC_DGN_1FFBF_FIELDS, 9 },
    { 0x1FFC4, "CHARGER_CONFIGURATION_COMMAND", RVC_DGN_1FFC4_FIELDS, 8 },
    { 0x1FFC5, "CHARGER_COMMAND", RVC_DGN_1FFC5_FIELDS, 5 },
    { 0x1FFC6, "CHARGER_CONFIGURATION_STATUS", RVC_DGN_1FFC6_FIELDS, 8 },
    { 0x1FFC7, "CHARGER_STATUS", RVC_DGN_1FFC7_FIELDS, 8 },
    { 0x1FFC8, "CHARGER_AC_STATUS_3", RVC_DGN_1FFC8_FIELDS, 3 },
    { 0x1FFC9, "CHARGER_AC_STATUS_2", RVC_DGN_1FFC9_FIELDS, 3 },
    { 0x1FFCA, "CHARGER_AC_STATUS_1", RVC_DGN_1FFCA_FIELDS, 3 },
    { 0x1FFCB, "INVERTER_OPE_STATUS", RVC_DGN_1FFCB_FIELDS, 0 },
    { 0x1FFCC, "INVERTER_DCBUS_STATUS", RVC_DGN_1FFCC_FIELDS, 0 },
    { 0x1FFCD, "INVERTER_APS_STATUS", RVC_DGN_1FFCD_FIELDS, 0 },
    { 0x1FFCE, "INVERTER_STATISTIC_STATUS", RVC_DGN_1FFCE_FIELDS, 0 },
    { 0x1FFCF, "INVERTER_CONFIGURATION_COMMAND_2", RVC_DGN_1FFCF_FIELDS, 4 },
    { 0x1FFD0, "INVERTER_CONFIGURATION_COMMAND_1", RVC_DGN_1FFD0_FIELDS, 4 },
    { 0x1FFD1, "INVERTER_CONFIGURATION_STATUS_2", RVC_DGN_1FFD1_FIELDS, 4 },
    { 0x1FFD2, "INVERTER_CONFIGURATION_STATUS_1", RVC_DGN_1FFD2_FIELDS, 7 },
    { 0x1FFD3, "INVERTER_COMMAND", RVC_DGN_1FFD3_FIELDS, 7 },
    { 0x1FFD4, "INVERTER_STATUS", RVC_DGN_1FFD4_FIELDS, 7 },
    { 0x1FFD5, "INVERTER_AC_STATUS_3", RVC_DGN_1FFD5_FIELDS, 3 },
    { 0x1FFD6, "INVERTER_AC_STATUS_2", RVC_DGN_1FFD6_FIELDS, 3 },
    { 0x1FFD7, "INVERTER_AC_STATUS_1", RVC_DGN_1FFD7_FIELDS, 3 },
    { 0x1FFD8, "GENERATOR_START_CONFIG_COMMAND", RVC_DGN_1FFD8_FIELDS, 0 },
    { 0x1FFD9, "GENERATOR_START_CONFIG_STATUS", RVC_DGN_1FFD9_FIELDS, 0 },
    { 0x1FFDA, "GENERATOR_COMMAND", RVC_DGN_1FFDA_FIELDS, 1 },
    { 0x1FFDB, "GENERATOR_STATUS_2", RVC_DGN_1FFDB_FIELDS, 8 },
    { 0x1FFDC, "GENERATOR_STATUS_1", RVC_DGN_1FFDC_FIELDS, 4 },
    { 0x1FFDD, "GENERATOR_AC_STATUS_3", RVC_DGN_1FFDD_FIELDS, 2 },
    { 0x1FFDE, "GENERATOR_AC_STATUS_2", RVC_DGN_1FFDE_FIELDS, 2 },
    { 0x1FFDF, "GENERATOR_AC_STATUS_1", RVC_DGN_1FFDF_FIELDS, 2 },
    { 0x1FFE0, "AIR_CONDITIONER_COMMAND", RVC_DGN_1FFE0_FIELDS, 0 },
    { 0x1FFE1, "AIR_CONDITIONER_STATUS", RVC_DGN_1FFE1_FIELDS, 8 },
    { 0x1FFE2, "THERMOSTAT_STATUS_1", RVC_DGN_1FFE2_FIELDS, 7 },
    { 0x1FFE3, "FURNACE_COMMAND", RVC_DGN_1FFE3_FIELDS, 7 },
    { 0x1FFE4, "FURNACE_STATUS", RVC_DGN_1FFE4_FIELDS, 11 },
    { 0x1FFE5, "SLIDE_MOTOR_STATUS", RVC_DGN_1FFE5_FIELDS, 0 },
    { 0x1FFE6, "SLIDE_SENSOR_STATUS", RVC_DGN_1FFE6_FIELDS, 0 },
    { 0x1FFE7, "SLIDE_COMMAND", RVC_DGN_1FFE7_FIELDS, 0 },
    { 0x1FFE8, "SLIDE_STATUS", RVC_DGN_1FFE8_FIELDS, 0 },
    { 0x1FFE9, "LEVELING_AIR_STATUS", RVC_DGN_1FFE9_FIELDS, 0 },
    { 0x1FFEA, "HYDRAULIC_PUMP_STATUS", RVC_DGN_1FFEA_FIELDS, 0 },
    { 0x1FFEB, "LEVELING_SENSOR_STATUS", RVC_DGN_1FFEB_FIELDS, 0 },
    { 0x1FFEC, "LEVELING_JACK_STATUS", RVC_DGN_1FFEC_FIELDS, 0 },
    { 0x1FFED, "LEVELING_CONTROL_STATUS", RVC_DGN_1FFED_FIELDS, 0 },
    { 0x1FFEE, "LEVELING_CONTROL_COMMAND", RVC_DGN_1FFEE_FIELDS, 0 },
    { 0x1FFEF, "AAS_SENSOR_STATUS", RVC_DGN_1FFEF_FIELDS, 26 },
    { 0x1FFF0, "AAS_STATUS", RVC_DGN_1FFF0_FIELDS, 7 },
    { 0x1FFF1, "AAS_COMMAND", RVC_DGN_1FFF1_FIELDS, 0 },
    { 0x1FFF2, "AAS_CONFIG_STATUS", RVC_DGN_1FFF2_FIELDS, 6 },
    { 0x1FFF3, "CHASSIS_MOBILITY_COMMAND", RVC_DGN_1FFF3_FIELDS, 6 },
    { 0x1FFF4, "CHASSIS_MOBILITY_STATUS", RVC_DGN_1FFF4_FIELDS, 9 },
    { 0x1FFF5, "GAS_SENSOR_STATUS", RVC_DGN_1FFF5_FIELDS, 7 },
    { 0x1FFF6, "WATERHEATER_COMMAND", RVC_DGN_1FFF6_FIELDS, 4 },
    { 0x1FFF7, "WATERHEATER_STATUS", RVC_DGN_1FFF7_FIELDS, 12 },
    { 0x1FFF8, "COMMUNICATION_STATUS_3", RVC_DGN_1FFF8_FIELDS, 3 },
    { 0x1FFF9, "COMMUNICATION_STATUS_2", RVC_DGN_1FFF9_FIELDS, 2 },
    { 0x1FFFA, "COMMUNICATION_STATUS_1", RVC_DGN_1FFFA_FIELDS, 3 },
    { 0x1FFFB, "DC_SOURCE_STATUS_3", RVC_DGN_1FFFB_FIELDS, 6 },
    { 0x1FFFC, "DC_SOURCE_STATUS_2", RVC_DGN_1FFFC_FIELDS, 5 },
    { 0x1FFFD, "DC_SOURCE_STATUS_1", RVC_DGN_1FFFD_FIELDS, 4 },
    { 0x1FFFE, "SET_DATE_TIME_COMMAND", RVC_DGN_1FFFE_FIELDS, 0 },
    { 0x1FFFF, "DATE_TIME_STATUS", RVC_DGN_1FFFF_FIELDS, 8 },
};

static const size_t RVC_DGN_DESC_COUNT = sizeof(RVC_DGN_DESCS) / sizeof(RVC_DGN_DESCS[0]);

#endif