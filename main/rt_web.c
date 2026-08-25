// Stand-alone RV-C Tools web stack. Ported from hub_core's lc_web.c at HEAD
// (2026-08-24): the handler bodies are the hub's, verbatim where possible, so
// behavior the hub already debugged (scan-vs-reconnect races, the ETag/304
// path, the closed-loop transmit confirm, the WS drop-oldest queue) carries
// over. Differences from the hub, all deliberate:
//   - no lc_auth / Dashboard PIN — every URI registers directly
//   - "trusted network" concept removed (it only existed to bypass the PIN)
//   - AP password lives in this file's own tiny NVS store ("netcfg"/"ap_pw"),
//     served through the same /api/auth/status + /api/auth/ap_password shapes
//     so the ported network page JS is unchanged
//   - no /control-ws, no publish config, no batch sender (nothing here calls
//     them; the tools page uses GET /api/rvc/send only)

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
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
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_http_server.h"
#include "esp_spiffs.h"
#include "mdns.h"
#include "lwip/sockets.h"

#include "rvc_spec_generated.h"
#include "lc_config.h"
#include "lc_types.h"
#include "lc_can.h"
#include "lc_state.h"
#include "lc_rvclog.h"
#include "rt_web.h"

/* Defined in main.c alongside the watched-DGN table. */
extern const watched_dgn_t WATCHED_DGNS[];
extern const size_t WATCHED_DGN_COUNT;
bool watched_dgn_is_enabled(size_t idx);
void watched_dgn_set_enabled(size_t idx, bool enabled);
extern int64_t diag_window_start_us;
extern uint32_t diag_last_rx_frames;
extern uint64_t diag_last_rx_busy_us;

const dgn_desc_t *find_dgn_desc(uint32_t dgn);
uint64_t extract_field_value(const uint8_t *data, uint8_t len, const field_desc_t *field);
const char *lookup_value_name(const field_desc_t *field, uint64_t value);
void format_decoded_number(char *out, size_t out_len, double value);
double convert_decoded_value(const field_desc_t *field, uint64_t raw_value);
void format_raw_field_value(char *out, size_t out_len, const uint8_t *data, uint8_t len, const field_desc_t *field);
const char *field_type_to_string(field_type_t type);

// ── Module state ───────────────────────────────────────────────────────────

static QueueHandle_t ws_event_queue = NULL;
static httpd_handle_t web_server = NULL;
// Push-path delivery counters, surfaced in /api/state's diag block. Added to
// diagnose "decoded values only update at the poll cadence": ws_sent counts
// frames actually handed to a /ws client; ws_noclient counts events that
// arrived at the pump with no client registered. A toggle that bumps
// ws_noclient (or neither) while the page says "connected" localizes the
// fault to the socket registration; ws_sent climbing while the UI still lags
// localizes it to the browser side.
static volatile uint32_t s_ws_sent = 0;
static volatile uint32_t s_ws_noclient = 0;
static int ws_client_fds[MAX_WS_CLIENTS] = {-1, -1};
static SemaphoreHandle_t ws_client_mutex = NULL;
static esp_netif_t *sta_netif = NULL;
static volatile bool wifi_sta_connected = false;
static volatile bool wifi_sta_got_ip = false;
static char wifi_sta_ssid[33] = {0};
static char wifi_sta_ip[16] = {0};
static volatile bool wifi_reconnect_task_pending = false;
static volatile bool wifi_roam_task_pending = false;
static volatile bool wifi_scan_in_progress = false;
static uint32_t wifi_sta_retry_attempts = 0;
static esp_timer_handle_t wifi_roam_timer = NULL;

typedef struct {
    char ssid[33];
    char password[65];
} wifi_saved_network_t;

typedef struct {
    uint32_t version;
    uint8_t  count;
    wifi_saved_network_t networks[WIFI_MAX_SAVED_NETWORKS];
} wifi_saved_networks_store_t;

// v1 here — this project's store never carried the hub's `trusted` byte, and
// the NVS namespace is the same "network"/"net_list" the hub used, so a board
// moving between firmwares starts its list fresh (version mismatch → empty).
#define WIFI_SAVED_NETWORKS_VERSION 1
#define WIFI_SAVED_NETWORKS_NVS_NS  "network"
#define WIFI_SAVED_NETWORKS_NVS_KEY "net_list"

// The device's identity, built once at AP bring-up and then constant.
static char s_ap_ssid[WIFI_AP_SSID_MAX] = WIFI_AP_SSID_PREFIX;
static char s_dev_id[5]                 = "";

static wifi_saved_networks_store_t saved_networks = {0};
static wifi_ap_record_t last_scan_records[WIFI_STA_MAX_SCAN_RESULTS];
static uint16_t         last_scan_count = 0;

// ── AP password (the one persisted "network setting" beyond the STA list) ──
// Minimal replacement for the hub's lc_auth AP-password half: a plain NVS
// string, loaded at boot. No PIN, no sessions, no recovery window — this is
// a bench diagnostic tool with a USB console; a forgotten password is cleared
// from the STA side or by erasing NVS over USB.
#define NETCFG_NVS_NS  "netcfg"
#define NETCFG_KEY_APPW "ap_pw"
static char s_ap_password[65] = "";

static void load_ap_password(void)
{
    s_ap_password[0] = '\0';
    nvs_handle_t h;
    if (nvs_open(NETCFG_NVS_NS, NVS_READONLY, &h) != ESP_OK) return;
    size_t sz = sizeof(s_ap_password);
    if (nvs_get_str(h, NETCFG_KEY_APPW, s_ap_password, &sz) != ESP_OK) {
        s_ap_password[0] = '\0';
    }
    nvs_close(h);
}

static esp_err_t save_ap_password(const char *pw)
{
    nvs_handle_t h;
    esp_err_t ret = nvs_open(NETCFG_NVS_NS, NVS_READWRITE, &h);
    if (ret != ESP_OK) return ret;
    ret = nvs_set_str(h, NETCFG_KEY_APPW, pw != NULL ? pw : "");
    if (ret == ESP_OK) ret = nvs_commit(h);
    nvs_close(h);
    if (ret == ESP_OK) {
        strlcpy(s_ap_password, pw != NULL ? pw : "", sizeof(s_ap_password));
    }
    return ret;
}

/* One place decides whether the AP is open or WPA2. */
static void fill_ap_security(wifi_config_t *cfg)
{
    if (s_ap_password[0] != '\0') {
        strncpy((char *)cfg->ap.password, s_ap_password, sizeof(cfg->ap.password) - 1);
        cfg->ap.authmode = WIFI_AUTH_WPA2_PSK;
    } else {
        cfg->ap.password[0] = '\0';
        cfg->ap.authmode = WIFI_AUTH_OPEN;
    }
}

/* ⚠ Re-applying the AP config RESTARTS the access point: anything connected
 * to it is dropped and has to rejoin — WPA2 is negotiated at association, so
 * this is unavoidable. The network page warns before calling this. */
static esp_err_t reapply_ap_config(void)
{
    wifi_config_t cfg = {0};
    if (esp_wifi_get_config(WIFI_IF_AP, &cfg) != ESP_OK) return ESP_FAIL;
    fill_ap_security(&cfg);
    return esp_wifi_set_config(WIFI_IF_AP, &cfg);
}

static void build_ap_ssid(void)
{
    uint8_t mac[6] = {0};
    if (esp_wifi_get_mac(WIFI_IF_AP, mac) != ESP_OK) {
        ESP_LOGW(TAG, "Could not read the AP MAC; SSID has no id suffix");
        return;
    }
    snprintf(s_dev_id, sizeof(s_dev_id), "%02X%02X", mac[4], mac[5]);
    snprintf(s_ap_ssid, sizeof(s_ap_ssid), "%s-%s", WIFI_AP_SSID_PREFIX, s_dev_id);
    ESP_LOGI(TAG, "Device id %s — AP SSID \"%s\"", s_dev_id, s_ap_ssid);
}

// ── SPIFFS + static assets ─────────────────────────────────────────────────

// Shared validator for every gzipped web asset, read once at mount.
// webui-version.json's `version` comes from version.txt, which build_web.py
// bumps on EVERY asset build — so this string changes exactly when the SPIFFS
// image changes, which is what an ETag needs to promise.
// ⚠ Static on purpose: httpd does not copy header VALUES, and httpd is
// single-threaded so one buffer is safe.
static char s_asset_etag[48];

static void load_asset_etag(void)
{
    FILE *f = fopen("/spiffs/webui-version.json", "r");
    if (f == NULL) return;
    char buf[192];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    buf[n] = '\0';

    const char *k = strstr(buf, "\"version\"");
    if (k == NULL) return;
    const char *q1 = strchr(k + strlen("\"version\""), '"');
    if (q1 == NULL) return;
    const char *q2 = strchr(q1 + 1, '"');
    if (q2 == NULL) return;
    size_t len = (size_t)(q2 - q1 - 1);
    if (len == 0 || len + 3 > sizeof(s_asset_etag)) return;

    s_asset_etag[0] = '"';
    memcpy(s_asset_etag + 1, q1 + 1, len);
    s_asset_etag[1 + len] = '"';
    s_asset_etag[2 + len] = '\0';
    ESP_LOGI(TAG, "Web asset ETag: %s", s_asset_etag);
}

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

    load_asset_etag();
}

static esp_err_t send_file_from_spiffs_ex(httpd_req_t *req, const char *path, const char *content_type, bool cacheable)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Could not open file: %s", path);
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    }

    httpd_resp_set_type(req, content_type);
    httpd_resp_set_hdr(req, "Cache-Control",
                       cacheable ? "public, max-age=31536000, immutable" : "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    char chunk[2048];
    size_t read_bytes;
    esp_err_t ret = ESP_OK;
    while ((read_bytes = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        ret = httpd_resp_send_chunk(req, chunk, read_bytes);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "send_file: chunk send failed for %s (err %d)", path, ret);
            fclose(file);
            return ret;
        }
    }

    if (ferror(file)) {
        ESP_LOGE(TAG, "send_file: read error for %s", path);
        fclose(file);
        return ESP_FAIL;
    }

    fclose(file);
    return httpd_resp_sendstr_chunk(req, NULL);
}

static esp_err_t send_gz_file_from_spiffs(httpd_req_t *req, const char *path, const char *content_type)
{
    // HTML pages are the mutable entry points at stable URLs → no-cache with
    // an ETag (a 304 is a few bytes and touches no flash); CSS carries a
    // ?v=<build> query from the pages, so it can be immutable.
    bool is_html = (strcmp(content_type, "text/html") == 0);
    const char *cache_hdr = is_html ? "no-cache"
                                    : "public, max-age=31536000, immutable";

    if (s_asset_etag[0] != '\0') {
        char inm[96];
        if (httpd_req_get_hdr_value_str(req, "If-None-Match", inm, sizeof(inm)) == ESP_OK &&
            strstr(inm, s_asset_etag) != NULL) {
            httpd_resp_set_status(req, "304 Not Modified");
            httpd_resp_set_hdr(req, "ETag", s_asset_etag);
            httpd_resp_set_hdr(req, "Cache-Control", cache_hdr);
            httpd_resp_set_hdr(req, "Connection", "close");
            return httpd_resp_send(req, NULL, 0);
        }
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Could not open file: %s", path);
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    }
    httpd_resp_set_type(req, content_type);
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_set_hdr(req, "Cache-Control", cache_hdr);
    if (s_asset_etag[0] != '\0') httpd_resp_set_hdr(req, "ETag", s_asset_etag);
    httpd_resp_set_hdr(req, "Connection", "close");
    char chunk[2048];
    size_t read_bytes;
    esp_err_t ret = ESP_OK;
    while ((read_bytes = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        ret = httpd_resp_send_chunk(req, chunk, read_bytes);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "send_gz_file: chunk send failed for %s (err %d)", path, ret);
            fclose(file);
            return ret;
        }
    }
    if (ferror(file)) {
        ESP_LOGE(TAG, "send_gz_file: read error for %s", path);
        fclose(file);
        return ESP_FAIL;
    }
    fclose(file);
    return httpd_resp_sendstr_chunk(req, NULL);
}

// RV-C Tools IS the home page; /rvc-tools.html stays registered too because
// the page's own log-mode links navigate to it by name.
static esp_err_t root_get_handler(httpd_req_t *req)
{
    return send_gz_file_from_spiffs(req, "/spiffs/rvc-tools.html.gz", "text/html");
}

static esp_err_t network_get_handler(httpd_req_t *req)
{
    return send_gz_file_from_spiffs(req, "/spiffs/network.html.gz", "text/html");
}

static esp_err_t common_css_get_handler(httpd_req_t *req)
{
    return send_gz_file_from_spiffs(req, "/spiffs/common.css.gz", "text/css");
}

static esp_err_t inter_font_get_handler(httpd_req_t *req)
{
    return send_file_from_spiffs_ex(req, "/spiffs/inter.woff2", "font/woff2", true);
}

static esp_err_t webui_version_get_handler(httpd_req_t *req)
{
    return send_file_from_spiffs_ex(req, "/spiffs/webui-version.json", "application/json", false);
}

// ── JSON helpers ───────────────────────────────────────────────────────────

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
            if (c == '\"' || c == '\\') json_append(buf, max, pos, "\\%c", c);
            else if (c == '\n') json_append(buf, max, pos, "\\n");
            else if (c == '\r') json_append(buf, max, pos, "\\r");
            else if (c == '\t') json_append(buf, max, pos, "\\t");
            else if ((unsigned char)c < 32) json_append(buf, max, pos, " ");
            else json_append(buf, max, pos, "%c", c);
        }
    }
    json_append(buf, max, pos, "\"");
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

// Same formatter the /api/state serializer and the WS field events share.
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

static const char *reset_reason_str(void)
{
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:    return "Power-on";
        case ESP_RST_EXT:        return "External reset pin";
        case ESP_RST_SW:         return "Software restart";
        case ESP_RST_PANIC:      return "Crash (panic)";
        case ESP_RST_INT_WDT:    return "Interrupt watchdog";
        case ESP_RST_TASK_WDT:   return "Task watchdog";
        case ESP_RST_WDT:        return "Watchdog (other)";
        case ESP_RST_DEEPSLEEP:  return "Deep-sleep wake";
        case ESP_RST_BROWNOUT:   return "Brownout";
        default:                 return "Unknown";
    }
}

// ── /api/state — the sniffer page's polling snapshot ──────────────────────

static esp_err_t api_state_get_handler(httpd_req_t *req)
{
    char chunk[1536];
    size_t pos = 0;
    esp_err_t ret = ESP_OK;

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    int64_t now_us = esp_timer_get_time();
    uint32_t free_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    uint32_t min_free_heap = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);

    nvs_stats_t nvs_stats = {0};
    esp_err_t nvs_stats_err = nvs_get_stats(NULL, &nvs_stats);
    UBaseType_t rx_waiting = lc_can_rx_queue != NULL ? uxQueueMessagesWaiting(lc_can_rx_queue) : 0;
    UBaseType_t ws_waiting = ws_event_queue != NULL ? uxQueueMessagesWaiting(ws_event_queue) : 0;
    uint32_t snap_global_seq, snap_rx_frames, snap_rx_dropped;
    bool snap_select_all;
    int64_t window_us;
    uint32_t rx_frames_delta;
    uint64_t rx_busy_delta;

    lc_state_lock();
    if (diag_window_start_us == 0) diag_window_start_us = now_us;
    window_us = now_us - diag_window_start_us;
    rx_frames_delta = diag_rx_frames - diag_last_rx_frames;
    rx_busy_delta = diag_rx_busy_us - diag_last_rx_busy_us;
    diag_window_start_us = now_us;
    diag_last_rx_frames = diag_rx_frames;
    diag_last_rx_busy_us = diag_rx_busy_us;
    snap_global_seq = global_change_seq;
    snap_select_all = lc_state_is_select_all_mode();
    snap_rx_frames = diag_rx_frames;
    snap_rx_dropped = diag_rx_dropped_isr;
    lc_state_unlock();

    // ⚠ Takes lc_state_lock() itself, so it must sit OUTSIDE the block above.
    size_t tracked_used = lc_state_tracked_used();

    uint32_t rx_task_load_pct = 0;
    uint32_t rx_fps = 0;
    if (window_us > 0) {
        uint64_t load = (rx_busy_delta * 100ULL) / (uint64_t)window_us;
        rx_task_load_pct = load > 100 ? 100 : (uint32_t)load;
        rx_fps = (uint32_t)(((uint64_t)rx_frames_delta * 1000000ULL) / (uint64_t)window_us);
    }

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                             "{\"ok\":true,\"seq\":%" PRIu32 ",\"mode\":\"%s\",\"now_us\":%" PRId64 ",",
                             snap_global_seq,
                             snap_select_all ? "discovery" : "selected",
                             now_us);
    if (ret != ESP_OK) return ret;

    // free_psram stays in the JSON (0 — this board has none) so the page's
    // diag consumer needs no fork from the hub's.
    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                             "\"diag\":{\"free_heap\":%u,\"min_free_heap\":%u,\"free_psram\":0,\"rx_queue\":%u,\"ws_queue\":%u,\"ws_sent\":%u,\"ws_noclient\":%u,\"rx_frames\":%u,\"rx_fps\":%u,\"rx_dropped\":%u,\"rx_task_load_pct\":%u,\"reset_reason\":\"%s\",\"coredump_present\":false,\"tracked_used\":%u,\"tracked_max\":%u,\"tracked_full\":%u,",
                             free_heap, min_free_heap, (unsigned)rx_waiting, (unsigned)ws_waiting,
                             (unsigned)s_ws_sent, (unsigned)s_ws_noclient,
                             (unsigned)snap_rx_frames, rx_fps, (unsigned)snap_rx_dropped, rx_task_load_pct,
                             reset_reason_str(),
                             (unsigned)tracked_used, (unsigned)MAX_TRACKED_STATES,
                             (unsigned)lc_state_tracked_full_events());
    if (ret != ESP_OK) return ret;
    if (nvs_stats_err == ESP_OK) {
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "\"nvs_used_entries\":%u,\"nvs_available_entries\":%u,\"nvs_total_entries\":%u},",
                                 (unsigned)nvs_stats.used_entries, (unsigned)nvs_stats.available_entries,
                                 (unsigned)nvs_stats.total_entries);
    } else {
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "\"nvs_used_entries\":null,\"nvs_available_entries\":null,\"nvs_total_entries\":null},");
    }
    if (ret != ESP_OK) return ret;

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"active\":[");
    if (ret != ESP_OK) return ret;

    // Per-entry copy under brief lock — never held across the send.
    bool first = true;
    for (size_t i = 0; i < MAX_ACTIVE_DGNS; i++) {
        active_dgn_state_t local_adgn;
        lc_state_lock();
        local_adgn = active_dgns[i];
        lc_state_unlock();

        if (!local_adgn.seen) continue;

        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) return ret;
        }
        first = false;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"name\":",
                                 local_adgn.dgn);
        if (ret != ESP_OK) return ret;

        ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                        local_adgn.name != NULL ? local_adgn.name : "UNKNOWN");
        if (ret != ESP_OK) return ret;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 ",\"count\":%u", local_adgn.count);
        if (ret != ESP_OK) return ret;

        if (local_adgn.has_source_id) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"source_id\":%u", (unsigned)local_adgn.source_id);
            if (ret != ESP_OK) return ret;
        }

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "}");
        if (ret != ESP_OK) return ret;
    }

    for (size_t i = 0; i < MAX_UNKNOWN_ACTIVE_DGNS; i++) {
        active_dgn_state_t local_u;
        lc_state_lock();
        local_u = unknown_active_dgns[i];
        lc_state_unlock();

        if (!local_u.seen) continue;
        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) return ret;
        }
        first = false;
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"name\":\"UNKNOWN-0x%05" PRIX32 "\",\"count\":%u,\"unknown\":true",
                                 local_u.dgn, local_u.dgn, local_u.count);
        if (ret != ESP_OK) return ret;
        if (local_u.has_source_id) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"source_id\":%u", (unsigned)local_u.source_id);
            if (ret != ESP_OK) return ret;
        }
        if (local_u.has_data) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"len\":%u,\"data\":[", (unsigned)local_u.len);
            if (ret != ESP_OK) return ret;
            for (uint8_t b = 0; b < local_u.len && b < 8; b++) {
                if (b > 0) {
                    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
                    if (ret != ESP_OK) return ret;
                }
                ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "%u", (unsigned)local_u.data[b]);
                if (ret != ESP_OK) return ret;
            }
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "]");
            if (ret != ESP_OK) return ret;
        }
        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "}");
        if (ret != ESP_OK) return ret;
    }

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "],\"states\":[");
    if (ret != ESP_OK) return ret;

    first = true;
    int tracked_state_count = 0;
    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        tracked_state_t local_ts;
        lc_state_lock();
        local_ts = tracked_states[i];
        lc_state_unlock();

        if (!local_ts.valid || !local_ts.has_snapshot) continue;

        const dgn_desc_t *desc = find_dgn_desc(local_ts.dgn);
        if (desc == NULL) continue;
        tracked_state_count++;

        if (!first) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
            if (ret != ESP_OK) return ret;
        }
        first = false;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 "{\"dgn\":\"0x%05" PRIX32 "\",\"dgn_name\":",
                                 local_ts.dgn);
        if (ret != ESP_OK) return ret;

        ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                        desc->name != NULL ? desc->name : "UNKNOWN");
        if (ret != ESP_OK) return ret;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"instance\":");
        if (ret != ESP_OK) return ret;

        if (local_ts.instance >= 0) {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"%d\"", local_ts.instance);
        } else {
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "\"n/a\"");
        }
        if (ret != ESP_OK) return ret;

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                 ",\"source_id\":%u,\"last_update_us\":%" PRId64 ",\"fields\":[",
                                 local_ts.source_id, local_ts.last_update_us);
        if (ret != ESP_OK) return ret;

        bool first_field = true;
        for (uint16_t f = 0; f < desc->field_count && f < 32; f++) {
            const field_desc_t *field = &desc->fields[f];

            char value_text[FIELD_VALUE_STRING_MAX];
            char raw_value_text[FIELD_VALUE_STRING_MAX];
            format_field_value(value_text, sizeof(value_text), local_ts.data, local_ts.len, field);
            format_raw_field_value(raw_value_text, sizeof(raw_value_text), local_ts.data, local_ts.len, field);
            uint64_t numeric_value = extract_field_value(local_ts.data, local_ts.len, field);
            const char *value_desc = lookup_value_name(field, numeric_value);

            if (!first_field) {
                ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",");
                if (ret != ESP_OK) return ret;
            }
            first_field = false;

            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "{\"name\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos,
                                            field->name != NULL ? field->name : "value");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"value\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, value_text);
            if (ret != ESP_OK) return ret;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"raw_value\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, raw_value_text);
            if (ret != ESP_OK) return ret;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"value_description\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, value_desc != NULL ? value_desc : "");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"value_label\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, value_desc != NULL ? value_desc : "");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, ",\"unit\":");
            if (ret != ESP_OK) return ret;
            ret = state_chunk_escape_append(req, chunk, sizeof(chunk), &pos, field->unit != NULL ? field->unit : "");
            if (ret != ESP_OK) return ret;

            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                                     ",\"type\":\"%s\",\"byte_start\":%u,\"byte_end\":%u,\"bit_start\":%u,\"bit_end\":%u,\"seq\":%u}",
                                     field_type_to_string(field->type),
                                     field->byte_start, field->byte_end,
                                     field->bit_start, field->bit_end,
                                     local_ts.field_change_seq[f]);
            if (ret != ESP_OK) return ret;
        }

        ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "]}");
        if (ret != ESP_OK) return ret;
    }

    ret = state_chunk_append(req, chunk, sizeof(chunk), &pos,
                             "],\"tracked_states\":%d,\"max_tracked_states\":%d}",
                             tracked_state_count, MAX_TRACKED_STATES);
    if (ret != ESP_OK) return ret;

    ret = state_chunk_flush(req, chunk, &pos);
    if (ret != ESP_OK) return ret;

    return httpd_resp_send_chunk(req, NULL, 0);
}

// Zeroes the CAN-side diagnostic counters so a stale boot burst doesn't sit
// under "dropped" forever. Shared by the manual reset endpoint and the
// one-shot post-boot timer.
static void reset_can_diag_counters(void)
{
    lc_state_lock();
    diag_rx_frames = 0;
    diag_rx_dropped_isr = 0;
    diag_rx_busy_us = 0;
    diag_last_rx_frames = 0;
    diag_last_rx_busy_us = 0;
    diag_window_start_us = 0;
    lc_state_unlock();
}

static esp_err_t api_diag_reset_get_handler(httpd_req_t *req)
{
    reset_can_diag_counters();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}

// One-shot, ~10 s after boot: the TWAI ISR starts filling its queue before
// Wi-Fi/httpd bring-up finishes, so a small burst of early drops is normal —
// clear once so steady-state numbers are what the page shows.
static void boot_diag_reset_timer_cb(void *arg)
{
    (void)arg;
    reset_can_diag_counters();
    ESP_LOGI(TAG, "auto-reset CAN diagnostic counters 10s after boot");
}

// ── RV-C frame logger endpoints (glue over lc_rvclog.c) ────────────────────

static esp_err_t api_rvclog_start_get_handler(httpd_req_t *req)
{
    uint32_t max_secs = LC_RVCLOG_DEFAULT_MAX_SECS;
    char query[64];
    char value[16];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK &&
        httpd_query_key_value(query, "max_secs", value, sizeof(value)) == ESP_OK) {
        long parsed = strtol(value, NULL, 10);
        if (parsed > 0) max_secs = (uint32_t)parsed;
    }

    esp_err_t err = lc_rvclog_start(max_secs);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    if (err != ESP_OK) {
        // The one way start can fail here (unlike the hub): the lazy 80 KB
        // buffer allocation didn't fit. Say so plainly.
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"not enough free memory for the capture buffer\"}");
    }

    lc_rvclog_status_t st;
    lc_rvclog_get_status(&st);
    char buf[96];
    snprintf(buf, sizeof(buf), "{\"ok\":true,\"max_secs\":%lu}", (unsigned long)st.max_secs);
    return httpd_resp_sendstr(req, buf);
}

static esp_err_t api_rvclog_stop_get_handler(httpd_req_t *req)
{
    lc_rvclog_stop();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}

static esp_err_t api_rvclog_mark_get_handler(httpd_req_t *req)
{
    uint32_t seq = 0;
    esp_err_t err = lc_rvclog_mark(&seq);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    char buf[96];
    if (err == ESP_OK) {
        snprintf(buf, sizeof(buf), "{\"ok\":true,\"seq\":%lu}", (unsigned long)seq);
        return httpd_resp_sendstr(req, buf);
    }

    const char *reason = (err == ESP_ERR_NO_MEM) ? "buffer full" : "no capture running";
    snprintf(buf, sizeof(buf), "{\"ok\":false,\"error\":\"%s\"}", reason);
    httpd_resp_set_status(req, "409 Conflict");
    return httpd_resp_sendstr(req, buf);
}

static esp_err_t api_rvclog_status_get_handler(httpd_req_t *req)
{
    lc_rvclog_status_t st;
    lc_rvclog_get_status(&st);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    char buf[192];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"capturing\":%s,\"buffer_full\":%s,\"count\":%lu,\"capacity\":%lu,\"elapsed_s\":%lu,\"max_secs\":%lu}",
        st.capturing ? "true" : "false", st.buffer_full ? "true" : "false",
        (unsigned long)st.count, (unsigned long)st.capacity,
        (unsigned long)st.elapsed_s, (unsigned long)st.max_secs);
    return httpd_resp_sendstr(req, buf);
}

// Streams the captured buffer as CSV: seq,t_ms,can_id_hex,dlc,data_hex,dir.
// Marker rows use "MARK" in the can_id column. Only available once capture
// is stopped — the viewer expects a complete, gapless window.
static esp_err_t api_rvclog_download_get_handler(httpd_req_t *req)
{
    lc_rvclog_status_t st;
    lc_rvclog_get_status(&st);

    if (st.capturing) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "close");
        httpd_resp_set_status(req, "409 Conflict");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"stop the capture before downloading\"}");
    }

    httpd_resp_set_type(req, "text/csv");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=\"rvclog.csv\"");

    char chunk[1024];
    size_t pos = 0;
    esp_err_t ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "seq,t_ms,can_id_hex,dlc,data_hex,dir\n");
    if (ret != ESP_OK) return ret;

    lc_rvclog_record_t rec;
    for (uint32_t i = 0; i < st.count; i++) {
        if (!lc_rvclog_get_record_copy(i, &rec)) break;

        double t_ms = rec.t_us / 1000.0;
        if (rec.can_id == LC_RVCLOG_MARK_ID) {
            uint32_t mark_seq = (uint32_t)rec.data[0] | ((uint32_t)rec.data[1] << 8) |
                                 ((uint32_t)rec.data[2] << 16) | ((uint32_t)rec.data[3] << 24);
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "%lu,%.1f,MARK,%lu,,\n",
                (unsigned long)(i + 1), t_ms, (unsigned long)mark_seq);
        } else {
            char hex[17];
            for (uint8_t b = 0; b < rec.len; b++) {
                snprintf(hex + (b * 2), 3, "%02X", rec.data[b]);
            }
            hex[rec.len * 2] = '\0';
            const char *dir = (rec.dir == LC_RVCLOG_DIR_TX)      ? "TX"
                            : (rec.dir == LC_RVCLOG_DIR_TX_FAIL) ? "TXFAIL"
                                                                 : "RX";
            ret = state_chunk_append(req, chunk, sizeof(chunk), &pos, "%lu,%.1f,%08lX,%u,%s,%s\n",
                (unsigned long)(i + 1), t_ms, (unsigned long)rec.can_id, (unsigned)rec.len, hex, dir);
        }
        if (ret != ESP_OK) return ret;
    }

    ret = state_chunk_flush(req, chunk, &pos);
    if (ret != ESP_OK) return ret;
    return httpd_resp_send_chunk(req, NULL, 0);
}

// ── DGN enable / discovery mode ────────────────────────────────────────────

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

    lc_state_lock();
    lc_state_set_select_all_mode(false);

    bool found = false;
    bool was_enabled = false;
    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        if (WATCHED_DGNS[i].dgn == dgn) {
            was_enabled = watched_dgn_is_enabled(i);
            watched_dgn_set_enabled(i, enabled);
            found = true;
            break;
        }
    }

    // Clear stale state rows only when transitioning disabled → enabled;
    // re-enabling an already-enabled DGN must NOT flush live tracked data.
    if (enabled && !was_enabled) {
        for (int i = 0; i < MAX_TRACKED_STATES; i++) {
            if (tracked_states[i].valid && tracked_states[i].dgn == dgn) {
                memset(&tracked_states[i], 0, sizeof(tracked_states[i]));
            }
        }
    }

    lc_state_unlock();

    if (!found) {
        ESP_LOGW(TAG, "/api/enable: DGN 0x%05" PRIX32 " not in WATCHED_DGNS[] (enabled=%d requested)",
                 dgn, (int)enabled);
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "DGN not found");
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}

static esp_err_t api_deselect_all_get_handler(httpd_req_t *req)
{
    lc_state_lock();
    lc_state_set_select_all_mode(false);

    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        watched_dgn_set_enabled(i, false);
    }
    lc_state_clear_tracked();

    lc_state_unlock();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}

static esp_err_t api_select_all_get_handler(httpd_req_t *req)
{
    lc_state_lock();
    lc_state_set_select_all_mode(true);

    for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
        watched_dgn_set_enabled(i, false);
    }
    lc_state_clear_all(WATCHED_DGN_COUNT);

    lc_state_unlock();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true,\"mode\":\"discovery\"}");
}

// ── /ws — delta-stream WebSocket ───────────────────────────────────────────

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

    // All slots full: replace the oldest so stale clients don't pin the table.
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

static bool ws_is_full(int new_fd)
{
    if (ws_client_mutex != NULL) xSemaphoreTake(ws_client_mutex, portMAX_DELAY);
    bool full = true;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_client_fds[i] < 0 || ws_client_fds[i] == new_fd) { full = false; break; }
    }
    if (ws_client_mutex != NULL) xSemaphoreGive(ws_client_mutex);
    return full;
}

static void ws_enqueue_text(const char *text)
{
    if (ws_event_queue == NULL || text == NULL) {
        return;
    }

    ws_event_t event = {0};
    strncpy(event.text, text, sizeof(event.text) - 1);

    if (xQueueSend(ws_event_queue, &event, 0) != pdTRUE) {
        // Drop the oldest pending event and try once more — never block the
        // CAN dispatch task.
        ws_event_t dropped;
        (void)xQueueReceive(ws_event_queue, &dropped, 0);
        (void)xQueueSend(ws_event_queue, &event, 0);
    }
}

void ws_enqueue_clear(void)
{
    ws_enqueue_text("{\"type\":\"clear\"}");
}

void ws_enqueue_activity_event(uint32_t dgn, const char *dgn_name, uint32_t count, bool has_source_id, uint8_t source_id)
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

void ws_enqueue_field_event(uint32_t dgn, const char *dgn_name, int16_t instance, const rvc_rx_frame_t *frame, const field_desc_t *field, bool changed, uint32_t seq)
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
    json_append(event, sizeof(event), &pos, ",\"value_label\":");
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
    (void)arg;
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

        if (client_count == 0) {
            s_ws_noclient++;
            continue;
        }

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
            } else {
                s_ws_sent++;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void ws_handle_command(const char *cmd)
{
    if (cmd == NULL) return;

    if (strcmp(cmd, "select_all") == 0) {
        // Select All enters lightweight discovery mode — activity events
        // identify active DGNs; the user enables specific DGNs for decode.
        lc_state_lock();
        lc_state_set_select_all_mode(true);
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            watched_dgn_set_enabled(i, false);
        }
        lc_state_clear_all(WATCHED_DGN_COUNT);
        lc_state_unlock();
        ws_enqueue_clear();
        ws_enqueue_text("{\"type\":\"mode\",\"mode\":\"discovery\"}");
        return;
    }

    if (strcmp(cmd, "deselect_all") == 0) {
        lc_state_lock();
        lc_state_set_select_all_mode(false);
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            watched_dgn_set_enabled(i, false);
        }
        lc_state_clear_all(WATCHED_DGN_COUNT);
        lc_state_unlock();
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

        lc_state_lock();
        lc_state_set_select_all_mode(false);
        for (size_t i = 0; i < WATCHED_DGN_COUNT; i++) {
            if (WATCHED_DGNS[i].dgn == dgn) {
                watched_dgn_set_enabled(i, enabled);
                break;
            }
        }
        lc_state_clear_tracked();
        lc_state_unlock();
        ws_enqueue_clear();
        ws_enqueue_text("{\"type\":\"mode\",\"mode\":\"selected\"}");
        return;
    }
}

static esp_err_t websocket_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        int fd = httpd_req_to_sockfd(req);
        if (ws_is_full(fd)) {
            static const char *reject = "{\"type\":\"error\",\"error\":\"max_clients\"}";
            httpd_ws_frame_t pkt = { .type = HTTPD_WS_TYPE_TEXT,
                                     .payload = (uint8_t *)reject,
                                     .len = strlen(reject) };
            httpd_ws_send_frame(req, &pkt);
            ESP_LOGW(TAG, "WebSocket /ws rejected: max clients reached (fd=%d)", fd);
            return ESP_FAIL;
        }
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

    // Commands are acknowledged by async clear/mode messages — delta-only.
    return ESP_OK;
}

// ── Wi-Fi manager ──────────────────────────────────────────────────────────

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

static void schedule_wifi_reconnect(void);

static esp_err_t load_saved_networks_from_nvs(void)
{
    memset(&saved_networks, 0, sizeof(saved_networks));
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(WIFI_SAVED_NETWORKS_NVS_NS, NVS_READONLY, &handle);
    if (ret != ESP_OK) return ret;

    size_t blob_size = sizeof(saved_networks);
    ret = nvs_get_blob(handle, WIFI_SAVED_NETWORKS_NVS_KEY, &saved_networks, &blob_size);
    nvs_close(handle);

    if (ret == ESP_OK && saved_networks.version == WIFI_SAVED_NETWORKS_VERSION) return ESP_OK;

    /* Blob missing or version mismatch — start empty. */
    memset(&saved_networks, 0, sizeof(saved_networks));
    return ESP_OK;
}

static esp_err_t save_saved_networks_to_nvs(void)
{
    saved_networks.version = WIFI_SAVED_NETWORKS_VERSION;
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(WIFI_SAVED_NETWORKS_NVS_NS, NVS_READWRITE, &handle);
    if (ret != ESP_OK) return ret;
    ret = nvs_set_blob(handle, WIFI_SAVED_NETWORKS_NVS_KEY, &saved_networks, sizeof(saved_networks));
    if (ret == ESP_OK) ret = nvs_commit(handle);
    nvs_close(handle);
    return ret;
}

static int find_saved_network_index(const char *ssid)
{
    if (ssid == NULL || ssid[0] == '\0') return -1;
    for (int i = 0; i < saved_networks.count; i++) {
        if (strncmp(saved_networks.networks[i].ssid, ssid, 32) == 0) return i;
    }
    return -1;
}

static bool ssid_in_last_scan(const char *ssid)
{
    if (ssid == NULL || ssid[0] == '\0' || last_scan_count == 0) return false;
    for (uint16_t i = 0; i < last_scan_count; i++) {
        if (strncmp((const char *)last_scan_records[i].ssid, ssid, 32) == 0) return true;
    }
    return false;
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

static esp_err_t connect_best_saved_network(void)
{
    if (saved_networks.count == 0) return ESP_ERR_NOT_FOUND;
    if (wifi_scan_in_progress) return ESP_ERR_WIFI_STATE;

    wifi_scan_in_progress = true;

    wifi_scan_config_t scan_cfg = {0};
    esp_err_t ret = esp_wifi_scan_start(&scan_cfg, true);
    if (ret != ESP_OK) {
        wifi_scan_in_progress = false;
        return ret;
    }

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count > WIFI_STA_MAX_SCAN_RESULTS) ap_count = WIFI_STA_MAX_SCAN_RESULTS;
    esp_wifi_scan_get_ap_records(&ap_count, last_scan_records);
    last_scan_count = ap_count;
    wifi_scan_in_progress = false;

    for (int i = 0; i < saved_networks.count; i++) {
        if (ssid_in_last_scan(saved_networks.networks[i].ssid)) {
            ESP_LOGI(TAG, "Connecting to saved network priority %d: %s", i, saved_networks.networks[i].ssid);
            return wifi_sta_apply_and_connect(saved_networks.networks[i].ssid,
                                              saved_networks.networks[i].password);
        }
    }
    ESP_LOGI(TAG, "No saved networks found in scan (%d APs visible)", ap_count);
    return ESP_ERR_NOT_FOUND;
}

static void wifi_reconnect_task(void *arg)
{
    (void)arg;
    uint32_t delay_ms = (wifi_sta_retry_attempts < WIFI_RECONNECT_BACKOFF_AFTER)
                        ? WIFI_RECONNECT_DELAY_MS
                        : WIFI_RECONNECT_DELAY_LONG_MS;
    vTaskDelay(pdMS_TO_TICKS(delay_ms));

    if (!wifi_sta_got_ip && !wifi_scan_in_progress && saved_networks.count > 0) {
        wifi_sta_retry_attempts++;
        ESP_LOGI(TAG, "Wi-Fi reconnect attempt %" PRIu32 " (delay %lums, %d saved networks)",
                 wifi_sta_retry_attempts, (unsigned long)delay_ms, saved_networks.count);
        esp_err_t ret = connect_best_saved_network();
        if (ret == ESP_ERR_NOT_FOUND) {
            // No saved network visible yet — keep retrying on the backoff.
            wifi_reconnect_task_pending = false;
            schedule_wifi_reconnect();
            vTaskDelete(NULL);
            return;
        }
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    }

    wifi_reconnect_task_pending = false;
    vTaskDelete(NULL);
}

static void schedule_wifi_reconnect(void)
{
    if (wifi_reconnect_task_pending || saved_networks.count == 0) return;
    if (wifi_scan_in_progress) return;

    wifi_reconnect_task_pending = true;
    BaseType_t ok = xTaskCreate(wifi_reconnect_task, "wifi_reconnect", 6144, NULL, 3, NULL);
    if (ok != pdPASS) wifi_reconnect_task_pending = false;
}

// ── Periodic roam: upgrade to a higher-priority network if one appears ─────

static void wifi_roam_task(void *arg)
{
    (void)arg;

    int current_priority = -1;
    for (int i = 0; i < (int)saved_networks.count; i++) {
        if (strncmp(saved_networks.networks[i].ssid, wifi_sta_ssid, 32) == 0) {
            current_priority = i;
            break;
        }
    }
    if (current_priority <= 0) goto roam_done;  // already on highest or unknown

    wifi_scan_in_progress = true;
    wifi_scan_config_t scan_cfg = {0};
    if (esp_wifi_scan_start(&scan_cfg, true) != ESP_OK) {
        wifi_scan_in_progress = false;
        goto roam_done;
    }

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count > WIFI_STA_MAX_SCAN_RESULTS) ap_count = WIFI_STA_MAX_SCAN_RESULTS;
    esp_wifi_scan_get_ap_records(&ap_count, last_scan_records);
    last_scan_count = ap_count;
    wifi_scan_in_progress = false;

    for (int i = 0; i < current_priority; i++) {
        if (ssid_in_last_scan(saved_networks.networks[i].ssid)) {
            ESP_LOGI(TAG, "Wi-Fi roam: upgrading from '%s' (pri %d) to '%s' (pri %d)",
                     wifi_sta_ssid, current_priority,
                     saved_networks.networks[i].ssid, i);
            wifi_sta_apply_and_connect(saved_networks.networks[i].ssid,
                                       saved_networks.networks[i].password);
            break;
        }
    }

roam_done:
    wifi_roam_task_pending = false;
    vTaskDelete(NULL);
}

static void wifi_roam_timer_cb(void *arg)
{
    (void)arg;
    if (wifi_roam_task_pending || wifi_reconnect_task_pending || wifi_scan_in_progress) return;

    // Reconnect watchdog: if the event-driven reconnect chain ever died
    // (xTaskCreate failure), this 5-minute timer is the safe fallback —
    // schedule_wifi_reconnect() no-ops while one is already pending.
    if (!wifi_sta_got_ip) {
        if (saved_networks.count > 0) {
            ESP_LOGW(TAG, "Wi-Fi watchdog: still disconnected, forcing a reconnect attempt");
            schedule_wifi_reconnect();
        }
        return;
    }

    if (saved_networks.count <= 1) return;

    int current_priority = -1;
    for (int i = 0; i < (int)saved_networks.count; i++) {
        if (strncmp(saved_networks.networks[i].ssid, wifi_sta_ssid, 32) == 0) {
            current_priority = i;
            break;
        }
    }
    if (current_priority <= 0) return;  // already on highest

    wifi_roam_task_pending = true;
    BaseType_t ok = xTaskCreate(wifi_roam_task, "wifi_roam", 4096, NULL, 3, NULL);
    if (ok != pdPASS) wifi_roam_task_pending = false;
}

// ── Wi-Fi API endpoints ────────────────────────────────────────────────────

static esp_err_t api_wifi_status_get_handler(httpd_req_t *req)
{
    wifi_ap_record_t sta_info = {0};
    bool have_sta_info = (wifi_sta_connected && esp_wifi_sta_get_ap_info(&sta_info) == ESP_OK);

    wifi_config_t ap_cfg = {0};
    esp_wifi_get_config(WIFI_IF_AP, &ap_cfg);

    wifi_sta_list_t sta_list = {0};
    esp_wifi_ap_get_sta_list(&sta_list);

    char json[640];
    snprintf(json, sizeof(json),
             "{\"ok\":true"
             ",\"ap_ssid\":\"%s\",\"ap_ip\":\"%s\",\"ap_channel\":%d,\"ap_clients\":%d"
             ",\"sta_connected\":%s,\"sta_got_ip\":%s"
             ",\"sta_ssid\":\"%s\",\"sta_ip\":\"%s\",\"sta_rssi\":%d"
             ",\"saved_ssid\":\"%s\",\"hub_id\":\"%s\"}",
             s_ap_ssid, WIFI_AP_IP_STRING,
             ap_cfg.ap.channel, sta_list.num,
             wifi_sta_connected ? "true" : "false",
             wifi_sta_got_ip ? "true" : "false",
             wifi_sta_ssid, wifi_sta_ip,
             have_sta_info ? sta_info.rssi : 0,
             saved_networks.count > 0 ? saved_networks.networks[0].ssid : "",
             s_dev_id);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

static esp_err_t api_wifi_scan_get_handler(httpd_req_t *req)
{
    wifi_scan_in_progress = true;

    // A stale saved STA profile can continuously retry while the user is on
    // the AP config page — disconnect the STA side before a manual scan so
    // the scan isn't racing the reconnect task. The AP stays up (AP+STA).
    if (!wifi_sta_got_ip) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_disconnect());
        vTaskDelay(pdMS_TO_TICKS(WIFI_SCAN_DISCONNECT_SETTLE_MS));
    }

    wifi_scan_config_t scan_config = {0};
    esp_err_t scan_ret = esp_wifi_scan_start(&scan_config, true);
    if (scan_ret != ESP_OK) {
        wifi_scan_in_progress = false;
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
    /* Cache results for /api/wifi/networks (saved network visibility check) */
    memcpy(last_scan_records, records, ap_count * sizeof(wifi_ap_record_t));
    last_scan_count = ap_count;
    wifi_scan_in_progress = false;
    if (!wifi_sta_got_ip) schedule_wifi_reconnect();

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

        const char *auth_mode_str = (records[i].authmode == WIFI_AUTH_OPEN) ? "OPEN" : "WPA2";
        snprintf(chunk, sizeof(chunk), "%s{\"ssid\":\"%s\",\"rssi\":%d,\"auth_mode\":\"%s\"}",
                 i == 0 ? "" : ",",
                 escaped,
                 records[i].rssi,
                 auth_mode_str);
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
    /* Add or update SSID in saved list (upsert at front if new). strlcpy
     * against real field sizes — see the hub for the -Werror reasoning; the
     * memsets keep NVS blob tail bytes zeroed. */
    int existing = find_saved_network_index(ssid);
    esp_err_t save_ret;
    if (existing >= 0) {
        memset(saved_networks.networks[existing].password, 0,
               sizeof(saved_networks.networks[existing].password));
        strlcpy(saved_networks.networks[existing].password, password,
                sizeof(saved_networks.networks[existing].password));
    } else if (saved_networks.count < WIFI_MAX_SAVED_NETWORKS) {
        /* Insert at front (highest priority) by shifting others down */
        memmove(&saved_networks.networks[1], &saved_networks.networks[0],
                saved_networks.count * sizeof(wifi_saved_network_t));
        memset(&saved_networks.networks[0], 0, sizeof(wifi_saved_network_t));
        strlcpy(saved_networks.networks[0].ssid,     ssid,
                sizeof(saved_networks.networks[0].ssid));
        strlcpy(saved_networks.networks[0].password, password,
                sizeof(saved_networks.networks[0].password));
        saved_networks.count++;
    } else {
        /* List full — replace lowest priority entry */
        memset(&saved_networks.networks[WIFI_MAX_SAVED_NETWORKS - 1], 0, sizeof(wifi_saved_network_t));
        strlcpy(saved_networks.networks[WIFI_MAX_SAVED_NETWORKS - 1].ssid,     ssid,
                sizeof(saved_networks.networks[WIFI_MAX_SAVED_NETWORKS - 1].ssid));
        strlcpy(saved_networks.networks[WIFI_MAX_SAVED_NETWORKS - 1].password, password,
                sizeof(saved_networks.networks[WIFI_MAX_SAVED_NETWORKS - 1].password));
    }
    save_ret = save_saved_networks_to_nvs();
    if (save_ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to save Wi-Fi network list to NVS: %s", esp_err_to_name(save_ret));
    }
    wifi_sta_retry_attempts = 0;
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

/* GET /api/wifi/networks — saved networks with visibility info. No "trusted"
 * field: the concept only existed to bypass the hub's PIN. */
static esp_err_t api_wifi_networks_get_handler(httpd_req_t *req)
{
    char buf[160];
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    httpd_resp_send_chunk(req, "{\"ok\":true,\"networks\":[", 23);
    for (int i = 0; i < saved_networks.count; i++) {
        char ssid_esc[80] = {0};
        size_t epos = 0;
        for (const char *p = saved_networks.networks[i].ssid; *p && epos + 3 < sizeof(ssid_esc); p++) {
            if (*p == '"' || *p == '\\') { ssid_esc[epos++] = '\\'; }
            ssid_esc[epos++] = *p;
        }
        bool visible = ssid_in_last_scan(saved_networks.networks[i].ssid);
        bool connected = (wifi_sta_got_ip &&
                          strncmp(wifi_sta_ssid, saved_networks.networks[i].ssid, 32) == 0);
        snprintf(buf, sizeof(buf),
                 "%s{\"ssid\":\"%s\",\"visible\":%s,\"connected\":%s}",
                 i == 0 ? "" : ",",
                 ssid_esc,
                 visible    ? "true" : "false",
                 connected  ? "true" : "false");
        httpd_resp_send_chunk(req, buf, strlen(buf));
    }
    httpd_resp_send_chunk(req, "]}", 2);
    return httpd_resp_send_chunk(req, NULL, 0);
}

/* GET /api/wifi/networks/forget?ssid=... */
static esp_err_t api_wifi_networks_forget_handler(httpd_req_t *req)
{
    char query[128] = {0};
    char ssid[64] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "ssid", ssid, sizeof(ssid)) != ESP_OK || ssid[0] == '\0') {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected ssid");
    }
    url_decode_in_place(ssid);

    int idx = find_saved_network_index(ssid);
    if (idx < 0) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid not found");
    }
    /* Shift remaining entries up */
    memmove(&saved_networks.networks[idx], &saved_networks.networks[idx + 1],
            (saved_networks.count - idx - 1) * sizeof(wifi_saved_network_t));
    memset(&saved_networks.networks[saved_networks.count - 1], 0, sizeof(wifi_saved_network_t));
    saved_networks.count--;

    esp_err_t ret = save_saved_networks_to_nvs();
    char json[96];
    snprintf(json, sizeof(json), "{\"ok\":%s,\"ssid\":\"%s\"}",
             ret == ESP_OK ? "true" : "false", ssid);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

/* GET /api/wifi/networks/move?ssid=...&dir=up|down — change priority */
static esp_err_t api_wifi_networks_move_handler(httpd_req_t *req)
{
    char query[160] = {0};
    char ssid[64] = {0};
    char dir[8] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "ssid", ssid, sizeof(ssid)) != ESP_OK ||
        httpd_query_key_value(query, "dir", dir, sizeof(dir)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected ssid and dir");
    }
    url_decode_in_place(ssid);

    int idx = find_saved_network_index(ssid);
    if (idx < 0) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid not found");

    int swap = -1;
    if (strcmp(dir, "up") == 0 && idx > 0) swap = idx - 1;
    else if (strcmp(dir, "down") == 0 && idx < saved_networks.count - 1) swap = idx + 1;

    if (swap >= 0) {
        wifi_saved_network_t tmp = saved_networks.networks[idx];
        saved_networks.networks[idx]  = saved_networks.networks[swap];
        saved_networks.networks[swap] = tmp;
        save_saved_networks_to_nvs();
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, "{\"ok\":true}");
}

/* GET /api/wifi/networks/select?ssid=... — manual connect (runtime only) */
static esp_err_t api_wifi_networks_select_handler(httpd_req_t *req)
{
    char query[128] = {0};
    char ssid[64] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "ssid", ssid, sizeof(ssid)) != ESP_OK || ssid[0] == '\0') {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected ssid");
    }
    url_decode_in_place(ssid);

    int idx = find_saved_network_index(ssid);
    if (idx < 0) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid not in saved list");
    if (!ssid_in_last_scan(ssid)) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid not found in last scan — scan first");

    wifi_sta_retry_attempts = 0;
    esp_err_t ret = wifi_sta_apply_and_connect(saved_networks.networks[idx].ssid,
                                               saved_networks.networks[idx].password);
    char json[128];
    snprintf(json, sizeof(json), "{\"ok\":%s,\"status\":\"%s\",\"ssid\":\"%s\"}",
             ret == ESP_OK ? "true" : "false",
             ret == ESP_OK ? "connecting" : esp_err_to_name(ret),
             ssid);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

// ── /api/rvc/send + closed-loop confirm ────────────────────────────────────

#define RVC_CFM_LOCK_TIMEOUT_MS 60
#define RVC_SEND_CONFIRM_TIMEOUT_MS 30   // 3 ticks at HZ=100 → 20-30 ms real

static volatile bool     s_cfm_armed = false;
static volatile uint32_t s_cfm_status_dgn = 0;  // raw 18-bit DGN of the expected status echo
static volatile int16_t  s_cfm_instance = -1;   // required data[0], -1 = any
static SemaphoreHandle_t s_cfm_sem = NULL;
static SemaphoreHandle_t s_cfm_lock = NULL;
// Sticky (RAM-only): once ANY send has seen a confirmation, this bus is
// known to confirm, so later sends may retry. On a bus that never confirms,
// zero retries ever happen — the "no automatic retry on a dead bus" rule the
// hub established, kept by construction.
static bool s_cfm_ever_confirmed = false;

void lc_web_tx_confirm_check(uint32_t can_id, const uint8_t *data, uint8_t len)
{
    if (!s_cfm_armed) return;

    uint32_t raw_dgn = (can_id >> 8) & 0x3FFFF;
    bool hit = false;
    // ACKNOWLEDGMENT is PDU1: 0E8xx — match the whole family so broadcast
    // (0E8FF) and directed (0E8F9) forms both count.
    if ((raw_dgn >> 8) == 0x0E8) {
        hit = true;
    } else if (s_cfm_status_dgn != 0 && raw_dgn == s_cfm_status_dgn) {
        int16_t want = s_cfm_instance;
        if (want < 0 || (len > 0 && data[0] == (uint8_t)want)) hit = true;
    }

    if (hit) {
        s_cfm_armed = false;
        if (s_cfm_sem != NULL) xSemaphoreGive(s_cfm_sem);
    }
}

// Closed-loop transmit: arm the confirm watch, send, wait for the target's
// ACK/status echo, resend on silence (gated by s_cfm_ever_confirmed).
// Returns the transmit result — ESP_OK means the frame reached the bus, NOT
// that the device obeyed (that's *out_confirmed).
static esp_err_t rvc_tx_confirmed(uint32_t dgn, const uint8_t *bytes,
                                  uint32_t cfm_dgn, int16_t cfm_inst, int max_retries,
                                  bool *out_confirmed, int *out_retries, uint32_t *out_can_id)
{
    if (out_confirmed) *out_confirmed = false;
    if (out_retries)   *out_retries = 0;

    bool watch = (cfm_dgn != 0 && cfm_dgn <= 0x3FFFF && s_cfm_sem != NULL && s_cfm_lock != NULL);
    if (!watch) {
        return lc_can_transmit_rvc(dgn, 0xF9, bytes, 8, out_can_id);
    }

    // Send unconfirmed rather than block behind another confirm cycle.
    if (xSemaphoreTake(s_cfm_lock, pdMS_TO_TICKS(RVC_CFM_LOCK_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGD(TAG, "confirm watch busy; sending DGN 0x%05" PRIX32 " unconfirmed", dgn);
        return lc_can_transmit_rvc(dgn, 0xF9, bytes, 8, out_can_id);
    }

    esp_err_t ret = ESP_OK;
    int retries = 0;
    while (1) {
        (void)xSemaphoreTake(s_cfm_sem, 0);   // drain a stale give
        s_cfm_status_dgn = cfm_dgn;
        s_cfm_instance   = cfm_inst;
        s_cfm_armed      = true;

        ret = lc_can_transmit_rvc(dgn, 0xF9, bytes, 8, out_can_id);
        if (ret != ESP_OK) { s_cfm_armed = false; break; }

        bool confirmed = (xSemaphoreTake(s_cfm_sem, pdMS_TO_TICKS(RVC_SEND_CONFIRM_TIMEOUT_MS)) == pdTRUE);
        s_cfm_armed = false;
        if (confirmed) {
            s_cfm_ever_confirmed = true;
            if (out_confirmed) *out_confirmed = true;
            break;
        }
        if (!s_cfm_ever_confirmed) break;
        if (retries >= max_retries) break;
        retries++;
    }

    if (out_retries) *out_retries = retries;
    xSemaphoreGive(s_cfm_lock);
    return ret;
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

static esp_err_t api_rvc_send_get_handler(httpd_req_t *req)
{
    char query[256] = {0};
    char dgn_text[24] = {0};
    char byte_text[8][8] = {{0}};
    uint8_t bytes[8] = {0};

    if (!lc_can_is_ready()) return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "TWAI not ready");

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
        httpd_query_key_value(query, "dgn", dgn_text, sizeof(dgn_text)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "expected dgn and b0..b7 query parameters");
    }

    // dgn == 0 is rejected: strtoul() returns 0 for unparseable input, so
    // accepting it would let a malformed DGN silently transmit as DGN 0.
    uint32_t dgn = (uint32_t)strtoul(dgn_text, NULL, 16);
    if (dgn == 0 || dgn > 0x3FFFF) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "DGN must be 0x00001 through 0x3FFFF");

    for (int i = 0; i < 8; i++) {
        char key[4];
        snprintf(key, sizeof(key), "b%d", i);
        if (httpd_query_key_value(query, key, byte_text[i], sizeof(byte_text[i])) != ESP_OK || !parse_u8_decimal(byte_text[i], &bytes[i])) {
            char err[96];
            snprintf(err, sizeof(err), "byte %d must be decimal 0 through 255", i);
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err);
        }
    }

    // Optional closed-loop confirm hints: cfm_dgn = status DGN to watch for,
    // cfm_inst = its byte-0 instance. Absent means a plain transmit.
    uint32_t cfm_dgn = 0;
    int16_t  cfm_inst = -1;
    {
        char cfm_text[16] = {0};
        if (httpd_query_key_value(query, "cfm_dgn", cfm_text, sizeof(cfm_text)) == ESP_OK) {
            cfm_dgn = (uint32_t)strtoul(cfm_text, NULL, 16);
            if (cfm_dgn > 0x3FFFF) cfm_dgn = 0;
            char inst_text[8] = {0};
            if (httpd_query_key_value(query, "cfm_inst", inst_text, sizeof(inst_text)) == ESP_OK) {
                cfm_inst = (int16_t)strtol(inst_text, NULL, 10);
            }
        }
    }

    const uint8_t source_address = 0xF9;
    uint32_t can_id = 0;
    bool confirmed = false;
    int  retries = 0;
    esp_err_t ret = rvc_tx_confirmed(dgn, bytes, cfm_dgn, cfm_inst, 1,
                                     &confirmed, &retries, &can_id);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "RV-C transmit failed: %s", esp_err_to_name(ret));
        char err[96];
        snprintf(err, sizeof(err), "transmit failed: %s", esp_err_to_name(ret));
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err);
    }

    ESP_LOGI(TAG, "TX RV-C DGN 0x%05" PRIX32 " CAN ID 0x%08" PRIX32 " bytes %u %u %u %u %u %u %u %u",
             dgn, can_id, bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);

    char json[256];
    snprintf(json, sizeof(json),
             "{\"ok\":true,\"dgn\":\"0x%05" PRIX32 "\",\"can_id\":\"0x%08" PRIX32 "\",\"source_id\":%u,"
             "\"confirmed\":%s,\"retried\":%d}",
             dgn, can_id, (unsigned)source_address, confirmed ? "true" : "false", retries);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

// ── AP password endpoints (auth-lite) ──────────────────────────────────────
// Same URL and JSON shapes as the hub's, so the ported network page JS works
// unchanged — but pin_set/recovery_window are hard false here: this firmware
// has no PIN and no recovery window (see the store's comment above).

static bool request_is_on_ap(httpd_req_t *req)
{
    int fd = httpd_req_to_sockfd(req);
    if (fd < 0) return false;
    struct sockaddr_in6 addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(fd, (struct sockaddr *)&addr, &addr_len) != 0) return false;
    uint32_t ip4 = 0;
    if (addr.sin6_family == AF_INET) {
        ip4 = ((struct sockaddr_in *)&addr)->sin_addr.s_addr;
    } else if (addr.sin6_family == AF_INET6) {
        // LWIP maps IPv4 clients as ::FFFF:a.b.c.d
        const uint8_t *b = (const uint8_t *)&addr.sin6_addr;
        ip4 = (uint32_t)b[12] | ((uint32_t)b[13] << 8) | ((uint32_t)b[14] << 16) | ((uint32_t)b[15] << 24);
    }
    // AP subnet is 192.168.4.0/24 (WIFI_AP_IP_STRING).
    return (ip4 & 0x00FFFFFF) == ((192u) | (168u << 8) | (4u << 16));
}

static esp_err_t api_auth_status_get_handler(httpd_req_t *req)
{
    char json[192];
    snprintf(json, sizeof(json),
             "{\"ok\":true,\"pin_set\":false,\"trusted\":true,\"recovery_window\":false,"
             "\"ap_has_password\":%s,\"on_ap\":%s}",
             s_ap_password[0] != '\0' ? "true" : "false",
             request_is_on_ap(req) ? "true" : "false");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");
    return httpd_resp_sendstr(req, json);
}

// Reads a small POST body into buf (NUL-terminated). ESP_ERR_INVALID_SIZE if
// it doesn't fit.
static esp_err_t read_small_body(httpd_req_t *req, char *buf, size_t buf_len)
{
    if (req->content_len >= buf_len) return ESP_ERR_INVALID_SIZE;
    size_t received = 0;
    while (received < req->content_len) {
        int r = httpd_req_recv(req, buf + received, req->content_len - received);
        if (r <= 0) return ESP_FAIL;
        received += (size_t)r;
    }
    buf[received] = '\0';
    return ESP_OK;
}

// Minimal {"key":"value"} string extractor — handles \" and \\ escapes,
// which is all a Wi-Fi password can realistically need.
static bool json_extract_string(const char *body, const char *key, char *out, size_t out_len)
{
    if (body == NULL || key == NULL || out == NULL || out_len == 0) return false;
    out[0] = '\0';
    char pattern[48];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *k = strstr(body, pattern);
    if (k == NULL) return false;
    const char *p = strchr(k + strlen(pattern), ':');
    if (p == NULL) return false;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return false;
    p++;
    size_t pos = 0;
    while (*p != '\0' && *p != '"' && pos + 1 < out_len) {
        if (*p == '\\' && (p[1] == '"' || p[1] == '\\')) p++;
        out[pos++] = *p++;
    }
    if (*p != '"') return false;
    out[pos] = '\0';
    return true;
}

static esp_err_t api_auth_ap_password_post_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Connection", "close");

    char body[192] = {0};
    if (read_small_body(req, body, sizeof(body)) != ESP_OK) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"body read failed\"}");
    }

    char pw[65] = {0};
    if (!json_extract_string(body, "password", pw, sizeof(pw))) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"expected password\"}");
    }

    // Empty clears; otherwise WPA2 requires 8..63 characters.
    size_t n = strlen(pw);
    if (n != 0 && (n < 8 || n > 63)) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"Wi-Fi passwords need 8 to 63 characters\"}");
    }

    esp_err_t ret = save_ap_password(pw);
    if (ret != ESP_OK) {
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"NVS save failed\"}");
    }

    // Answer BEFORE bouncing the AP: re-applying the config drops every AP
    // client, including (possibly) the browser that asked.
    esp_err_t send_ret = httpd_resp_sendstr(req, "{\"ok\":true}");
    ESP_LOGW(TAG, "AP password %s — restarting the AP (clients will drop)",
             pw[0] != '\0' ? "set" : "removed");
    reapply_ap_config();
    return send_ret;
}

// ── Wi-Fi bring-up + HTTP server ───────────────────────────────────────────

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi AP client associated, AID=%d MAC=" MACSTR, event->aid, MAC2STR(event->mac));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi AP client disconnected, AID=%d MAC=" MACSTR, event->aid, MAC2STR(event->mac));
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
        // ASSOC_LEAVE (8) means we called esp_wifi_disconnect() intentionally
        // (e.g. before a user scan) — the scan handler triggers the reconnect.
        if (event->reason != WIFI_REASON_ASSOC_LEAVE) {
            if (event->reason == WIFI_REASON_AUTH_FAIL ||
                event->reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT) {
                ESP_LOGW(TAG, "Wi-Fi auth failure for %s — check password. Will retry with backoff.",
                         wifi_sta_ssid);
            }
            schedule_wifi_reconnect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        wifi_sta_connected = true;
        wifi_sta_got_ip = true;
        wifi_sta_retry_attempts = 0;
        snprintf(wifi_sta_ip, sizeof(wifi_sta_ip), IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Wi-Fi STA got IP: %s", wifi_sta_ip);
    }
}

static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 32;
    config.max_open_sockets = 6;
    config.stack_size = 10240;
    config.recv_wait_timeout = 3;
    config.send_wait_timeout = 1;
    config.backlog_conn = 2;

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return NULL;
    }

    web_server = server;

    static const httpd_uri_t root_uri            = { .uri = "/",                        .method = HTTP_GET,  .handler = root_get_handler };
    static const httpd_uri_t rvc_tools_uri       = { .uri = "/rvc-tools.html",          .method = HTTP_GET,  .handler = root_get_handler };
    static const httpd_uri_t network_uri         = { .uri = "/network.html",            .method = HTTP_GET,  .handler = network_get_handler };
    static const httpd_uri_t common_css_uri      = { .uri = "/common.css",              .method = HTTP_GET,  .handler = common_css_get_handler };
    static const httpd_uri_t inter_font_uri      = { .uri = "/inter.woff2",             .method = HTTP_GET,  .handler = inter_font_get_handler };
    static const httpd_uri_t webui_version_uri   = { .uri = "/webui-version.json",      .method = HTTP_GET,  .handler = webui_version_get_handler };
    static const httpd_uri_t api_state_uri       = { .uri = "/api/state",               .method = HTTP_GET,  .handler = api_state_get_handler };
    static const httpd_uri_t api_diag_reset_uri  = { .uri = "/api/diag/reset",          .method = HTTP_GET,  .handler = api_diag_reset_get_handler };
    static const httpd_uri_t api_rvclog_start_uri  = { .uri = "/api/rvclog/start",      .method = HTTP_GET,  .handler = api_rvclog_start_get_handler };
    static const httpd_uri_t api_rvclog_stop_uri   = { .uri = "/api/rvclog/stop",       .method = HTTP_GET,  .handler = api_rvclog_stop_get_handler };
    static const httpd_uri_t api_rvclog_mark_uri   = { .uri = "/api/rvclog/mark",       .method = HTTP_GET,  .handler = api_rvclog_mark_get_handler };
    static const httpd_uri_t api_rvclog_status_uri = { .uri = "/api/rvclog/status",     .method = HTTP_GET,  .handler = api_rvclog_status_get_handler };
    static const httpd_uri_t api_rvclog_download_uri = { .uri = "/api/rvclog/download", .method = HTTP_GET,  .handler = api_rvclog_download_get_handler };
    static const httpd_uri_t api_enable_uri      = { .uri = "/api/enable",              .method = HTTP_GET,  .handler = api_enable_get_handler };
    static const httpd_uri_t api_deselect_uri    = { .uri = "/api/deselect_all",        .method = HTTP_GET,  .handler = api_deselect_all_get_handler };
    static const httpd_uri_t api_select_uri      = { .uri = "/api/select_all",          .method = HTTP_GET,  .handler = api_select_all_get_handler };
    static const httpd_uri_t api_wifi_status_uri = { .uri = "/api/wifi/status",         .method = HTTP_GET,  .handler = api_wifi_status_get_handler };
    static const httpd_uri_t api_wifi_scan_uri   = { .uri = "/api/wifi/scan",           .method = HTTP_GET,  .handler = api_wifi_scan_get_handler };
    static const httpd_uri_t api_wifi_connect_uri = { .uri = "/api/wifi/connect",       .method = HTTP_GET,  .handler = api_wifi_connect_get_handler };
    static const httpd_uri_t api_wifi_networks_uri = { .uri = "/api/wifi/networks",     .method = HTTP_GET,  .handler = api_wifi_networks_get_handler };
    static const httpd_uri_t api_wifi_forget_uri = { .uri = "/api/wifi/networks/forget", .method = HTTP_GET, .handler = api_wifi_networks_forget_handler };
    static const httpd_uri_t api_wifi_move_uri   = { .uri = "/api/wifi/networks/move",  .method = HTTP_GET,  .handler = api_wifi_networks_move_handler };
    static const httpd_uri_t api_wifi_select_uri = { .uri = "/api/wifi/networks/select", .method = HTTP_GET, .handler = api_wifi_networks_select_handler };
    static const httpd_uri_t api_rvc_send_uri    = { .uri = "/api/rvc/send",            .method = HTTP_GET,  .handler = api_rvc_send_get_handler };
    static const httpd_uri_t api_auth_status_uri = { .uri = "/api/auth/status",         .method = HTTP_GET,  .handler = api_auth_status_get_handler };
    static const httpd_uri_t api_auth_appw_uri   = { .uri = "/api/auth/ap_password",    .method = HTTP_POST, .handler = api_auth_ap_password_post_handler };
    static const httpd_uri_t websocket_uri       = { .uri = "/ws",                      .method = HTTP_GET,  .handler = websocket_handler, .is_websocket = true };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &rvc_tools_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &network_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &common_css_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &inter_font_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webui_version_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_state_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_diag_reset_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvclog_start_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvclog_stop_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvclog_mark_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvclog_status_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvclog_download_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_enable_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_deselect_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_select_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_status_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_scan_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_connect_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_networks_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_forget_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_move_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_wifi_select_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_rvc_send_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_auth_status_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_auth_appw_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &websocket_uri));

    ESP_LOGI(TAG, "HTTP server started at http://%s/", WIFI_AP_IP_STRING);
    return server;
}

static void wifi_ap_init(void)
{
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

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_set_hostname(ap_netif, WIFI_NET_HOSTNAME));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_set_hostname(sta_netif, WIFI_NET_HOSTNAME));

    // "rvctools.local" — a name instead of an IP on either interface. Losing
    // name resolution is a convenience regression, not a boot failure.
    esp_err_t mdns_err = mdns_init();
    if (mdns_err == ESP_OK) {
        mdns_hostname_set(MDNS_HOSTNAME);
        mdns_instance_name_set("RV-C Tools");
        mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
        ESP_LOGI(TAG, "mDNS started: http://%s.local/", MDNS_HOSTNAME);
    } else {
        ESP_LOGW(TAG, "mDNS init failed: %s (IP address still works)", esp_err_to_name(mdns_err));
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

    // Uses the AP MAC — the suffix matches what a scanner reports as BSSID.
    build_ap_ssid();

    load_ap_password();

    wifi_config_t wifi_config = {0};
    size_t ap_ssid_len = strlen(s_ap_ssid);
    if (ap_ssid_len > sizeof(wifi_config.ap.ssid)) ap_ssid_len = sizeof(wifi_config.ap.ssid);
    memcpy(wifi_config.ap.ssid, s_ap_ssid, ap_ssid_len);
    wifi_config.ap.ssid_len = ap_ssid_len;
    wifi_config.ap.channel = WIFI_AP_CHANNEL;
    wifi_config.ap.max_connection = WIFI_AP_MAX_CLIENTS;
    wifi_config.ap.pmf_cfg.required = false;
    fill_ap_security(&wifi_config);

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    load_saved_networks_from_nvs();
    if (saved_networks.count > 0) {
        ESP_LOGI(TAG, "Loaded %d saved Wi-Fi network(s), connecting to best available…", saved_networks.count);
        wifi_sta_retry_attempts = 0;
        ESP_ERROR_CHECK_WITHOUT_ABORT(connect_best_saved_network());
    } else {
        ESP_LOGI(TAG, "No saved Wi-Fi networks in NVS");
    }

    // Periodic roam timer: scan for a higher-priority saved network + act as
    // the reconnect watchdog, every 5 minutes.
    esp_timer_create_args_t roam_timer_args = {
        .callback = wifi_roam_timer_cb,
        .name     = "wifi_roam",
    };
    ESP_ERROR_CHECK(esp_timer_create(&roam_timer_args, &wifi_roam_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(wifi_roam_timer,
        (uint64_t)WIFI_ROAM_CHECK_INTERVAL_MS * 1000ULL));

    // Disable modem sleep — AP responsiveness while serving the web UI.
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGI(TAG, "Wi-Fi AP+STA started");
    ESP_LOGI(TAG, "SSID: %s", s_ap_ssid);
    ESP_LOGI(TAG, "AP IP: %s", WIFI_AP_IP_STRING);

    spiffs_init();
    start_webserver();
}

esp_err_t rt_web_init(void)
{
    // One-shot boot diag reset — see boot_diag_reset_timer_cb.
    esp_timer_handle_t boot_diag_reset_timer = NULL;
    esp_timer_create_args_t boot_diag_reset_timer_args = {
        .callback = boot_diag_reset_timer_cb,
        .name     = "boot_diag_reset",
    };
    if (esp_timer_create(&boot_diag_reset_timer_args, &boot_diag_reset_timer) == ESP_OK) {
        esp_timer_start_once(boot_diag_reset_timer, 10ULL * 1000ULL * 1000ULL);
    }

    ws_client_mutex = xSemaphoreCreateMutex();
    if (ws_client_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket client mutex");
        return ESP_ERR_NO_MEM;
    }

    // Closed-loop transmit confirm. Failure is non-fatal: rvc_tx_confirmed
    // falls back to a plain unconfirmed transmit if either is NULL.
    s_cfm_sem  = xSemaphoreCreateBinary();
    s_cfm_lock = xSemaphoreCreateMutex();
    if (s_cfm_sem == NULL || s_cfm_lock == NULL) {
        ESP_LOGW(TAG, "Confirm watch unavailable (out of memory); commands will send unconfirmed");
    }

    ws_event_queue = xQueueCreate(WS_EVENT_QUEUE_LENGTH, sizeof(ws_event_t));
    if (ws_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket event queue");
        return ESP_ERR_NO_MEM;
    }

    wifi_ap_init();

    xTaskCreate(
        ws_delta_task,
        "ws_delta_task",
        6144,
        NULL,
        4,
        NULL
    );

    return ESP_OK;
}
