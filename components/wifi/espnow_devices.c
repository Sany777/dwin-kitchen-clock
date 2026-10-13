#include "espnow_devices.h"

#include "dwin_config.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include <stdbool.h>
#include <string.h>

static espnow_device_info_t devices[MAX_SLAVE_DEVICES];
static size_t device_count;
static portMUX_TYPE devices_lock = portMUX_INITIALIZER_UNLOCKED;
static bool espnow_initialized;

static void espnow_receive_callback(const esp_now_recv_info_t *info,
                                    const uint8_t *data, int data_len) {
  (void)data;
  (void)data_len;
  if (info == NULL || info->src_addr == NULL) {
    return;
  }

  const int64_t now_us = esp_timer_get_time();
  portENTER_CRITICAL(&devices_lock);
  size_t index = 0;
  while (index < device_count &&
         memcmp(devices[index].mac, info->src_addr, ESP_NOW_ETH_ALEN) != 0) {
    index++;
  }
  if (index == device_count) {
    if (device_count < MAX_SLAVE_DEVICES) {
      device_count++;
    } else {
      index = 0;
      for (size_t i = 1; i < device_count; i++) {
        if (devices[i].last_seen_us < devices[index].last_seen_us) {
          index = i;
        }
      }
    }
    memcpy(devices[index].mac, info->src_addr, ESP_NOW_ETH_ALEN);
  }
  devices[index].rssi = info->rx_ctrl != NULL ? info->rx_ctrl->rssi : 0;
  devices[index].last_seen_us = now_us;
  portEXIT_CRITICAL(&devices_lock);
}

void espnow_devices_init(void) {
  if (espnow_initialized) {
    return;
  }

  const esp_err_t init_result = esp_now_init();
  if (init_result != ESP_OK && init_result != ESP_ERR_ESPNOW_EXIST) {
    return;
  }
  if (esp_now_register_recv_cb(espnow_receive_callback) == ESP_OK) {
    espnow_initialized = true;
  }
}

size_t espnow_devices_snapshot(espnow_device_info_t *snapshot,
                               size_t capacity) {
  if (snapshot == NULL || capacity == 0) {
    return 0;
  }

  portENTER_CRITICAL(&devices_lock);
  const size_t count = device_count < capacity ? device_count : capacity;
  memcpy(snapshot, devices, count * sizeof(*snapshot));
  portEXIT_CRITICAL(&devices_lock);
  return count;
}
