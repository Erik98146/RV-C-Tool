#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>


#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_spiffs.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "driver/gpio.h"

#include "esp_twai.h"
#include "esp_twai_onchip.h"

#include "rvc_spec_generated.h"

#define TAG "RVC_SNIFFER"

// Copperhill / SK Pang ESP32 CAN board
#define CAN_TX_GPIO GPIO_NUM_25
#define CAN_RX_GPIO GPIO_NUM_26

// External diagnostic LED. Connect GPIO2 -> 330 ohm resistor -> LED anode, LED cathode -> GND.
#define LED_GPIO GPIO_NUM_2

#define RX_QUEUE_LENGTH 256
#define MAX_TRACKED_STATES 512
#define FIELD_VALUE_STRING_MAX 96
#define API_RESPONSE_MAX_BYTES 120000
#define SELECT_ALL_MODE_MAX_TRACKED_STATES 48
#define SELECT_ALL_MODE_MAX_API_INSTANCES 32
#define SELECT_ALL_MODE_MIN_UPDATE_US 750000

// LED behavior
#define VALID_RX_HOLD_MS      500
#define HEARTBEAT_ON_MS       40
#define HEARTBEAT_PERIOD_MS   1000

// Wi-Fi softAP / captive portal settings
#define WIFI_AP_SSID          "rvc-tools"
#define WIFI_AP_CHANNEL       6
#define WIFI_AP_MAX_CLIENTS   4
#define WIFI_AP_IP_STRING     "192.168.4.1"
#define WIFI_STA_MAX_SCAN_RESULTS 16
#define DNS_PORT              53
#define DNS_TASK_STACK        4096

// WebSocket delta-stream settings
#define MAX_WS_CLIENTS         2
#define WS_EVENT_QUEUE_LENGTH  16
#define WS_EVENT_MAX_BYTES     768

// Performance controls. The browser uses /api/state polling as the reliable data path.
// Disable websocket field spam and serial printf bursts unless actively debugging.
#define ENABLE_WS_PUSH_EVENTS 0
#define ENABLE_SERIAL_CHANGE_LOG 0

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

// Watched RV-C DGNs from rvc-spec.yml.
// Change true to false, or comment out a line, to disable reporting for that DGN.
static watched_dgn_t WATCHED_DGNS[] = {
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
static const size_t WATCHED_DGN_COUNT = sizeof(WATCHED_DGNS) / sizeof(WATCHED_DGNS[0]);

#define DISCOVERY_EVENT_MIN_US 1000000
#define MAX_UNKNOWN_ACTIVE_DGNS 64
#define WIFI_RECONNECT_DELAY_MS 5000

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

static QueueHandle_t rx_queue = NULL;
static volatile int64_t last_valid_rx_time_us = 0;
static tracked_state_t tracked_states[MAX_TRACKED_STATES];
static SemaphoreHandle_t state_mutex = NULL;
static uint32_t global_change_seq = 0;
static bool select_all_mode = false;
static active_dgn_state_t active_dgns[sizeof(WATCHED_DGNS) / sizeof(WATCHED_DGNS[0])];
static active_dgn_state_t unknown_active_dgns[MAX_UNKNOWN_ACTIVE_DGNS];

// Diagnostics exposed at /api/state. rx_task_load_pct estimates only this sniffer task's busy time.
static volatile uint32_t diag_rx_frames = 0;
static volatile uint32_t diag_rx_dropped_isr = 0;
static uint64_t diag_rx_busy_us = 0;
static int64_t diag_window_start_us = 0;
static uint32_t diag_last_rx_frames = 0;
static uint64_t diag_last_rx_busy_us = 0;

static QueueHandle_t ws_event_queue = NULL;
static httpd_handle_t web_server = NULL;
static twai_node_handle_t g_twai_node = NULL;
static int ws_client_fds[MAX_WS_CLIENTS] = {-1, -1};
static SemaphoreHandle_t ws_client_mutex = NULL;
static esp_netif_t *sta_netif = NULL;
static volatile bool wifi_sta_connected = false;
static volatile bool wifi_sta_got_ip = false;
static char wifi_sta_ssid[33] = {0};
static char wifi_sta_ip[16] = {0};
static char saved_wifi_ssid[33] = {0};
static char saved_wifi_password[65] = {0};
static volatile bool wifi_reconnect_task_pending = false;

static const dgn_desc_t *find_dgn_desc(uint32_t dgn);
static uint64_t extract_field_value(const uint8_t *data, uint8_t len, const field_desc_t *field);
static const char *lookup_value_name(const field_desc_t *field, uint64_t value);
static void format_decoded_number(char *out, size_t out_len, double value);
static double convert_decoded_value(const field_desc_t *field, uint64_t raw_value);
static void format_raw_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field);
static const char *field_type_to_string(field_type_t type);
static int16_t extract_instance(const dgn_desc_t *desc, const uint8_t *data, uint8_t len);
static void clear_tracked_state_and_rx_queue(void);
static bool any_decoded_field_changed(const dgn_desc_t *desc, const rvc_rx_frame_t *frame, const tracked_state_t *previous);
static bool field_changed(const tracked_state_t *previous, const rvc_rx_frame_t *current, const field_desc_t *field);
static void clear_tracked_state_and_rx_queue(void);
static void ws_enqueue_clear(void);
static void ws_enqueue_field_event(uint32_t dgn, const char *dgn_name, int16_t instance, const rvc_rx_frame_t *frame, const field_desc_t *field, bool changed, uint32_t seq);
static void ws_enqueue_activity_event(uint32_t dgn, const char *dgn_name, uint32_t count, bool has_source_id, uint8_t source_id);
static const watched_dgn_t *find_watched_dgn_any(uint32_t dgn);
static void url_decode_in_place(char *s);
static esp_err_t api_wifi_status_get_handler(httpd_req_t *req);
static esp_err_t api_wifi_scan_get_handler(httpd_req_t *req);
static esp_err_t api_wifi_connect_get_handler(httpd_req_t *req);
static esp_err_t api_rvc_send_get_handler(httpd_req_t *req);
static bool load_wifi_credentials_from_nvs(char *ssid, size_t ssid_len, char *password, size_t password_len);
static esp_err_t save_wifi_credentials_to_nvs(const char *ssid, const char *password);
static esp_err_t wifi_sta_apply_and_connect(const char *ssid, const char *password);
static void wifi_reconnect_task(void *arg);
static void schedule_wifi_reconnect(void);
static active_dgn_state_t *get_unknown_active_state(uint32_t dgn);


static void spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 4,
        .format_if_mount_failed = false,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS: %s", esp_err_to_name(ret));
        return;
    }

    size_t total = 0;
    size_t used = 0;
    ret = esp_spiffs_info("storage", &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS mounted: total=%u, used=%u", (unsigned)total, (unsigned)used);
    } else {
        ESP_LOGW(TAG, "SPIFFS mounted, but info failed: %s", esp_err_to_name(ret));
    }
}

static esp_err_t send_file_from_spiffs(httpd_req_t *req, const char *path, const char *content_type)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Could not open file: %s", path);
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    }

    httpd_resp_set_type(req, content_type);
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    char chunk[512];
    size_t read_bytes;
    esp_err_t ret = ESP_OK;

    while ((read_bytes = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        ret = httpd_resp_send_chunk(req, chunk, read_bytes);
        if (ret != ESP_OK) {
            fclose(file);
            httpd_resp_sendstr_chunk(req, NULL);
            return ret;
        }
    }

    fclose(file);
    return httpd_resp_sendstr_chunk(req, NULL);
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    return send_file_from_spiffs(req, "/spiffs/index.html", "text/html");
}

static void json_append(char *buf, size_t max, size_t *pos, const char *fmt, ...)
{
    if (*pos >= max) return;
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(buf + *pos, max - *pos, fmt, args);
    va_end(args);
    if (written < 0) return;
    if ((size_t)written >= max - *pos) *pos = max - 1;
    else *pos += (size_t)written;
}

static void json_escape_append(char *buf, size_t max, size_t *pos, const char *text)
{
    json_append(buf, max, pos, "\"");
    if (text != NULL) {
        for (const char *p = text; *p != '\0'; p++) {
            char c = *p;
            if (c == '"' || c == '\\') json_append(buf, max, pos, "\\%c", c);
            else if (c == '\n') json_append(buf, max, pos, "\\n");
            else if (c == '\r') json_append(buf, max, pos, "\\r");
            else if (c == '\t') json_append(buf, max, pos, "\\t");
            else if ((unsigned char)c < 32) json_append(buf, max, pos, " ");
            else json_append(buf, max, pos, "%c", c);
        }
    }
    json_append(buf, max, pos, "\"");
}

static void format_decoded_number(char *out, size_t out_len, double value)
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

static double convert_decoded_value(const field_desc_t *field, uint64_t raw_value)
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

static void format_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field)
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

    uint64_t raw_value = extract_field_value(data, len, field);
    const char *value_name = lookup_value_name(field, raw_value);

    char decoded_text[48];
    format_decoded_number(decoded_text, sizeof(decoded_text), convert_decoded_value(field, raw_value));

    if (value_name != NULL && field->unit != NULL && strlen(field->unit) > 0) {
        snprintf(out, out_len, "%s = %s %s", decoded_text, value_name, field->unit);
    } else if (value_name != NULL) {
        snprintf(out, out_len, "%s = %s", decoded_text, value_name);
    } else if (field->unit != NULL && strlen(field->unit) > 0) {
        snprintf(out, out_len, "%s %s", decoded_text, field->unit);
    } else {
        snprintf(out, out_len, "%s", decoded_text);
    }
}

static const char *field_type_to_string(field_type_t type)
{
    switch (type) {
        case FIELD_UINT: return "uint";
        case FIELD_BITS: return "bits";
        case FIELD_ASCII: return "ascii";
        case FIELD_RAW: return "raw";
        default: return "unknown";
    }
}

static void format_raw_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field)
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


static esp_err_t state_chunk_flush(httpd_req_t *req, char *chunk, size_t *pos)
{
    if (*pos == 0) return ESP_OK;
    esp_err_t ret = httpd_resp_send_chunk(req, chunk, *pos);
    *pos = 0;
    return ret;
}

static esp_err_t state_chunk_append(httpd_req_t *req, char *chunk, size_t chunk_len, size_t *pos, const char *fmt, ...)
{
    if (*pos >= chunk_len - 192) {
        esp_err_t ret = state_chunk_flush(req, chunk, pos);
        if (ret != ESP_OK) return ret;
    }

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(chunk + *pos, chunk_len - *pos, fmt, args);
    va_end(args);

    if (written < 0) return ESP_FAIL;
    if ((size_t)written >= chunk_len - *pos) return ESP_ERR_NO_MEM;
    *pos += (size_t)written;
    return ESP_OK;
}

static esp_err_t state_chunk_escape_append(httpd_req_t *req, char *chunk, size_t chunk_len, size_t *pos, const char *text)
{
    esp_err_t ret = state_chunk_append(req, chunk, chunk_len, pos, "\"");
    if (ret != ESP_OK) return ret;

    if (text != NULL) {
        for (const char *p = text; *p != '\0'; p++) {
            char c = *p;
            if (*pos >= chunk_len - 16) {
                ret = state_chunk_flush(req, chunk, pos);
                if (ret != ESP_OK) return ret;
            }
            if (c == '"' || c == '\\') ret = state_chunk_append(req, chunk, chunk_len, pos, "\\%c", c);
            else if (c == '\n') ret = state_chunk_append(req, chunk, chunk_len, pos, "\\n");
            else if (c == '\r') ret = state_chunk_append(req, chunk, chunk_len, pos, "\\r");
            else if (c == '\t') ret = state_chunk_append(req, chunk, chunk_len, pos, "\\t");
            else if ((unsigned char)c < 32) ret = state_chunk_append(req, chunk, chunk_len, pos, " ");
            else ret = state_chunk_append(req, chunk, chunk_len, pos, "%c", c);
            if (ret != ESP_OK) return ret;
        }
    }

    return state_chunk_append(req, chunk, chunk_len, pos, "\"");
}

static esp_err_t api_state_get_handler(httpd_req_t *req)
{
    char chunk[1536];
    size_t pos = 0;
    esp_err_t ret = ESP_OK;

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);

    int64_t now_us = esp_timer_get_time();
    uint32_t free_heap = esp_get_free_heap_size();
    uint32_t min_free_heap = esp_get_minimum_free_heap_size();
    UBaseType_t rx_waiting = rx_queue != NULL ? uxQueueMessagesWaiting(rx_queue) : 0;
    UBaseType_t ws_waiting = ws_event_queue != NULL ? uxQueueMessagesWaiting(ws_event_queue) : 0;

    if (diag_window_start_us == 0) diag_window_start_us = now_us;
    int64_t window_us = now_us - diag_window_start_us;
    uint32_t rx_frames_delta = diag_rx_frames - diag_last_rx_frames;
    uint64_t rx_busy_delta = diag_rx_busy_us - diag_last_rx_busy_us;
    uint32_t rx_task_load_pct = 0;
    uint32_t rx_fps = 0;
    if (window_us > 0) {
        uint64_t load = (rx_busy_delta * 100ULL) / (uint64_t)window_us;
        rx_task_load_pct = load > 100 ? 100 : (uint32_t)load;
        rx_fps = (uint32_t)(((uint64_t)rx_frames_delta * 1000000ULL) / (uint64_t)window_us);
    }
    diag_window_start_us = now_us;
    diag_last_rx_frames = diag_rx_frames;
    diag_last_rx_busy_us = diag_rx_busy_us;

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                             "{\"ok\":true,\"seq\":%" PRIu32 ",\"mode\":\"%s\",\"now_us\":%" PRId64 ",",
                             global_change_seq,
                             select_all_mode ? "discovery" : "selected",
                             now_us);
    if (ret != ESP_OK) goto done;

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                             "\"diag\":{\"free_heap\":%u,\"min_free_heap\":%u,\"rx_queue\":%u,\"ws_queue\":%u,\"rx_frames\":%u,\"rx_fps\":%u,\"rx_dropped\":%u,\"rx_task_load_pct\":%u},",
                             free_heap, min_free_heap, (unsigned)rx_waiting, (unsigned)ws_waiting,
                             (unsigned)diag_rx_frames, rx_fps, (unsigned)diag_rx_dropped_isr, rx_task_load_pct);
    if (ret != ESP_OK) goto done;

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"active\":[");
    if (ret != ESP_OK) goto done;

    bool first = true;
    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        if (!active_dgns[i].seen) continue;

        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) goto done;
        }
        first = false;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"name\":",
                                 active_dgns[i].dgn);
        if (ret != ESP_OK) goto done;

        ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                        active_dgns[i].name != NULL ? active_dgns[i].name : "UNKNOWN");
        if (ret != ESP_OK) goto done;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 ",\"count\":%u", active_dgns[i].count);
        if (ret != ESP_OK) goto done;

        if (active_dgns[i].has_source_id) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"source_id\":%u", (unsigned)active_dgns[i].source_id);
            if (ret != ESP_OK) goto done;
        }

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "}");
        if (ret != ESP_OK) goto done;
    }

    for (size_t i = 0; i < MAX_UNKNOWN_ACTIVE_DGNS; i++) {
        if (!unknown_active_dgns[i].seen) continue;
        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) goto done;
        }
        first = false;
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"name\":\"UNKNOWN-0x%05" PRIX32 "\",\"count\":%u,\"unknown\":true",
                                 unknown_active_dgns[i].dgn,
                                 unknown_active_dgns[i].dgn,
                                 unknown_active_dgns[i].count);
        if (ret != ESP_OK) goto done;
        if (unknown_active_dgns[i].has_source_id) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"source_id\":%u", (unsigned)unknown_active_dgns[i].source_id);
            if (ret != ESP_OK) goto done;
        }
        if (unknown_active_dgns[i].has_data) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"len\":%u,\"data\":[", (unsigned)unknown_active_dgns[i].len);
            if (ret != ESP_OK) goto done;
            for (uint8_t b = 0; b < unknown_active_dgns[i].len && b < 8; b++) {
                if (b > 0) {
                    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
                    if (ret != ESP_OK) goto done;
                }
                ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "%u", (unsigned)unknown_active_dgns[i].data[b]);
                if (ret != ESP_OK) goto done;
            }
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "]");
            if (ret != ESP_OK) goto done;
        }
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "}");
        if (ret != ESP_OK) goto done;
    }

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "],\"states\":[");
    if (ret != ESP_OK) goto done;

    first = true;
    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        tracked_state_t *state = &tracked_states[i];
        if (!state->valid || !state->has_snapshot) continue;

        const dgn_desc_t *desc = find_dgn_desc(state->dgn);
        if (desc == NULL) continue;

        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) goto done;
        }
        first = false;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"dgn_name\":",
                                 state->dgn);
        if (ret != ESP_OK) goto done;

        ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                        desc->name != NULL ? desc->name : "UNKNOWN");
        if (ret != ESP_OK) goto done;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"instance\":");
        if (ret != ESP_OK) goto done;

        if (state->instance >= 0) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"%d\"", state->instance);
        } else {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"n/a\"");
        }
        if (ret != ESP_OK) goto done;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 ",\"source_id\":%u,\"last_update_us\":%" PRId64 ",\"fields\":[",
                                 state->source_id,
                                 state->last_update_us);
        if (ret != ESP_OK) goto done;

        bool first_field = true;
        for (uint16_t f = 0; f < desc->field_count && f < 32; f++) {
            const field_desc_t *field = &desc->fields[f];

            char value_text[FIELD_VALUE_STRING_MAX];
            char raw_value_text[FIELD_VALUE_STRING_MAX];
            format_field_value(value_text, sizeof(value_text), state->data, state->len, field);
            format_raw_field_value(raw_value_text, sizeof(raw_value_text), state->data, state->len, field);
            uint64_t numeric_value = extract_field_value(state->data, state->len, field);
            const char *value_desc = lookup_value_name(field, numeric_value);

            if (!first_field) {
                ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
                if (ret != ESP_OK) goto done;
            }
            first_field = false;

            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "{\"name\":");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                            field->name != NULL ? field->name : "value");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"value\":");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, value_text);
            if (ret != ESP_OK) goto done;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"raw_value\":");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, raw_value_text);
            if (ret != ESP_OK) goto done;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"value_description\":");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, value_desc != NULL ? value_desc : "");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"unit\":");
            if (ret != ESP_OK) goto done;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, field->unit != NULL ? field->unit : "");
            if (ret != ESP_OK) goto done;

            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"type\":\"%s\",\"byte_start\":%u,\"byte_end\":%u,\"bit_start\":%u,\"bit_end\":%u,\"seq\":%u}",
                                     field_type_to_string(field->type),
                                     field->byte_start,
                                     field->byte_end,
                                     field->bit_start,
                                     field->bit_end,
                                     state->field_change_seq[f]);
            if (ret != ESP_OK) goto done;
        }

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "]}");
        if (ret != ESP_OK) goto done;
    }

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "]}");
    if (ret != ESP_OK) goto done;

    ret = state_chunk_flush(req, chunk, &pos);
    if (ret != ESP_OK) goto done;

    ret = httpd_resp_send_chunk(req, NULL, 0);

done:
    if (state_mutex != NULL) xSemaphoreGive(state_mutex);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "/api/state chunk send failed: %s", esp_err_to_name(ret));
    }
    return ret;
}


static esp_err_t api_enable_get_handler(httpd_req_t *req)
{
    char query[96] = {0};
    char dgn_text[24] = {0};
    char enabled_text[8] = {0};

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "dgn", dgn_text, sizeof(dgn_text)) != ESP_OK ||
        httpd_query_key_value(query, "enabled", enabled_text, sizeof(enabled_text)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected dgn and enabled query parameters");
    }

    uint32_t dgn = (uint32_t)strtoul(dgn_text, NULL, 0);
    bool enabled = atoi(enabled_text) != 0;

    if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
    select_all_mode = false;

    bool found = false;
    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        if (WATCHED_DGNS[i].dgn == dgn) {
            WATCHED_DGNS[i].enabled = enabled;
            found = true;
            break;
        }
    }

    if (state_mutex != NULL) xSemaphoreGive(state_mutex);

    if (!found) {
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "DGN not found");
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}


static esp_err_t api_deselect_all_get_handler(httpd_req_t *req)
{
    if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
    select_all_mode = false;

    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        WATCHED_DGNS[i].enabled = false;
    }
    clear_tracked_state_and_rx_queue();

    if (state_mutex != NULL) xSemaphoreGive(state_mutex);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}


static void clear_tracked_state_and_rx_queue(void)
{
    memset(tracked_states, 0, sizeof(tracked_states));
    global_change_seq++;
    if (rx_queue != NULL) {
        xQueueReset(rx_queue);
    }
}

static esp_err_t api_select_all_get_handler(httpd_req_t *req)
{
    if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
    select_all_mode = true;

    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        WATCHED_DGNS[i].enabled = false;
    }
    memset(active_dgns, 0, sizeof(active_dgns));
    memset(unknown_active_dgns, 0, sizeof(unknown_active_dgns));
    clear_tracked_state_and_rx_queue();

    if (state_mutex != NULL) xSemaphoreGive(state_mutex);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true,\"mode\":\"discovery\"}");
}

static void ws_add_client(int fd)
{
    if (ws_client_mutex != NULL) xSemaphoreTake(ws_client_mutex, portMAX_DELAY);

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_client_fds[i] == fd) {
            if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);
            return;
        }
    }

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_client_fds[i] < 0) {
            ws_client_fds[i] = fd;
            ESP_LOGI(TAG, "WebSocket client registered fd=%d", fd);
            if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);
            return;
        }
    }

    // If all slots are full, replace the oldest slot. This keeps the server from
    // holding stale clients forever after phone/laptop reconnects.
    ESP_LOGW(TAG, "WebSocket client table full; replacing fd=%d with fd=%d", ws_client_fds[0], fd);
    ws_client_fds[0] = fd;

    if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);
}

static void ws_remove_client(int fd)
{
    if (ws_client_mutex != NULL) xSemaphoreTake(ws_client_mutex, portMAX_DELAY);

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_client_fds[i] == fd) {
            ws_client_fds[i] = -1;
            ESP_LOGI(TAG, "WebSocket client removed fd=%d", fd);
        }
    }

    if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);
}

static void ws_enqueue_text(const char *text)
{
    if (ws_event_queue == NULL || text == NULL) {
        return;
    }

    ws_event_t event = {0};
    strncpy(event.text, text, sizeof(event.text) - 1);

    if (xQueueSend(ws_event_queue, &event, 0) != pdTRUE) {
        // Drop the oldest pending event and try once more. Never block CAN processing.
        ws_event_t dropped;
        (void)xQueueReceive(ws_event_queue, &dropped, 0);
        (void)xQueueSend(ws_event_queue, &event, 0);
    }
}

static void ws_enqueue_clear(void)
{
    ws_enqueue_text("{\"type\":\"clear\"}");
}

static void ws_enqueue_activity_event(uint32_t dgn, const char *dgn_name, uint32_t count, bool has_source_id, uint8_t source_id)
{
    char event[WS_EVENT_MAX_BYTES];
    size_t pos = 0;

    json_append(event, sizeof(event), &pos, "{\"type\":\"activity\",\"dgn\":\"0x%05" PRIX32 "\",\"name\":", dgn);
    json_escape_append(event, sizeof(event), &pos, dgn_name != NULL ? dgn_name : "UNKNOWN");
    json_append(event, sizeof(event), &pos, ",\"count\":%u", count);
    if (has_source_id) {
        json_append(event, sizeof(event), &pos, ",\"source_id\":%u", (unsigned)source_id);
    }
    json_append(event, sizeof(event), &pos, "}");

    ws_enqueue_text(event);
}

static void ws_enqueue_field_event(uint32_t dgn, const char *dgn_name, int16_t instance, const rvc_rx_frame_t *frame, const field_desc_t *field, bool changed, uint32_t seq)
{
    if (frame == NULL || field == NULL) {
        return;
    }

    char value_text[FIELD_VALUE_STRING_MAX];
    char raw_value_text[FIELD_VALUE_STRING_MAX];
    format_field_value(value_text, sizeof(value_text), frame->data, frame->len, field);
    format_raw_field_value(raw_value_text, sizeof(raw_value_text), frame->data, frame->len, field);
    uint64_t numeric_value = extract_field_value(frame->data, frame->len, field);
    const char *value_desc = lookup_value_name(field, numeric_value);

    char event[WS_EVENT_MAX_BYTES];
    size_t pos = 0;

    json_append(event, sizeof(event), &pos, "{\"type\":\"field\",\"seq\":%u,\"changed\":%s,\"dgn\":\"0x%05" PRIX32 "\",\"dgn_name\":",
                seq, changed ? "true" : "false", dgn);
    json_escape_append(event, sizeof(event), &pos, dgn_name != NULL ? dgn_name : "UNKNOWN");
    json_append(event, sizeof(event), &pos, ",\"instance\":");
    if (instance >= 0) json_append(event, sizeof(event), &pos, "\"%d\"", instance);
    else json_append(event, sizeof(event), &pos, "\"n/a\"");
    json_append(event, sizeof(event), &pos, ",\"source_id\":%u", (unsigned)(frame->id & 0xFF));
    json_append(event, sizeof(event), &pos, ",\"field\":{\"name\":");
    json_escape_append(event, sizeof(event), &pos, field->name != NULL ? field->name : "value");
    json_append(event, sizeof(event), &pos, ",\"value\":");
    json_escape_append(event, sizeof(event), &pos, value_text);
    json_append(event, sizeof(event), &pos, ",\"raw_value\":");
    json_escape_append(event, sizeof(event), &pos, raw_value_text);
    json_append(event, sizeof(event), &pos, ",\"value_description\":");
    json_escape_append(event, sizeof(event), &pos, value_desc != NULL ? value_desc : "");
    json_append(event, sizeof(event), &pos, ",\"unit\":");
    json_escape_append(event, sizeof(event), &pos, field->unit != NULL ? field->unit : "");
    json_append(event, sizeof(event), &pos, ",\"type\":");
    json_escape_append(event, sizeof(event), &pos, field_type_to_string(field->type));
    json_append(event, sizeof(event), &pos, ",\"byte_start\":%u,\"byte_end\":%u,\"bit_start\":%u,\"bit_end\":%u,\"seq\":%u}}",
                field->byte_start, field->byte_end, field->bit_start, field->bit_end, seq);

    ws_enqueue_text(event);
}

static void ws_delta_task(void *arg)
{
    ws_event_t event;

    while (1) {
        if (xQueueReceive(ws_event_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if (web_server == NULL) {
            continue;
        }

        int clients[MAX_WS_CLIENTS];
        int client_count = 0;

        if (ws_client_mutex != NULL) xSemaphoreTake(ws_client_mutex, portMAX_DELAY);
        for (int i = 0; i < MAX_WS_CLIENTS; i++) {
            if (ws_client_fds[i] >= 0) {
                clients[client_count++] = ws_client_fds[i];
            }
        }
        if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);

        for (int i = 0; i < client_count; i++) {
            httpd_ws_frame_t ws_pkt = {
                .final = true,
                .fragmented = false,
                .type = HTTPD_WS_TYPE_TEXT,
                .payload = (uint8_t *)event.text,
                .len = strlen(event.text),
            };

            esp_err_t ret = httpd_ws_send_frame_async(web_server, clients[i], &ws_pkt);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "WebSocket async send failed fd=%d: %s", clients[i], esp_err_to_name(ret));
                ws_remove_client(clients[i]);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void ws_handle_command(const char *cmd)
{
    if (cmd == NULL) return;

    if (strcmp(cmd, "select_all") == 0) {
        // Version 2 behavior: Select All enters lightweight discovery mode.
        // It DOES NOT decode/track every DGN continuously. That was the source
        // of the lockups. Activity events identify active DGNs, and the user can
        // enable specific DGNs for full decode.
        if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
        select_all_mode = true;
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            WATCHED_DGNS[i].enabled = false;
        }
        memset(active_dgns, 0, sizeof(active_dgns));
    memset(unknown_active_dgns, 0, sizeof(unknown_active_dgns));
        clear_tracked_state_and_rx_queue();
        if (state_mutex != NULL) xSemaphoreGive(state_mutex);
        ws_enqueue_clear();
        ws_enqueue_text("{\"type\":\"mode\",\"mode\":\"discovery\"}");
        return;
    }

    if (strcmp(cmd, "deselect_all") == 0) {
        if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
        select_all_mode = false;
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            WATCHED_DGNS[i].enabled = false;
        }
        memset(active_dgns, 0, sizeof(active_dgns));
    memset(unknown_active_dgns, 0, sizeof(unknown_active_dgns));
        clear_tracked_state_and_rx_queue();
        if (state_mutex != NULL) xSemaphoreGive(state_mutex);
        ws_enqueue_clear();
        ws_enqueue_text("{\"type\":\"mode\",\"mode\":\"idle\"}");
        return;
    }

    if (strncmp(cmd, "enable:", 7) == 0) {
        char dgn_text[24] = {0};
        char enabled_text[8] = {0};
        const char *p = cmd + 7;
        const char *colon = strchr(p, ':');
        if (colon == NULL) return;

        size_t dgn_len = colon - p;
        if (dgn_len >= sizeof(dgn_text)) dgn_len = sizeof(dgn_text) - 1;
        memcpy(dgn_text, p, dgn_len);
        strncpy(enabled_text, colon + 1, sizeof(enabled_text) - 1);

        uint32_t dgn = (uint32_t)strtoul(dgn_text, NULL, 0);
        bool enabled = atoi(enabled_text) != 0;

        if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);
        select_all_mode = false;
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            if (WATCHED_DGNS[i].dgn == dgn) {
                WATCHED_DGNS[i].enabled = enabled;
                break;
            }
        }
        clear_tracked_state_and_rx_queue();
        if (state_mutex != NULL) xSemaphoreGive(state_mutex);
        ws_enqueue_clear();
        ws_enqueue_text("{\"type\":\"mode\",\"mode\":\"selected\"}");
        return;
    }
}

static esp_err_t websocket_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        int fd = httpd_req_to_sockfd(req);
        ESP_LOGI(TAG, "WebSocket client connected fd=%d", fd);
        ws_add_client(fd);
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt = {0};
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    if (ws_pkt.len > 127) {
        return ESP_ERR_INVALID_SIZE;
    }

    char payload[128] = {0};
    ws_pkt.payload = (uint8_t *)payload;

    ret = httpd_ws_recv_frame(req, &ws_pkt, sizeof(payload) - 1);
    if (ret != ESP_OK) {
        return ret;
    }

    payload[ws_pkt.len] = '\0';
    ws_handle_command(payload);

    // Commands are acknowledged by async hello/clear messages. Do not send the
    // full state here; this is intentionally delta-only.
    return ESP_OK;
}


static void url_decode_in_place(char *s)
{
    char *src = s;
    char *dst = s;

    while (*src != '\0') {
        if (*src == '%' && src[1] != '\0' && src[2] != '\0') {
            char hex[3] = {src[1], src[2], '\0'};
            char *end = NULL;
            long value = strtol(hex, &end, 16);
            if (end != NULL && *end == '\0') {
                *dst++ = (char)value;
                src += 3;
                continue;
            }
        }

        if (*src == '+') {
            *dst++ = ' ';
            src++;
            continue;
        }

        *dst++ = *src++;
    }

    *dst = '\0';
}

static bool load_wifi_credentials_from_nvs(char *ssid, size_t ssid_len, char *password, size_t password_len)
{
    if (ssid == NULL || password == NULL || ssid_len == 0 || password_len == 0) return false;
    ssid[0] = '\0';
    password[0] = '\0';
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("network", NVS_READONLY, &handle);
    if (ret != ESP_OK) return false;
    size_t required = ssid_len;
    ret = nvs_get_str(handle, "ssid", ssid, &required);
    if (ret != ESP_OK || ssid[0] == '\0') {
        nvs_close(handle);
        ssid[0] = '\0';
        return false;
    }
    required = password_len;
    ret = nvs_get_str(handle, "password", password, &required);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        password[0] = '\0';
        ret = ESP_OK;
    }
    nvs_close(handle);
    return ret == ESP_OK;
}

static esp_err_t save_wifi_credentials_to_nvs(const char *ssid, const char *password)
{
    if (ssid == NULL || ssid[0] == '\0') return ESP_ERR_INVALID_ARG;
    if (password == NULL) password = "";
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("network", NVS_READWRITE, &handle);
    if (ret != ESP_OK) return ret;
    ret = nvs_set_str(handle, "ssid", ssid);
    if (ret == ESP_OK) ret = nvs_set_str(handle, "password", password);
    if (ret == ESP_OK) ret = nvs_commit(handle);
    nvs_close(handle);
    return ret;
}

static esp_err_t wifi_sta_apply_and_connect(const char *ssid, const char *password)
{
    if (ssid == NULL || ssid[0] == '\0') return ESP_ERR_INVALID_ARG;
    if (password == NULL) password = "";
    wifi_config_t sta_config = {0};
    strncpy((char *)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid) - 1);
    strncpy((char *)sta_config.sta.password, password, sizeof(sta_config.sta.password) - 1);
    sta_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    sta_config.sta.pmf_cfg.capable = true;
    sta_config.sta.pmf_cfg.required = false;
    wifi_sta_connected = false;
    wifi_sta_got_ip = false;
    memset(wifi_sta_ssid, 0, sizeof(wifi_sta_ssid));
    strncpy(wifi_sta_ssid, ssid, sizeof(wifi_sta_ssid) - 1);
    wifi_sta_ip[0] = '\0';
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_disconnect());
    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
    if (ret == ESP_OK) ret = esp_wifi_connect();
    return ret;
}

static void wifi_reconnect_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(WIFI_RECONNECT_DELAY_MS));
    if (!wifi_sta_got_ip && saved_wifi_ssid[0] != '\0') {
        ESP_LOGI(TAG, "Retrying saved Wi-Fi STA connection to %s", saved_wifi_ssid);
        ESP_ERROR_CHECK_WITHOUT_ABORT(wifi_sta_apply_and_connect(saved_wifi_ssid, saved_wifi_password));
    }
    wifi_reconnect_task_pending = false;
    vTaskDelete(NULL);
}

static void schedule_wifi_reconnect(void)
{
    if (wifi_reconnect_task_pending || saved_wifi_ssid[0] == '\0') return;
    wifi_reconnect_task_pending = true;
    BaseType_t ok = xTaskCreate(wifi_reconnect_task, "wifi_reconnect", 4096, NULL, 3, NULL);
    if (ok != pdPASS) wifi_reconnect_task_pending = false;
}

static esp_err_t api_wifi_status_get_handler(httpd_req_t *req)
{
    char json[224];
    snprintf(json, sizeof(json),
             "{\"ok\":true,\"ap_ssid\":\"%s\",\"ap_ip\":\"%s\",\"sta_connected\":%s,\"sta_got_ip\":%s,\"sta_ssid\":\"%s\",\"sta_ip\":\"%s\",\"saved_ssid\":\"%s\"}",
             WIFI_AP_SSID,
             WIFI_AP_IP_STRING,
             wifi_sta_connected ? "true" : "false",
             wifi_sta_got_ip ? "true" : "false",
             wifi_sta_ssid,
             wifi_sta_ip,
             saved_wifi_ssid);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

static esp_err_t api_wifi_scan_get_handler(httpd_req_t *req)
{
    wifi_scan_config_t scan_config = {0};
    esp_err_t scan_ret = esp_wifi_scan_start(&scan_config, true);
    if (scan_ret != ESP_OK) {
        char err_json[96];
        snprintf(err_json, sizeof(err_json), "{\"ok\":false,\"error\":\"scan_start_failed:%s\"}", esp_err_to_name(scan_ret));
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store");
        httpd_resp_set_hdr(req, "Connection", "close");
        return httpd_resp_sendstr(req, err_json);
    }

    uint16_t ap_count = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_num(&ap_count));
    if (ap_count > WIFI_STA_MAX_SCAN_RESULTS) ap_count = WIFI_STA_MAX_SCAN_RESULTS;

    wifi_ap_record_t records[WIFI_STA_MAX_SCAN_RESULTS] = {0};
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records(&ap_count, records));

    char chunk[256];
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    snprintf(chunk, sizeof(chunk), "{\"ok\":true,\"aps\":[");
    esp_err_t ret = httpd_resp_send_chunk(req, chunk, strlen(chunk));
    if (ret != ESP_OK) return ret;

    for (uint16_t i = 0; i < ap_count; i++) {
        char ssid[33] = {0};
        memcpy(ssid, records[i].ssid, 32);

        char escaped[80] = {0};
        size_t epos = 0;
        for (const char *p = ssid; *p != '\0' && epos + 2 < sizeof(escaped); p++) {
            if (*p == '"' || *p == '\\') {
                if (epos + 3 >= sizeof(escaped)) break;
                escaped[epos++] = '\\';
                escaped[epos++] = *p;
            } else if ((unsigned char)*p >= 32) {
                escaped[epos++] = *p;
            }
        }
        escaped[epos] = '\0';

        snprintf(chunk, sizeof(chunk), "%s{\"ssid\":\"%s\",\"rssi\":%d,\"auth\":%d}",
                 i == 0 ? "" : ",",
                 escaped,
                 records[i].rssi,
                 records[i].authmode);
        ret = httpd_resp_send_chunk(req, chunk, strlen(chunk));
        if (ret != ESP_OK) return ret;
    }

    ret = httpd_resp_send_chunk(req, "]}", 2);
    if (ret != ESP_OK) return ret;
    return httpd_resp_send_chunk(req, NULL, 0);
}

static esp_err_t api_wifi_connect_get_handler(httpd_req_t *req)
{
    char query[192] = {0};
    char ssid[64] = {0};
    char password[96] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "ssid", ssid, sizeof(ssid)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected ssid query parameter");
    }
    (void)httpd_query_key_value(query, "password", password, sizeof(password));
    url_decode_in_place(ssid);
    url_decode_in_place(password);
    if (strlen(ssid) == 0 || strlen(ssid) > 32 || strlen(password) > 63) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid ssid or password length");
    }
    esp_err_t save_ret = save_wifi_credentials_to_nvs(ssid, password);
    if (save_ret == ESP_OK) {
        memset(saved_wifi_ssid, 0, sizeof(saved_wifi_ssid));
        memset(saved_wifi_password, 0, sizeof(saved_wifi_password));
        strncpy(saved_wifi_ssid, ssid, sizeof(saved_wifi_ssid) - 1);
        strncpy(saved_wifi_password, password, sizeof(saved_wifi_password) - 1);
    } else {
        ESP_LOGW(TAG, "Failed to save Wi-Fi credentials to NVS: %s", esp_err_to_name(save_ret));
    }
    esp_err_t ret = wifi_sta_apply_and_connect(ssid, password);
    char json[192];
    snprintf(json, sizeof(json), "{\"ok\":%s,\"status\":\"%s\",\"ssid\":\"%s\",\"saved\":%s}",
             ret == ESP_OK ? "true" : "false",
             ret == ESP_OK ? "connecting" : esp_err_to_name(ret),
             ssid,
             save_ret == ESP_OK ? "true" : "false");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

static bool parse_u8_decimal(const char *text, uint8_t *out)
{
    if (text == NULL || out == NULL || text[0] == '\0') return false;
    char *end = NULL;
    unsigned long value = strtoul(text, &end, 10);
    if (end == text || *end != '\0' || value > 255) return false;
    *out = (uint8_t)value;
    return true;
}

static uint32_t build_rvc_ext_id_from_dgn(uint32_t dgn, uint8_t source_address)
{
    const uint8_t priority = 6;
    uint32_t raw_dgn = dgn & 0x3FFFF;

    if (dgn <= 0xFFFF) {
        uint8_t pf = dgn & 0xFF;
        if (pf < 240) raw_dgn = ((uint32_t)pf << 8) | 0xFF;
    }

    return ((uint32_t)priority << 26) | ((raw_dgn & 0x3FFFF) << 8) | source_address;
}

static esp_err_t api_rvc_send_get_handler(httpd_req_t *req)
{
    char query[256] = {0};
    char dgn_text[24] = {0};
    char byte_text[8][8] = {{0}};
    uint8_t bytes[8] = {0};

    if (g_twai_node == NULL) return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "TWAI not ready");

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "dgn", dgn_text, sizeof(dgn_text)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected dgn and b0..b7 query parameters");
    }

    uint32_t dgn = (uint32_t)strtoul(dgn_text, NULL, 16);
    if (dgn > 0x3FFFF) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "DGN must be 0x00000 through 0x3FFFF");

    for (int i = 0; i < 8; i++) {
        char key[4];
        snprintf(key, sizeof(key), "b%d", i);
        if (httpd_query_key_value(query, key, byte_text[i], sizeof(byte_text[i])) != ESP_OK || !parse_u8_decimal(byte_text[i], &bytes[i])) {
            char err[96];
            snprintf(err, sizeof(err), "byte %d must be decimal 0 through 255", i);
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err);
        }
    }

    const uint8_t source_address = 0xF9;
    uint32_t can_id = build_rvc_ext_id_from_dgn(dgn, source_address);

    twai_frame_t tx_frame = {
        .header.id = can_id,
        .header.ide = true,
        .header.rtr = false,
        .buffer = bytes,
        .buffer_len = sizeof(bytes),
    };

    esp_err_t ret = twai_node_transmit(g_twai_node, &tx_frame, 1000);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "RV-C transmit failed: %s", esp_err_to_name(ret));
        char err[96];
        snprintf(err, sizeof(err), "transmit failed: %s", esp_err_to_name(ret));
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err);
    }

    ret = twai_node_transmit_wait_all_done(g_twai_node, 1000);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "RV-C transmit wait failed: %s", esp_err_to_name(ret));
        char err[96];
        snprintf(err, sizeof(err), "transmit wait failed: %s", esp_err_to_name(ret));
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err);
    }

    ESP_LOGI(TAG, "TX RV-C DGN 0x%05" PRIX32 " CAN ID 0x%08" PRIX32 " bytes %u %u %u %u %u %u %u %u",
             dgn, can_id, bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);

    char json[192];
    snprintf(json, sizeof(json), "{\"ok\":true,\"dgn\":\"0x%05" PRIX32 "\",\"can_id\":\"0x%08" PRIX32 "\",\"source_id\":%u}", dgn, can_id, (unsigned)source_address);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

static esp_err_t captive_redirect_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "http://" WIFI_AP_IP_STRING "/");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_send(req, NULL, 0);
}

static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.lru_purge_enable = true;
    config.uri_match_fn = httpd_uri_match_wildcard;

    // Wi-Fi stability / socket exhaustion protection.
    // accept(23) means the HTTP server ran out of socket/file descriptors.
    // Keep this server small, aggressively purge old sockets, and avoid long waits.
    config.max_uri_handlers = 24;
    config.max_open_sockets = 5;
    config.stack_size = 10240;
    config.recv_wait_timeout = 5;
    config.send_wait_timeout = 5;
    config.backlog_conn = 2;

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return NULL;
    }

    web_server = server;

    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_state_uri = {
        .uri = "/api/state",
        .method = HTTP_GET,
        .handler = api_state_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_enable_uri = {
        .uri = "/api/enable",
        .method = HTTP_GET,
        .handler = api_enable_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_deselect_all_uri = {
        .uri = "/api/deselect_all",
        .method = HTTP_GET,
        .handler = api_deselect_all_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_select_all_uri = {
        .uri = "/api/select_all",
        .method = HTTP_GET,
        .handler = api_select_all_get_handler,
        .user_ctx = NULL,
    };


    httpd_uri_t api_wifi_status_uri = {
        .uri = "/api/wifi/status",
        .method = HTTP_GET,
        .handler = api_wifi_status_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_wifi_scan_uri = {
        .uri = "/api/wifi/scan",
        .method = HTTP_GET,
        .handler = api_wifi_scan_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_wifi_connect_uri = {
        .uri = "/api/wifi/connect",
        .method = HTTP_GET,
        .handler = api_wifi_connect_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t api_rvc_send_uri = {
        .uri = "/api/rvc/send",
        .method = HTTP_GET,
        .handler = api_rvc_send_get_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t websocket_uri = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = websocket_handler,
        .user_ctx = NULL,
        .is_websocket = true,
    };

    httpd_uri_t captive_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = captive_redirect_handler,
        .user_ctx = NULL,
    };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_state_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_enable_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_deselect_all_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_select_all_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_status_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_scan_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_connect_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvc_send_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &websocket_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &captive_uri));

    ESP_LOGI(TAG, "HTTP server started at http://%s/", WIFI_AP_IP_STRING);
    return server;
}

static int build_dns_response(const uint8_t *query, int query_len, uint8_t *response, int response_max_len)
{
    if (query_len < 12 || response_max_len < query_len + 16) {
        return 0;
    }

    memcpy(response, query, query_len);

    response[2] = 0x81;
    response[3] = 0x80;
    response[6] = 0x00;
    response[7] = 0x01;
    response[8] = 0x00;
    response[9] = 0x00;
    response[10] = 0x00;
    response[11] = 0x00;

    int offset = query_len;

    response[offset++] = 0xC0;
    response[offset++] = 0x0C;
    response[offset++] = 0x00;
    response[offset++] = 0x01;
    response[offset++] = 0x00;
    response[offset++] = 0x01;
    response[offset++] = 0x00;
    response[offset++] = 0x00;
    response[offset++] = 0x00;
    response[offset++] = 0x00;
    response[offset++] = 0x00;
    response[offset++] = 0x04;
    response[offset++] = 192;
    response[offset++] = 168;
    response[offset++] = 4;
    response[offset++] = 1;

    return offset;
}

static void dns_server_task(void *arg)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "DNS socket create failed");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(DNS_PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "DNS socket bind failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Captive DNS server started on port %d", DNS_PORT);

    uint8_t query[512];
    uint8_t response[512];

    while (1) {
        struct sockaddr_in client_addr = {0};
        socklen_t client_len = sizeof(client_addr);

        int len = recvfrom(sock, query, sizeof(query), 0, (struct sockaddr *)&client_addr, &client_len);
        if (len <= 0) {
            continue;
        }

        int response_len = build_dns_response(query, len, response, sizeof(response));
        if (response_len > 0) {
            sendto(sock, response, response_len, 0, (struct sockaddr *)&client_addr, client_len);
        }
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi AP client connected, AID=%d", event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi AP client disconnected, AID=%d", event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi STA started");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;
        wifi_sta_connected = true;
        wifi_sta_got_ip = false;
        memset(wifi_sta_ssid, 0, sizeof(wifi_sta_ssid));
        memcpy(wifi_sta_ssid, event->ssid, event->ssid_len < sizeof(wifi_sta_ssid) - 1 ? event->ssid_len : sizeof(wifi_sta_ssid) - 1);
        ESP_LOGI(TAG, "Wi-Fi STA connected to %s", wifi_sta_ssid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
        wifi_sta_connected = false;
        wifi_sta_got_ip = false;
        wifi_sta_ip[0] = '\0';
        ESP_LOGW(TAG, "Wi-Fi STA disconnected, reason=%d", event->reason);
        schedule_wifi_reconnect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        wifi_sta_connected = true;
        wifi_sta_got_ip = true;
        snprintf(wifi_sta_ip, sizeof(wifi_sta_ip), IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Wi-Fi STA got IP: %s", wifi_sta_ip);
    }
}

static void wifi_ap_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(ret);
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    if (ap_netif == NULL) {
        ESP_LOGE(TAG, "Failed to create default Wi-Fi AP netif");
        return;
    }

    sta_netif = esp_netif_create_default_wifi_sta();
    if (sta_netif == NULL) {
        ESP_LOGE(TAG, "Failed to create default Wi-Fi STA netif");
        return;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.ap.ssid, WIFI_AP_SSID, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(WIFI_AP_SSID);
    wifi_config.ap.channel = WIFI_AP_CHANNEL;
    wifi_config.ap.max_connection = WIFI_AP_MAX_CLIENTS;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    wifi_config.ap.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (load_wifi_credentials_from_nvs(saved_wifi_ssid, sizeof(saved_wifi_ssid), saved_wifi_password, sizeof(saved_wifi_password))) {
        ESP_LOGI(TAG, "Loaded saved Wi-Fi STA credentials for %s", saved_wifi_ssid);
        ESP_ERROR_CHECK_WITHOUT_ABORT(wifi_sta_apply_and_connect(saved_wifi_ssid, saved_wifi_password));
    } else {
        ESP_LOGI(TAG, "No saved Wi-Fi STA credentials in NVS");
    }

    // Disable modem sleep. This improves AP responsiveness while serving the web UI.
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGI(TAG, "Wi-Fi AP+STA started");
    ESP_LOGI(TAG, "SSID: %s", WIFI_AP_SSID);
    ESP_LOGI(TAG, "Security: open");
    ESP_LOGI(TAG, "DHCP server: enabled by default on AP netif");
    ESP_LOGI(TAG, "AP IP: %s", WIFI_AP_IP_STRING);

    spiffs_init();
    start_webserver();

    xTaskCreate(
        dns_server_task,
        "dns_server_task",
        DNS_TASK_STACK,
        NULL,
        4,
        NULL
    );
}

static active_dgn_state_t *get_unknown_active_state(uint32_t dgn)
{
    active_dgn_state_t *empty = NULL;
    for (size_t i = 0; i < MAX_UNKNOWN_ACTIVE_DGNS; i++) {
        if (unknown_active_dgns[i].seen && unknown_active_dgns[i].dgn == dgn) return &unknown_active_dgns[i];
        if (!unknown_active_dgns[i].seen && empty == NULL) empty = &unknown_active_dgns[i];
    }
    if (empty != NULL) {
        memset(empty, 0, sizeof(*empty));
        empty->seen = true;
        empty->dgn = dgn;
    }
    return empty;
}

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

static const watched_dgn_t *find_watched_dgn(uint32_t dgn)
{
    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        if (WATCHED_DGNS[i].enabled && WATCHED_DGNS[i].dgn == dgn) {
            return &WATCHED_DGNS[i];
        }
    }
    return NULL;
}

static const watched_dgn_t *find_watched_dgn_any(uint32_t dgn)
{
    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        if (WATCHED_DGNS[i].dgn == dgn) {
            return &WATCHED_DGNS[i];
        }
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

static const dgn_desc_t *find_dgn_desc(uint32_t dgn)
{
    for (size_t i = 0; i < RVC_DGN_DESC_COUNT; i++) {
        if (RVC_DGN_DESCS[i].dgn == dgn) {
            return &RVC_DGN_DESCS[i];
        }
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

    // PDU1 / destination-specific frames appear as PF + destination in raw bits.
    // The RV-C reference lists these as shorter DGNs such as 0x0EA.
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

static uint64_t extract_field_value(const uint8_t *data, uint8_t len, const field_desc_t *field)
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

static const char *lookup_value_name(const field_desc_t *field, uint64_t value)
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

static int16_t extract_instance(const dgn_desc_t *desc, const uint8_t *data, uint8_t len)
{
    if (desc == NULL) {
        return -1;
    }

    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];

        if (field->name != NULL && strcasecmp(field->name, "instance") == 0) {
            return (int16_t)extract_field_value(data, len, field);
        }
    }

    return -1;
}

static tracked_state_t *get_tracked_state(uint32_t dgn, int16_t instance, uint8_t source_id)
{
    tracked_state_t *empty = NULL;

    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        if (tracked_states[i].valid) {
            if (tracked_states[i].dgn == dgn && tracked_states[i].instance == instance && tracked_states[i].source_id == source_id) {
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

static bool any_decoded_field_changed(const dgn_desc_t *desc, const rvc_rx_frame_t *frame, const tracked_state_t *previous)
{
    if (desc == NULL) return false;

    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];
        if (field_changed(previous, frame, field)) {
            return true;
        }
    }
    return false;
}

static bool frame_changed(tracked_state_t *state, const rvc_rx_frame_t *frame)
{
    if (state == NULL) {
        return true;
    }

    if (state->len != frame->len) {
        return true;
    }

    return memcmp(state->data, frame->data, frame->len) != 0;
}

static void update_tracked_state(tracked_state_t *state, const rvc_rx_frame_t *frame)
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

static bool field_changed(const tracked_state_t *previous, const rvc_rx_frame_t *current, const field_desc_t *field)
{
    if (previous == NULL || previous->len == 0) {
        return true;
    }

    uint64_t old_value = extract_field_value(previous->data, previous->len, field);
    uint64_t new_value = extract_field_value(current->data, current->len, field);

    return old_value != new_value;
}

static void print_ascii_field(const uint8_t *data, uint8_t len, const field_desc_t *field)
{
    printf("\"");

    for (uint8_t i = field->byte_start; i <= field->byte_end && i < len && i < 8; i++) {
        char c = (char)data[i];
        if (c >= 32 && c <= 126) {
            printf("%c", c);
        } else {
            printf(".");
        }
    }

    printf("\"");
}

static bool print_decoded_change(const dgn_desc_t *desc, const rvc_rx_frame_t *frame, const tracked_state_t *previous)
{
    bool any_field_changed = false;

    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];

        if (field_changed(previous, frame, field)) {
            any_field_changed = true;
            break;
        }
    }

    if (!any_field_changed) {
        return false;
    }

    int16_t instance = extract_instance(desc, frame->data, frame->len);

    printf("\n--- RV-C FIELD CHANGE ---\n");
    printf("CAN ID:   0x%08" PRIX32 "\n", frame->id);
    printf("DGN:      0x%05" PRIX32 "  %s\n", desc->dgn, desc->name);
    printf("Instance: ");

    if (instance >= 0) {
        printf("%d\n", instance);
    } else {
        printf("n/a\n");
    }

    printf("Changed decoded fields only:\n");

    for (uint16_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *field = &desc->fields[i];

        if (!field_changed(previous, frame, field)) {
            continue;
        }

        printf("  - %s: ", field->name != NULL ? field->name : "value");

        if (field->type == FIELD_ASCII) {
            print_ascii_field(frame->data, frame->len, field);
        } else {
            uint64_t value = extract_field_value(frame->data, frame->len, field);
            char decoded_text[48];
            format_decoded_number(decoded_text, sizeof(decoded_text), convert_decoded_value(field, value));
            printf("%s", decoded_text);

            const char *value_name = lookup_value_name(field, value);
            if (value_name != NULL) {
                printf(" = %s", value_name);
            }

            if (field->unit != NULL && strlen(field->unit) > 0) {
                printf(" %s", field->unit);
            }
        }

        printf("\n");
    }

    printf("--------------------------\n");
    return true;
}

static bool IRAM_ATTR twai_rx_done_callback(
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

    if (rx_queue != NULL) {
        if (xQueueSendFromISR(rx_queue, &out, &high_task_wakeup) != pdTRUE) {
            diag_rx_dropped_isr++;
        }
    }

    return high_task_wakeup == pdTRUE;
}

static void rvc_print_task(void *arg)
{
    rvc_rx_frame_t frame;
    uint32_t processed_since_yield = 0;

    while (1) {
        if (xQueueReceive(rx_queue, &frame, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        int64_t process_start_us = esp_timer_get_time();
        diag_rx_frames++;

        if (!frame.is_extended || frame.is_rtr) {
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        uint32_t raw_dgn = get_raw_dgn_from_ext_id(frame.id);
        int64_t now_us = process_start_us;

        if (state_mutex != NULL) xSemaphoreTake(state_mutex, portMAX_DELAY);

        bool discovery_mode = select_all_mode;

        if (discovery_mode) {
            uint32_t dgn = normalize_dgn_any(raw_dgn);
            const watched_dgn_t *watched_any = find_watched_dgn_any(dgn);

            if (watched_any != NULL) {
                size_t idx = (size_t)(watched_any - WATCHED_DGNS);
                active_dgn_state_t *active = &active_dgns[idx];
                active->seen = true;
                active->dgn = dgn;
                active->name = watched_any->name;
                active->count++;
                active->has_source_id = true;
                active->source_id = (uint8_t)(frame.id & 0xFF);
                active->has_data = true;
                active->len = frame.len > 8 ? 8 : frame.len;
                memcpy(active->data, frame.data, active->len);

                if ((now_us - active->last_sent_us) >= DISCOVERY_EVENT_MIN_US) {
                    active->last_sent_us = now_us;
#if ENABLE_WS_PUSH_EVENTS
                    ws_enqueue_activity_event(dgn, watched_any->name, active->count, active->has_source_id, active->source_id);
#endif
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

            if (state_mutex != NULL) xSemaphoreGive(state_mutex);
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        uint32_t dgn = get_dgn_from_ext_id(frame.id);

        const watched_dgn_t *watched = find_watched_dgn(dgn);
        if (watched == NULL) {
            if (state_mutex != NULL) xSemaphoreGive(state_mutex);
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        const dgn_desc_t *desc = find_dgn_desc(dgn);
        if (desc == NULL) {
            if (state_mutex != NULL) xSemaphoreGive(state_mutex);
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        int16_t instance = extract_instance(desc, frame.data, frame.len);
        uint8_t source_id = (uint8_t)(frame.id & 0xFF);
        tracked_state_t *state = get_tracked_state(dgn, instance, source_id);

        if (state == NULL) {
            if (state_mutex != NULL) xSemaphoreGive(state_mutex);
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        if (!state->has_snapshot) {
            // Baseline frame: store initial field values. /api/state polling reports them.
#if ENABLE_WS_PUSH_EVENTS
            for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                ws_enqueue_field_event(dgn, desc->name, instance, &frame, &desc->fields[i], false, global_change_seq);
            }
#endif
            update_tracked_state(state, &frame);
            last_valid_rx_time_us = now_us;
            if (state_mutex != NULL) xSemaphoreGive(state_mutex);
            diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);
            continue;
        }

        if (frame_changed(state, &frame)) {
            bool changed = any_decoded_field_changed(desc, &frame, state);

            if (changed) {
                global_change_seq++;
                state->dgn_change_seq = global_change_seq;

                for (uint16_t i = 0; i < desc->field_count && i < 32; i++) {
                    const field_desc_t *field = &desc->fields[i];
                    if (field_changed(state, &frame, field)) {
                        state->field_change_seq[i] = global_change_seq;
#if ENABLE_WS_PUSH_EVENTS
                        ws_enqueue_field_event(dgn, desc->name, instance, &frame, field, true, global_change_seq);
#endif
                    }
                }
#if ENABLE_SERIAL_CHANGE_LOG
                print_decoded_change(desc, &frame, state);
#endif
                last_valid_rx_time_us = now_us;
            }

            update_tracked_state(state, &frame);
        }

        if (state_mutex != NULL) xSemaphoreGive(state_mutex);

        diag_rx_busy_us += (uint64_t)(esp_timer_get_time() - process_start_us);

        processed_since_yield++;
        if (processed_since_yield >= 32) {
            processed_since_yield = 0;
            taskYIELD();
        }
    }
}

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

void app_main(void)
{
    printf("\nStarting RV-C change sniffer with web dashboard\n");
    printf("Watching %u RV-C DGNs from rvc-spec.yml\n", (unsigned)WATCHED_DGN_COUNT);

    state_mutex = xSemaphoreCreateMutex();
    if (state_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create state mutex");
        return;
    }

    ws_client_mutex = xSemaphoreCreateMutex();
    if (ws_client_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket client mutex");
        return;
    }

    ws_event_queue = xQueueCreate(WS_EVENT_QUEUE_LENGTH, sizeof(ws_event_t));
    if (ws_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket event queue");
        return;
    }

    wifi_ap_init();
    led_init();

    rx_queue = xQueueCreate(RX_QUEUE_LENGTH, sizeof(rvc_rx_frame_t));
    if (rx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create RX queue");
        return;
    }

    xTaskCreate(
        rvc_print_task,
        "rvc_print_task",
        8192,
        NULL,
        5,
        NULL
    );

    xTaskCreate(
        led_heartbeat_task,
        "led_heartbeat_task",
        2048,
        NULL,
        4,
        NULL
    );

    xTaskCreate(
        ws_delta_task,
        "ws_delta_task",
        6144,
        NULL,
        4,
        NULL
    );

    twai_node_handle_t twai_node = NULL;

    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = CAN_TX_GPIO,
        .io_cfg.rx = CAN_RX_GPIO,
        .bit_timing.bitrate = 250000,
        .tx_queue_depth = 8,
        .flags.enable_listen_only = false,
    };

    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &twai_node));
    g_twai_node = twai_node;

    twai_event_callbacks_t callbacks = {
        .on_rx_done = twai_rx_done_callback,
    };

    ESP_ERROR_CHECK(twai_node_register_event_callbacks(twai_node, &callbacks, NULL));
    ESP_ERROR_CHECK(twai_node_enable(twai_node));

    ESP_LOGI(TAG, "TWAI node enabled");
    ESP_LOGI(TAG, "Mode: normal RX/TX");
    ESP_LOGI(TAG, "Bitrate: 250 kbps");
    ESP_LOGI(TAG, "LED GPIO: %d", LED_GPIO);
    ESP_LOGI(TAG, "All watched DGNs default disabled");
    ESP_LOGI(TAG, "Reporting individual decoded field changes only");
    ESP_LOGI(TAG, "WebSocket mode: true push-on-change delta stream");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
