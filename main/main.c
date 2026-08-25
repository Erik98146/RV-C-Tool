// Stand-alone RV-C Tools — Copperhill ESP32 CAN module.
//
// Ported 2026-08-24 from LaunchControl hub_core (the RV-C decode pipeline in
// its main.c, at HEAD — post frame-logger and TX-confirm). Everything
// hub-specific (cards, publish config, MQTT, BLE, Victron, displays, clock)
// is gone; what remains is the sniffer/decoder core: WATCHED_DGNS, the
// per-frame dispatch loop feeding the state DB and the /ws push stream, the
// diagnostic LED, and app_main bring-up.

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "rvc_spec_generated.h"
#include "lc_config.h"
#include "lc_types.h"
#include "lc_can.h"
#include "lc_state.h"
#include "lc_rvclog.h"
#include "lc_timberline.h"
#include "rt_web.h"

// Watched RV-C DGNs — copied verbatim from hub_core (sorted by DGN; the
// binary searches below depend on the order). const, so the ~3.1 KB table
// lives in flash; the only mutable part is s_watched_override[] below.
const watched_dgn_t WATCHED_DGNS[] = {
    {0xE8, "ACKNOWLEDGMENT", false},
    {0xEA, "REQUEST_FOR_DGN", false},
    {0x16C, "DC_COMPONENT_DRIVER_STATUS_4", false},
    {0x16F, "DC_COMPONENT_DRIVER_STATUS_1", false},
    {0x17D, "DOWNLOAD", false},
    {0x17E, "TERMINAL", false},
    {0x17F, "GENERAL_RESET", false},
    {0xBFC1, "G12_TANK_LEVEL_SENSOR", false},
    {0xEBFF, "DATA_PACKET", false},
    {0xECFF, "INITIAL_PACKET", false},
    {0xEE00, "ADDRESS_CLAIM", false},
    {0xEEFF, "ADDRESS_CLAIM", false},
    {0xEF70, "WAKESPEED_BMS_QUERY", false},
    {0xEF80, "RENOGY_BMS_RESPONSE", false},
    {0xF004, "UNKNOWN-0F004", false},
    {0xFC08, "UNKNOWN-0FC08", false},
    {0xFECA, "DM_1", false},
    {0xFED5, "J1939_ALTERNATOR_INFORMATION_1", false},
    {0xFEEB, "PRODUCT_IDENTIFICATION", false},
    {0xFEF3, "GPS_POSITION", false},
    {0xFF01, "UNKNOWN-0FF01", false},
    {0x10FFD, "DC_SOURCE_STATUS_SPYDER", false},
    {0x15FCE, "G12_CONFIGURATION", false},
    {0x1AAB7, "UNKNOWN-1AAB7", false},
    {0x1AADC, "UNKNOWN-1AADC", false},
    {0x1AAFD, "DC_SOURCE_STATUS_G12", false},
    {0x1EE00, "UNKNOWN-1EE00", false},
    {0x1EF65, "TIMBERLINE_PROPRIETARY", false},
    {0x1FACE, "UNKNOWN-1FACE", false},
    {0x1FACF, "UNKNOWN-1FACF", false},
    {0x1FBDA, "G12_INPUT_STATUS", false},
    {0x1FDCB, "INVERTER_TEMPERATURE_STATUS_2", false},
    {0x1FDE2, "ROOF_FAN_COMMAND_2", false},
    {0x1FDE3, "ROOF_FAN_STATUS_2", false},
    {0x1FDF9, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_4", false},
    {0x1FDFA, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_4", false},
    {0x1FDFB, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_3", false},
    {0x1FDFC, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_3", false},
    {0x1FDFD, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND_2", false},
    {0x1FDFE, "SOLAR_CONTROLLER_CONFIGURATION_STATUS_2", false},
    {0x1FDFF, "SOLAR_CONTROLLER_SOLAR_ARRAY_STATUS", false},
    {0x1FE80, "SOLAR_CONTROLLER_BATTERY_STATUS", false},
    {0x1FE81, "SOLAR_CONTROLLER_STATUS_6", false},
    {0x1FE82, "SOLAR_CONTROLLER_STATUS_5", false},
    {0x1FE83, "SOLAR_CONTROLLER_STATUS_4", false},
    {0x1FE84, "SOLAR_CONTROLLER_STATUS_3", false},
    {0x1FE85, "SOLAR_CONTROLLER_STATUS_2", false},
    {0x1FE8B, "BATTERY_STATUS_11", false},
    {0x1FE8C, "BATTERY_STATUS_10", false},
    {0x1FE8D, "BATTERY_STATUS_9", false},
    {0x1FE8E, "BATTERY_STATUS_8", false},
    {0x1FE8F, "BATTERY_STATUS_7", false},
    {0x1FE90, "BATTERY_STATUS_6", false},
    {0x1FE91, "BATTERY_STATUS_5", false},
    {0x1FE92, "BATTERY_STATUS_4", false},
    {0x1FE93, "BATTERY_STATUS_3", false},
    {0x1FE94, "BATTERY_STATUS_2", false},
    {0x1FE95, "BATTERY_STATUS_1", false},
    {0x1FE96, "CIRCULATION_PUMP_COMMAND", false},
    {0x1FE97, "CIRCULATION_PUMP_STATUS", false},
    {0x1FE98, "WATERHEATER_COMMAND2", false},
    {0x1FE99, "WATERHEATER_STATUS_2", false},
    {0x1FE9F, "GENERIC_ALARM_STATUS", false},
    {0x1FEA3, "CHARGER_STATUS_2", false},
    {0x1FEA5, "BATTERY_STATUS_11", false},
    {0x1FEA6, "ROOF_FAN_COMMAND_1", false},
    {0x1FEA7, "ROOF_FAN_STATUS_1", false},
    {0x1FEAD, "SOLAR_EQUALIZATION_CONFIGURATION_COMMAND", false},
    {0x1FEAE, "SOLAR_EQUALIZATION_CONFIGURATION_STATUS", false},
    {0x1FEAF, "SOLAR_EQUALIZATION_STATUS", false},
    {0x1FEB0, "SOLAR_CONTROLLER_CONFIGURATION_COMMAND", false},
    {0x1FEB1, "SOLAR_CONTROLLER_COMMAND", false},
    {0x1FEB2, "SOLAR_CONTROLLER_CONFIGURATION_STATUS", false},
    {0x1FEB3, "SOLAR_CONTROLLER_STATUS", false},
    {0x1FEB4, "GENERIC_ACFAULT_CONFIGURATION_COMMAND_2", false},
    {0x1FEB5, "GENERIC_ACFAULT_CONFIGURATION_COMMAND_1", false},
    {0x1FEB6, "GENERIC_ACFAULT_CONFIGURATION_STATUS_2", false},
    {0x1FEB7, "GENERIC_ACFAULT_CONFIGURATION_STATUS_1", false},
    {0x1FEB8, "GENERIC_AC_STATUS_4", false},
    {0x1FEB9, "GENERIC_AC_STATUS_3", false},
    {0x1FEBA, "GENERIC_AC_STATUS_2", false},
    {0x1FEBB, "GENERIC_AC_STATUS_1", false},
    {0x1FEBC, "HYDRAULIC_PUMP_COMMAND", false},
    {0x1FEBD, "INVERTER_TEMPERATURE_STATUS", false},
    {0x1FEBE, "CHARGER_CONFIGURATION_COMMAND_4", false},
    {0x1FEBF, "CHARGER_CONFIGURATION_STATUS_4", false},
    {0x1FEC0, "GENERATOR_DC_EQUALIZATION_CONFIGURATION_COMMAND", false},
    {0x1FEC1, "GENERATOR_DC_EQUALIZATION_CONFIGURATION_STATUS", false},
    {0x1FEC2, "GENERATOR_DC_EQUALIZATION_STATUS", false},
    {0x1FEC3, "GENERATOR_DC_CONFIGURATION_COMMAND", false},
    {0x1FEC4, "GENERATOR_DC_COMMAND", false},
    {0x1FEC5, "GENERATOR_DC_CONFIGURATION_STATUS", false},
    {0x1FEC6, "GENERATOR_DC_STATUS_1", false},
    {0x1FEC7, "DC_SOURCE_STATUS_6", false},
    {0x1FEC8, "DC_SOURCE_STATUS_5", false},
    {0x1FEC9, "DC_SOURCE_STATUS_4", false},
    {0x1FECA, "DM_RV", false},
    {0x1FECB, "CHARGER_CONFIGURATION_COMMAND_3", false},
    {0x1FECC, "CHARGER_CONFIGURATION_STATUS_3", false},
    {0x1FECD, "INVERTER_CONFIGURATION_COMMAND_3", false},
    {0x1FECE, "INVERTER_CONFIGURATION_STATUS_3", false},
    {0x1FECF, "DC_DISCONNECT_COMMAND", false},
    {0x1FED0, "DC_DISCONNECT_STATUS", false},
    {0x1FED1, "SUSPENSION_AIR_PRESSURE_STATUS", false},
    {0x1FED2, "AGS_CRITERION_STATUS_2", false},
    {0x1FED3, "GPS_STATUS", false},
    {0x1FED4, "AGS_DEMAND_CONFIGURATION_COMMAND", false},
    {0x1FED5, "AGS_DEMAND_CONFIGURATION_STATUS", false},
    {0x1FED6, "MFG_SPECIFIC_CLAIM_REQUEST", false},
    {0x1FED7, "GENERIC_INDICATOR_STATUS", false},
    {0x1FED8, "GENERIC_CONFIGURATION_STATUS", false},
    {0x1FED9, "GENERIC_INDICATOR_COMMAND", false},
    {0x1FEDA, "DC_DIMMER_STATUS_3", false},
    {0x1FEDB, "DC_DIMMER_COMMAND_2", false},
    {0x1FEDC, "DC_LOAD_STATUS_2", false},
    {0x1FEDD, "AC_LOAD_STATUS_2", false},
    {0x1FEDE, "WINDOW_SHADE_CONTROL_STATUS", false},
    {0x1FEDF, "WINDOW_SHADE_CONTROL_COMMAND", false},
    {0x1FEE0, "DC_MOTOR_CONTROL_STATUS", false},
    {0x1FEE1, "DC_MOTOR_CONTROL_COMMAND", false},
    {0x1FEE2, "WINDOW_COMMAND", false},
    {0x1FEE3, "WINDOW_STATUS", false},
    {0x1FEE4, "LOCK_COMMAND", false},
    {0x1FEE5, "LOCK_STATUS", false},
    {0x1FEE8, "INVERTER_DC_STATUS", false},
    {0x1FEE9, "TIRE_ID_COMMAND", false},
    {0x1FEEA, "TIRE_ID_STATUS", false},
    {0x1FEEB, "TIRE_TEMPERATURE_CONFIGURATION_COMMAND", false},
    {0x1FEEC, "TIRE_PRESSURE_CONFIGURATION_COMMAND", false},
    {0x1FEED, "TIRE_PRESSURE_CONFIGURATION_STATUS", false},
    {0x1FEEE, "TIRE_TEMPERATURE_CONFIGURATION_STATUS", false},
    {0x1FEEF, "TIRE_SLOW_LEAK_ALARM", false},
    {0x1FEF0, "TIRE_STATUS", false},
    {0x1FEF1, "TIRE_RAW_STATUS", false},
    {0x1FEF2, "AWNING_COMMAND", false},
    {0x1FEF3, "AWNING_STATUS", false},
    {0x1FEF4, "THERMOSTAT_SCHEDULE_COMMAND_2", false},
    {0x1FEF5, "THERMOSTAT_SCHEDULE_COMMAND_1", false},
    {0x1FEF6, "THERMOSTAT_SCHEDULE_STATUS_2", false},
    {0x1FEF7, "THERMOSTAT_SCHEDULE_STATUS_1", false},
    {0x1FEF8, "THERMOSTAT_COMMAND_2", false},
    {0x1FEF9, "THERMOSTAT_COMMAND_1", false},
    {0x1FEFA, "THERMOSTAT_STATUS_2", false},
    {0x1FEFB, "FLOOR_HEAT_COMMAND", false},
    {0x1FEFC, "FLOOR_HEAT_STATUS", false},
    {0x1FEFD, "AGS_CRITERION_COMMAND", false},
    {0x1FEFE, "AGS_CRITERION_STATUS", false},
    {0x1FEFF, "GENERATOR_DEMAND_COMMAND", false},
    {0x1FF80, "GENERATOR_DEMAND_STATUS", false},
    {0x1FF81, "ATS_ACFAULT_CONFIGURATION_COMMAND_2", false},
    {0x1FF82, "ATS_ACFAULT_CONFIGURATION_COMMAND_1", false},
    {0x1FF83, "ATS_ACFAULT_CONFIGURATION_STATUS_2", false},
    {0x1FF84, "ATS_ACFAULT_CONFIGURATION_STATUS_1", false},
    {0x1FF85, "ATS_AC_STATUS_4", false},
    {0x1FF86, "CHARGER_ACFAULT_CONFIGURATION_COMMAND_2", false},
    {0x1FF87, "CHARGER_ACFAULT_CONFIGURATION_COMMAND_1", false},
    {0x1FF88, "CHARGER_ACFAULT_CONFIGURATION_STATUS_2", false},
    {0x1FF89, "CHARGER_ACFAULT_CONFIGURATION_STATUS_1", false},
    {0x1FF8A, "CHARGER_AC_STATUS_4", false},
    {0x1FF8B, "INVERTER_ACFAULT_CONFIGURATION_COMMAND_2", false},
    {0x1FF8C, "INVERTER_ACFAULT_CONFIGURATION_COMMAND_1", false},
    {0x1FF8D, "INVERTER_ACFAULT_CONFIGURATION_STATUS_2", false},
    {0x1FF8E, "INVERTER_ACFAULT_CONFIGURATION_STATUS_1", false},
    {0x1FF8F, "INVERTER_AC_STATUS_4", false},
    {0x1FF94, "GENERATOR_AC_STATUS_4", false},
    {0x1FF95, "CHARGER_CONFIGURATION_COMMAND_2", false},
    {0x1FF96, "CHARGER_CONFIGURATION_STATUS_2", false},
    {0x1FF97, "CHARGER_EQUALIZATION_CONFIGURATION_COMMAND", false},
    {0x1FF98, "CHARGER_EQUALIZATION_CONFIGURATION_STATUS", false},
    {0x1FF99, "CHARGER_EQUALIZATION_STATUS", false},
    {0x1FF9A, "HEAT_PUMP_COMMAND", false},
    {0x1FF9B, "HEAT_PUMP_STATUS", false},
    {0x1FF9C, "THERMOSTAT_AMBIENT_STATUS", false},
    {0x1FF9F, "COMPASS_CALIBRATE_COMMAND", false},
    {0x1FFA0, "COMPASS_BEARING_STATUS", false},
    {0x1FFA1, "WEATHER_CALIBRATE_COMMAND", false},
    {0x1FFA2, "ALTIMETER_COMMAND", false},
    {0x1FFA3, "ALTIMETER_STATUS", false},
    {0x1FFA4, "WEATHER_STATUS_2", false},
    {0x1FFA5, "WEATHER_STATUS_1", false},
    {0x1FFA9, "ATS_COMMAND", false},
    {0x1FFAA, "ATS_STATUS", false},
    {0x1FFAB, "ATS_AC_STATUS_3", false},
    {0x1FFAC, "ATS_AC_STATUS_2", false},
    {0x1FFAD, "ATS_AC_STATUS_1", false},
    {0x1FFAE, "WASTEDUMP_COMMAND", false},
    {0x1FFAF, "WASTEDUMP_STATUS", false},
    {0x1FFB0, "AUTOFILL_COMMAND", false},
    {0x1FFB1, "AUTOFILL_STATUS", false},
    {0x1FFB2, "WATER_PUMP_COMMAND", false},
    {0x1FFB3, "WATER_PUMP_STATUS", false},
    {0x1FFB4, "TANK_GEOMETRY_COMMAND", false},
    {0x1FFB5, "TANK_GEOMETRY_STATUS", false},
    {0x1FFB6, "TANK_CALIBRATION_COMMAND", false},
    {0x1FFB7, "TANK_STATUS", false},
    {0x1FFB8, "DIGITAL_INPUT_STATUS", false},
    {0x1FFB9, "DC_DIMMER_COMMAND", false},
    {0x1FFBA, "DC_DIMMER_STATUS_2", false},
    {0x1FFBB, "DC_DIMMER_STATUS_1", false},
    {0x1FFBC, "DC_LOAD_COMMAND", false},
    {0x1FFBD, "DC_LOAD_STATUS", false},
    {0x1FFBE, "AC_LOAD_COMMAND", false},
    {0x1FFBF, "AC_LOAD_STATUS", false},
    {0x1FFC4, "CHARGER_CONFIGURATION_COMMAND", false},
    {0x1FFC5, "CHARGER_COMMAND", false},
    {0x1FFC6, "CHARGER_CONFIGURATION_STATUS", false},
    {0x1FFC7, "CHARGER_STATUS", false},
    {0x1FFC8, "CHARGER_AC_STATUS_3", false},
    {0x1FFC9, "CHARGER_AC_STATUS_2", false},
    {0x1FFCA, "CHARGER_AC_STATUS_1", false},
    {0x1FFCB, "INVERTER_OPE_STATUS", false},
    {0x1FFCC, "INVERTER_DCBUS_STATUS", false},
    {0x1FFCD, "INVERTER_APS_STATUS", false},
    {0x1FFCE, "INVERTER_STATISTIC_STATUS", false},
    {0x1FFCF, "INVERTER_CONFIGURATION_COMMAND_2", false},
    {0x1FFD0, "INVERTER_CONFIGURATION_COMMAND_1", false},
    {0x1FFD1, "INVERTER_CONFIGURATION_STATUS_2", false},
    {0x1FFD2, "INVERTER_CONFIGURATION_STATUS_1", false},
    {0x1FFD3, "INVERTER_COMMAND", false},
    {0x1FFD4, "INVERTER_STATUS", false},
    {0x1FFD5, "INVERTER_AC_STATUS_3", false},
    {0x1FFD6, "INVERTER_AC_STATUS_2", false},
    {0x1FFD7, "INVERTER_AC_STATUS_1", false},
    {0x1FFD8, "GENERATOR_START_CONFIG_COMMAND", false},
    {0x1FFD9, "GENERATOR_START_CONFIG_STATUS", false},
    {0x1FFDA, "GENERATOR_COMMAND", false},
    {0x1FFDB, "GENERATOR_STATUS_2", false},
    {0x1FFDC, "GENERATOR_STATUS_1", false},
    {0x1FFDD, "GENERATOR_AC_STATUS_3", false},
    {0x1FFDE, "GENERATOR_AC_STATUS_2", false},
    {0x1FFDF, "GENERATOR_AC_STATUS_1", false},
    {0x1FFE0, "AIR_CONDITIONER_COMMAND", false},
    {0x1FFE1, "AIR_CONDITIONER_STATUS", false},
    {0x1FFE2, "THERMOSTAT_STATUS_1", false},
    {0x1FFE3, "FURNACE_COMMAND", false},
    {0x1FFE4, "FURNACE_STATUS", false},
    {0x1FFE5, "SLIDE_MOTOR_STATUS", false},
    {0x1FFE6, "SLIDE_SENSOR_STATUS", false},
    {0x1FFE7, "SLIDE_COMMAND", false},
    {0x1FFE8, "SLIDE_STATUS", false},
    {0x1FFE9, "LEVELING_AIR_STATUS", false},
    {0x1FFEA, "HYDRAULIC_PUMP_STATUS", false},
    {0x1FFEB, "LEVELING_SENSOR_STATUS", false},
    {0x1FFEC, "LEVELING_JACK_STATUS", false},
    {0x1FFED, "LEVELING_CONTROL_STATUS", false},
    {0x1FFEE, "LEVELING_CONTROL_COMMAND", false},
    {0x1FFEF, "AAS_SENSOR_STATUS", false},
    {0x1FFF0, "AAS_STATUS", false},
    {0x1FFF1, "AAS_COMMAND", false},
    {0x1FFF2, "AAS_CONFIG_STATUS", false},
    {0x1FFF3, "CHASSIS_MOBILITY_COMMAND", false},
    {0x1FFF4, "CHASSIS_MOBILITY_STATUS", false},
    {0x1FFF5, "GAS_SENSOR_STATUS", false},
    {0x1FFF6, "WATERHEATER_COMMAND", false},
    {0x1FFF7, "WATERHEATER_STATUS", false},
    {0x1FFF8, "COMMUNICATION_STATUS_3", false},
    {0x1FFF9, "COMMUNICATION_STATUS_2", false},
    {0x1FFFA, "COMMUNICATION_STATUS_1", false},
    {0x1FFFB, "DC_SOURCE_STATUS_3", false},
    {0x1FFFC, "DC_SOURCE_STATUS_2", false},
    {0x1FFFD, "DC_SOURCE_STATUS_1", false},
    {0x1FFFE, "SET_DATE_TIME_COMMAND", false},
    {0x1FFFF, "DATE_TIME_STATUS", false},
};
const size_t WATCHED_DGN_COUNT = sizeof(WATCHED_DGNS) / sizeof(WATCHED_DGNS[0]);

/* Runtime enable/disable, one byte per row: 0 = follow the table's compiled-in
 * default, 1 = forced ON, 2 = forced OFF. An OVERRIDE, not a copy — plain
 * zero-initialised BSS is the correct starting state, so there is no boot-order
 * dependency (see hub_core main.c for the full reasoning). */
static uint8_t s_watched_override[sizeof(WATCHED_DGNS) / sizeof(WATCHED_DGNS[0])];

bool watched_dgn_is_enabled(size_t idx)
{
    if (idx >= WATCHED_DGN_COUNT) return false;
    const uint8_t o = s_watched_override[idx];
    return (o == 0) ? WATCHED_DGNS[idx].enabled : (o == 1);
}

void watched_dgn_set_enabled(size_t idx, bool enabled)
{
    if (idx >= WATCHED_DGN_COUNT) return;
    s_watched_override[idx] = enabled ? 1 : 2;
}

volatile int64_t last_valid_rx_time_us = 0;

// Diagnostics exposed at /api/state. rx_task_load_pct estimates only the
// dispatch task's busy time.
int64_t diag_window_start_us = 0;
uint32_t diag_last_rx_frames = 0;
uint64_t diag_last_rx_busy_us = 0;

const dgn_desc_t *find_dgn_desc(uint32_t dgn);
uint64_t extract_field_value(const uint8_t *data, uint8_t len, const field_desc_t *field);
const char *lookup_value_name(const field_desc_t *field, uint64_t value);
void format_decoded_number(char *out, size_t out_len, double value);
double convert_decoded_value(const field_desc_t *field, uint64_t raw_value);
void format_raw_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field);
const char *field_type_to_string(field_type_t type);
int16_t extract_instance(const dgn_desc_t *desc, const uint8_t *data, uint8_t len);
bool field_changed(const tracked_state_t *previous, const rvc_rx_frame_t *current, const field_desc_t *field);

void format_decoded_number(char *out, size_t out_len, double value)
{
    if (out_len == 0) return;

    if (value > -0.0000005 && value < 0.0000005) {
        value = 0.0;
    }

    double rounded = (value >= 0.0) ? (double)((int64_t)(value + 0.5)) : (double)((int64_t)(value - 0.5));
    if ((value - rounded) > -0.0000005 && (value - rounded) < 0.0000005) {
        snprintf(out, out_len, "%" PRId64, (int64_t)rounded);
        return;
    }

    snprintf(out, out_len, "%.3f", value);

    size_t len = strlen(out);
    while (len > 0 && out[len - 1] == '0') {
        out[len - 1] = '\0';
        len--;
    }
    if (len > 0 && out[len - 1] == '.') {
        out[len - 1] = '\0';
    }
}

double convert_decoded_value(const field_desc_t *field, uint64_t raw_value)
{
    double conv = (field != NULL) ? field->conv : 1.0;
    double offset = (field != NULL) ? field->offset : 0.0;

    if (conv == 0.0) {
        conv = 1.0;
    }

    double value = ((double)raw_value * conv) + offset;

    if (field != NULL && field->inv) {
        value = -value;
    }

    return value;
}

const char *field_type_to_string(field_type_t type)
{
    switch (type) {
        case FIELD_UINT: return "uint";
        case FIELD_BITS: return "bits";
        case FIELD_ASCII: return "ascii";
        case FIELD_RAW: return "raw";
        default: return "unknown";
    }
}

void format_raw_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field)
{
    if (out_len == 0) return;
    out[0] = '\0';

    if (field->type == FIELD_ASCII) {
        size_t pos = 0;
        for (uint8_t i = field->byte_start; i <= field->byte_end && i < len && i < 8 && pos + 1 < out_len; i++) {
            char c = (char)data[i];
            out[pos++] = (c >= 32 && c <= 126) ? c : '.';
        }
        out[pos] = '\0';
        return;
    }

    uint64_t value = extract_field_value(data, len, field);

    if (field->type == FIELD_BITS) {
        uint8_t width = field->bit_end - field->bit_start + 1;
        if (width >= out_len) width = out_len - 1;
        for (uint8_t i = 0; i < width; i++) {
            uint8_t bit = width - 1 - i;
            out[i] = ((value >> bit) & 1) ? '1' : '0';
        }
        out[width] = '\0';
    } else {
        snprintf(out, out_len, "%" PRIu64, value);
    }
}

#if LC_HAS_DIAG_LED
static void led_set(bool on)
{
    gpio_set_level(LED_GPIO, on ? 1 : 0);
}

static void led_init(void)
{
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    led_set(false);
}
#endif  // LC_HAS_DIAG_LED

static const watched_dgn_t *find_watched_dgn(uint32_t dgn)
{
    size_t lo = 0, hi = WATCHED_DGN_COUNT;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (WATCHED_DGNS[mid].dgn == dgn) {
            return watched_dgn_is_enabled(mid) ? &WATCHED_DGNS[mid] : NULL;
        }
        if (WATCHED_DGNS[mid].dgn < dgn) lo = mid + 1;
        else hi = mid;
    }
    return NULL;
}

const watched_dgn_t *find_watched_dgn_any(uint32_t dgn)
{
    size_t lo = 0, hi = WATCHED_DGN_COUNT;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (WATCHED_DGNS[mid].dgn == dgn) return &WATCHED_DGNS[mid];
        if (WATCHED_DGNS[mid].dgn < dgn) lo = mid + 1;
        else hi = mid;
    }
    return NULL;
}

static uint32_t normalize_dgn_any(uint32_t raw_dgn)
{
    if (find_watched_dgn_any(raw_dgn) != NULL) {
        return raw_dgn;
    }

    uint8_t pf = (raw_dgn >> 8) & 0xFF;
    if (pf < 240) {
        uint32_t short_dgn = raw_dgn >> 8;
        if (find_watched_dgn_any(short_dgn) != NULL) {
            return short_dgn;
        }
    }

    return raw_dgn;
}

const dgn_desc_t *find_dgn_desc(uint32_t dgn)
{
    size_t lo = 0, hi = RVC_DGN_DESC_COUNT;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (RVC_DGN_DESCS[mid].dgn == dgn) return &RVC_DGN_DESCS[mid];
        if (RVC_DGN_DESCS[mid].dgn < dgn) lo = mid + 1;
        else hi = mid;
    }
    return NULL;
}

static uint32_t get_raw_dgn_from_ext_id(uint32_t id)
{
    return (id >> 8) & 0x3FFFF;
}

static uint32_t normalize_dgn(uint32_t raw_dgn)
{
    // Most RV-C DGNs such as 0x1FEDA match directly.
    if (find_watched_dgn(raw_dgn) != NULL) {
        return raw_dgn;
    }

    // PDU1 / destination-specific frames appear as PF + destination in raw
    // bits; the RV-C reference lists these as shorter DGNs such as 0x0EA.
    uint8_t pf = (raw_dgn >> 8) & 0xFF;
    if (pf < 240) {
        uint32_t short_dgn = raw_dgn >> 8;
        if (find_watched_dgn(short_dgn) != NULL) {
            return short_dgn;
        }
    }

    return raw_dgn;
}

static uint32_t get_dgn_from_ext_id(uint32_t id)
{
    return normalize_dgn(get_raw_dgn_from_ext_id(id));
}

static uint64_t extract_little_endian_bytes(const uint8_t *data, uint8_t len, uint8_t byte_start, uint8_t byte_end)
{
    if (byte_start >= len) {
        return 0;
    }

    if (byte_end >= len) {
        byte_end = len - 1;
    }

    uint64_t value = 0;
    uint8_t shift = 0;

    for (uint8_t i = byte_start; i <= byte_end && i < 8; i++) {
        value |= ((uint64_t)data[i]) << shift;
        shift += 8;
    }

    return value;
}

uint64_t extract_field_value(const uint8_t *data, uint8_t len, const field_desc_t *field)
{
    uint64_t value = extract_little_endian_bytes(data, len, field->byte_start, field->byte_end);

    if (field->bit_start != 0 || field->bit_end != 7 || field->type == FIELD_BITS) {
        uint8_t width = field->bit_end - field->bit_start + 1;
        uint64_t mask;

        if (width >= 64) {
            mask = UINT64_MAX;
        } else {
            mask = ((uint64_t)1 << width) - 1;
        }

        value = (value >> field->bit_start) & mask;
    }

    return value;
}

const char *lookup_value_name(const field_desc_t *field, uint64_t value)
{
    if (field->values == NULL || field->value_count == 0) {
        return NULL;
    }

    for (uint16_t i = 0; i < field->value_count; i++) {
        if ((uint64_t)field->values[i].value == value) {
            return field->values[i].name;
        }
    }

    return NULL;
}

// Elwell Timberline's 1EF65 is MULTIPLEXED: byte 0 selects the layout of the
// remaining seven. Swap in the matching descriptor before anything decodes.
static const dgn_desc_t *resolve_desc(uint32_t dgn, const dgn_desc_t *desc,
                                      const uint8_t *data, uint8_t len)
{
    if (dgn != 0x1EF65u) return desc;
    const dgn_desc_t *variant = lc_timberline_desc_for_frame(data, len);
    return variant != NULL ? variant : desc;
}

static int64_t last_tracked_full_log_us = 0;

/* Transport-protocol and addressing DGNs: watched (so the sniffer still sees
 * them) but never given a tracked-state slot — they occur once per SOURCE
 * ADDRESS and no card/device can bind them, so on the hub they burned 15 of
 * 17 lost slots in a real capture. Same values, same reasoning here. */
static inline bool dgn_claims_tracked_state(uint32_t dgn)
{
    switch (dgn) {
    case 0xE8:      /* ACKNOWLEDGMENT   -- read by lc_web_tx_confirm_check() */
    case 0xEA:      /* REQUEST_FOR_DGN                                      */
    case 0xEBFF:    /* DATA_PACKET      -- multi-frame transport            */
    case 0xECFF:    /* INITIAL_PACKET   -- multi-frame transport            */
    case 0xEE00:    /* ADDRESS_CLAIM                                        */
    case 0xEEFF:    /* ADDRESS_CLAIM                                        */
        return false;
    default:
        return true;
    }
}

// ⚠ The variants of a multiplexed DGN MUST land in separate tracked states:
// using the message type as the instance keeps them distinct (they share
// (dgn, source) and carry no RV-C Instance field).
static int16_t resolve_instance(uint32_t dgn, const dgn_desc_t *desc,
                                const uint8_t *data, uint8_t len)
{
    if (dgn == 0x1EF65u && len >= 1) return (int16_t)data[0];
    return extract_instance(desc, data, len);
}

int16_t extract_instance(const dgn_desc_t *desc, const uint8_t *data, uint8_t len)
{
    if (desc == NULL) {
        return -1;
    }

    /* Pass 1: a field named exactly "Instance". */
    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];

        if (field->name != NULL && strcasecmp(field->name, "instance") == 0) {
            return (int16_t)extract_field_value(data, len, field);
        }
    }

    /* Pass 2: a field whose name ENDS in "Instance" — "Charger Instance",
     * "Dc Instance", etc. First match wins (matters for 1FEA3). See
     * hub_core main.c for the full history of why both passes exist. */
    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];
        if (field->name == NULL) continue;

        size_t n = strlen(field->name);
        if (n > 8 && strcasecmp(field->name + n - 8, "instance") == 0) {
            return (int16_t)extract_field_value(data, len, field);
        }
    }

    /* Pass 3: no instance field of any kind — single-instance per node, so
     * identity is (DGN, 0, source address). */
    return 0;
}

bool field_changed(const tracked_state_t *previous, const rvc_rx_frame_t *current, const field_desc_t *field)
{
    if (previous == NULL || previous->len == 0) {
        return true;
    }

    uint64_t old_value = extract_field_value(previous->data, previous->len, field);
    uint64_t new_value = extract_field_value(current->data, current->len, field);

    return old_value != new_value;
}

static void rvc_print_task(void *arg)
{
    rvc_rx_frame_t frame;
    uint32_t processed_since_yield = 0;

    // Subscribe to the task watchdog; the receive times out at 1 s so a
    // silent bus still feeds it. Failure is logged, never fatal.
    if (esp_task_wdt_add(NULL) != ESP_OK) {
        ESP_LOGW(TAG, "rvc_print_task: TWDT subscribe failed (continuing unwatched)");
    }

    while (1) {
        bool got_frame = (xQueueReceive(lc_can_rx_queue, &frame,
                                        pdMS_TO_TICKS(1000)) == pdTRUE);
        esp_task_wdt_reset();
        if (!got_frame) {
            continue;
        }

        int64_t process_start_us = esp_timer_get_time();
        diag_rx_frames++;

        // RV-C raw-frame capture — literal bus traffic, ahead of the
        // extended/RTR filter and everything else below, so a capture sees
        // exactly what arrived. No-op (single bool check) when idle.
        lc_rvclog_capture_frame(frame.id, frame.data, frame.len);

        if (!frame.is_extended || frame.is_rtr) {
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        // Closed-loop transmit confirm (see rt_web.c's /api/rvc/send): while
        // a send is waiting on the target's ACK/status echo, this spots it
        // and wakes the sender. A single volatile read when idle.
        lc_web_tx_confirm_check(frame.id, frame.data, frame.len);

        uint32_t raw_dgn = get_raw_dgn_from_ext_id(frame.id);
        int64_t now_us = process_start_us;

        lc_state_lock();

        bool discovery_mode = lc_state_is_select_all_mode();

        if (discovery_mode) {
            uint32_t dgn = normalize_dgn_any(raw_dgn);
            const watched_dgn_t *watched_any = find_watched_dgn_any(dgn);

            if (watched_any != NULL) {
                active_dgn_state_t *active = get_active_dgn_state(dgn);
                if (active != NULL) {
                    active->name = watched_any->name;
                    active->count++;
                    active->has_source_id = true;
                    active->source_id = (uint8_t)(frame.id & 0xFF);
                    active->has_data = true;
                    active->len = frame.len > 8 ? 8 : frame.len;
                    memcpy(active->data, frame.data, active->len);

                    const dgn_desc_t *discovery_desc = find_dgn_desc(dgn);
                    if (discovery_desc != NULL) {
                        discovery_desc = resolve_desc(dgn, discovery_desc, frame.data, frame.len);
                        int16_t discovery_instance = resolve_instance(dgn, discovery_desc, frame.data, frame.len);
                        lc_state_note_discovery_instance(dgn, watched_any->name, active->source_id, discovery_instance);
                    }

                    if ((now_us - active->last_sent_us) >= DISCOVERY_EVENT_MIN_US) {
                        active->last_sent_us = now_us;
#if ENABLE_WS_PUSH_EVENTS
                        ws_enqueue_activity_event(dgn, watched_any->name, active->count, active->has_source_id, active->source_id);
#endif
                    }
                }
            } else {
                uint32_t unknown_dgn = raw_dgn;
                active_dgn_state_t *active = get_unknown_active_state(unknown_dgn);
                if (active != NULL) {
                    active->count++;
                    active->has_source_id = true;
                    active->source_id = (uint8_t)(frame.id & 0xFF);
                    active->has_data = true;
                    active->len = frame.len > 8 ? 8 : frame.len;
                    memcpy(active->data, frame.data, active->len);
                    if ((now_us - active->last_sent_us) >= DISCOVERY_EVENT_MIN_US) {
                        active->last_sent_us = now_us;
#if ENABLE_WS_PUSH_EVENTS
                        ws_enqueue_activity_event(unknown_dgn, "UNKNOWN", active->count, active->has_source_id, active->source_id);
#endif
                    }
                }
            }

            lc_state_unlock();
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        uint32_t dgn = get_dgn_from_ext_id(frame.id);

        const watched_dgn_t *watched = find_watched_dgn(dgn);
        if (watched == NULL) {
            lc_state_unlock();
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        const dgn_desc_t *desc = find_dgn_desc(dgn);
        if (desc == NULL) {
            lc_state_unlock();
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }
        desc = resolve_desc(dgn, desc, frame.data, frame.len);

        int16_t instance = resolve_instance(dgn, desc, frame.data, frame.len);
        uint8_t source_id = (uint8_t)(frame.id & 0xFF);

        if (!dgn_claims_tracked_state(dgn)) {
            lc_state_unlock();
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        tracked_state_t *state = get_tracked_state(dgn, instance, source_id);

        if (state == NULL) {
            /* Table full — loud, rate-limited; silent was how the hub twice
             * mis-diagnosed vanishing devices. */
            lc_state_note_tracked_full();
            int64_t now_full_us = esp_timer_get_time();
            if (now_full_us - last_tracked_full_log_us > 30LL * 1000000LL) {
                last_tracked_full_log_us = now_full_us;
                ESP_LOGW(TAG, "tracked_states FULL (%d slots) -- DGN %05" PRIX32
                              " inst %d src 0x%02X and any other new device will "
                              "NOT appear until a slot frees. Raise MAX_TRACKED_STATES.",
                         MAX_TRACKED_STATES, dgn, instance, source_id);
            }
            lc_state_unlock();
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        // Variables captured under lock for WS enqueue outside the lock.
        // desc and desc->fields[] are static const — safe without lock.
        uint32_t ws_changed_mask = 0;  // bitmask of field indices with changed values
        bool     ws_baseline     = false;
        uint32_t ws_seq          = global_change_seq;

        if (!state->has_snapshot) {
            // Baseline frame: store initial field values.
            ws_baseline = true;
            ws_seq      = global_change_seq;
            update_tracked_state(state, &frame);
            last_valid_rx_time_us = now_us;
        } else if (frame_changed(state, &frame)) {
            // Single pass: detect changed fields and update per-field sequences.
            for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                if (field_changed(state, &frame, &desc->fields[i])) {
                    ws_changed_mask |= (1u << i);
                }
            }

            if (ws_changed_mask) {
                global_change_seq++;
                state->dgn_change_seq = global_change_seq;
                ws_seq = global_change_seq;
                for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                    if (ws_changed_mask & (1u << i)) {
                        state->field_change_seq[i] = global_change_seq;
                    }
                }
                last_valid_rx_time_us = now_us;
            }

            update_tracked_state(state, &frame);
        } else {
            // Frame data unchanged — just refresh the freshness stamp.
            state->last_update_us = esp_timer_get_time();
        }

        lc_state_unlock();

        diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);

        // WS enqueue runs outside the state lock. Unlike the hub (whose
        // dashboard path is publish-config-gated), the sniffer page here is
        // the ONLY consumer, and it takes plain field events over /ws.
#if ENABLE_WS_PUSH_EVENTS
        if (ws_baseline) {
            for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                ws_enqueue_field_event(dgn, desc->name, instance, &frame, &desc->fields[i], false, ws_seq);
            }
        } else if (ws_changed_mask) {
            for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                if (ws_changed_mask & (1u << i)) {
                    ws_enqueue_field_event(dgn, desc->name, instance, &frame, &desc->fields[i], true, ws_seq);
                }
            }
        }
#endif

        processed_since_yield++;
        if (processed_since_yield >= 32) {
            processed_since_yield = 0;
            taskYIELD();
        }
    }
}

#if LC_HAS_DIAG_LED
static void led_heartbeat_task(void *arg)
{
    bool heartbeat_on = false;
    int64_t heartbeat_start_us = 0;
    int64_t last_heartbeat_us = 0;

    while (1) {
        int64_t now_us = esp_timer_get_time();

        bool valid_rx_recent =
            last_valid_rx_time_us > 0 &&
            (now_us - last_valid_rx_time_us) <= (VALID_RX_HOLD_MS * 1000);

        if (valid_rx_recent) {
            led_set(true);
            heartbeat_on = false;
        } else {
            if (heartbeat_on) {
                if ((now_us - heartbeat_start_us) >= (HEARTBEAT_ON_MS * 1000)) {
                    led_set(false);
                    heartbeat_on = false;
                }
            } else {
                if ((now_us - last_heartbeat_us) >= (HEARTBEAT_PERIOD_MS * 1000)) {
                    led_set(true);
                    heartbeat_on = true;
                    heartbeat_start_us = now_us;
                    last_heartbeat_us = now_us;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
#endif  // LC_HAS_DIAG_LED

static void lc_nvs_init_or_recover(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
        return;
    }

    ESP_ERROR_CHECK(ret);
}

void app_main(void)
{
    printf("\nRV-C Tools (stand-alone) — %s\n", LC_BOARD_NAME);
    printf("Watching %u RV-C DGNs\n", (unsigned)WATCHED_DGN_COUNT);

    lc_nvs_init_or_recover();

    ESP_ERROR_CHECK(lc_state_init());

    // Just creates a mutex — must exist before rvc_print_task starts calling
    // lc_rvclog_capture_frame, since a capture could be started from the
    // very first httpd request.
    lc_rvclog_init();

    // Initialize CAN/TWAI before the web/Wi-Fi stack so the RX queue is
    // allocated while the heap is still relatively unconstrained — on the
    // WROOM-32 this ordering is load-bearing, not stylistic.
    ESP_ERROR_CHECK(lc_can_init());

    // rvc_print_task (the RX queue's only consumer) must exist before
    // anything slow runs: lc_can_init() enabled the bus, so the ISR is
    // already pushing frames into a 64-slot queue. Starting Wi-Fi/httpd
    // first would drop the boot burst.
    xTaskCreate(
        rvc_print_task,
        "rvc_print_task",
        8192,
        NULL,
        5,
        NULL
    );

    ESP_ERROR_CHECK(rt_web_init());

#if LC_HAS_DIAG_LED
    led_init();
    xTaskCreate(
        led_heartbeat_task,
        "led_heartbeat_task",
        2048,
        NULL,
        4,
        NULL
    );
#endif

    ESP_LOGI(TAG, "Board: %s", LC_BOARD_NAME);
    ESP_LOGI(TAG, "Free heap after init: %u bytes internal",
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    ESP_LOGI(TAG, "TWAI node enabled — 250 kbps, TX GPIO %d / RX GPIO %d",
             CAN_TX_GPIO, CAN_RX_GPIO);
#if LC_HAS_DIAG_LED
    ESP_LOGI(TAG, "LED GPIO: %d", LED_GPIO);
#endif
    ESP_LOGI(TAG, "All watched DGNs default disabled");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
