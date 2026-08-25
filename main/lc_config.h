#pragma once

// Stand-alone RV-C Tools — non-hardware configuration constants. Pins live in
// the board header. Trimmed from hub_core's lc_config.h: everything the hub
// needed for cards/publish/MQTT/BLE/displays is gone; what remains is exactly
// what the sniffer, the frame logger, the transmitter and the Wi-Fi manager
// consume. Names are kept identical to the hub's so the copied modules
// (lc_can.c, lc_state.c, main.c decode pipeline) compile unchanged.

#include "board/board_copperhill_esp32_can.h"

#define TAG "RVC_TOOLS"

// Keep CAN receive buffering modest on ESP32 so Wi-Fi + HTTP can still
// allocate heap. At 250 kbps, 64 frames is enough burst protection.
#define RX_QUEUE_LENGTH 64

// Compact activity-view slots for the discovery/sniffer "what's on the bus"
// list — allocated on first sighting with oldest-eviction. 64 × 40 B ≈ 2.6 KB.
#define MAX_ACTIVE_DGNS 64
// Live state slots, keyed by (DGN + instance + source). The hub measured a
// real coach needing 81 tuples (2026-08-11), so 96 — the table never evicts
// and fails silently when full. ~15 KB BSS, scanned per CAN frame.
#define MAX_TRACKED_STATES 96
#define FIELD_VALUE_STRING_MAX 96
#define SELECT_ALL_MODE_MAX_TRACKED_STATES 48

// LED behavior (LC_HAS_DIAG_LED in the board header).
#define VALID_RX_HOLD_MS      500
#define HEARTBEAT_ON_MS       40
#define HEARTBEAT_PERIOD_MS   1000

// Wi-Fi softAP. The prefix gets the last two MAC bytes appended at runtime
// ("RVC-Tools-A3F2") — same scheme as the hub, so two units on one bench are
// distinguishable. mDNS name is deliberately short and unsuffixed.
#define WIFI_AP_SSID_PREFIX   "RVC-Tools"
#define WIFI_AP_SSID_MAX      33   // 32 + NUL
#define WIFI_NET_HOSTNAME     "RVC-Tools"
#define MDNS_HOSTNAME         "rvctools"
#define WIFI_AP_CHANNEL       6
#define WIFI_AP_MAX_CLIENTS   4
#define WIFI_AP_IP_STRING     "192.168.4.1"
#define WIFI_STA_MAX_SCAN_RESULTS   16
#define WIFI_MAX_SAVED_NETWORKS     8
#define WIFI_RECONNECT_DELAY_MS           5000
#define WIFI_RECONNECT_DELAY_LONG_MS      30000
#define WIFI_RECONNECT_BACKOFF_AFTER      3
#define WIFI_ROAM_CHECK_INTERVAL_MS       (5 * 60 * 1000)
#define WIFI_SCAN_DISCONNECT_SETTLE_MS 350

// WebSocket delta-stream settings (/ws — the sniffer page's live feed).
#define MAX_WS_CLIENTS         2
#define WS_EVENT_QUEUE_LENGTH  16
#define WS_EVENT_MAX_BYTES     512

// Discovery/unknown-DGN reporting settings
#define DISCOVERY_EVENT_MIN_US 1000000
#define MAX_UNKNOWN_ACTIVE_DGNS 64
#define MAX_DISCOVERY_ACTIVE_INSTANCES 96

// The browser uses /api/state polling as the reliable data path; the /ws
// push stream is the fast path on top of it.
#define ENABLE_WS_PUSH_EVENTS 1
#define ENABLE_SERIAL_CHANGE_LOG 0
