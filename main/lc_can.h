#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h"

#include "lc_types.h"

extern QueueHandle_t lc_can_rx_queue;

extern volatile uint32_t diag_rx_frames;
extern volatile uint32_t diag_rx_dropped_isr;
extern uint64_t diag_rx_busy_us;

esp_err_t lc_can_init(void);
bool lc_can_is_ready(void);
uint32_t lc_can_build_rvc_ext_id_from_dgn(uint32_t dgn, uint8_t source_address);
esp_err_t lc_can_transmit_rvc(uint32_t dgn, uint8_t source_address, const uint8_t *bytes, size_t len, uint32_t *out_can_id);
