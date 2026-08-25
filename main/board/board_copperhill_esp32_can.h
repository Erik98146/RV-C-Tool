#pragma once

// Copperhill / SK Pang ESP32 WiFi + BT + CAN bus module — ESP32-WROOM-32,
// 4 MB flash, no PSRAM, onboard 3.3 V CAN transceiver, micro-USB or 7-12 V
// supply. This is the board the LaunchControl hub originally ran on before
// its 2026-07-28 ESP32-S3 port; the pins below are the exact values the last
// working WROOM-32 build (git tag hw/wroom32-final) shipped with — verified
// on hardware, not copied from a datasheet.
//
// Single source of truth for every pin this project drives. lc_config.h
// includes this; nothing else touches a raw GPIO number.

#include "driver/gpio.h"

#define LC_BOARD_NAME "Copperhill ESP32 CAN module (ESP32-WROOM-32)"

#define CAN_TX_GPIO GPIO_NUM_25
#define CAN_RX_GPIO GPIO_NUM_26

// External diagnostic LED: GPIO2 -> 330R -> LED anode, cathode -> GND.
// Solid while valid RV-C frames are arriving, short heartbeat blink when the
// bus is quiet. Set to 0 if nothing is wired to GPIO2.
#define LC_HAS_DIAG_LED 1
#define LED_GPIO GPIO_NUM_2
