#pragma once

#include "esp_now.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t mac[ESP_NOW_ETH_ALEN];
  int8_t rssi;
  int64_t last_seen_us;
} espnow_device_info_t;

void espnow_devices_init(void);
size_t espnow_devices_snapshot(espnow_device_info_t *devices, size_t capacity);
