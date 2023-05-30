#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "time.h"
#include "dwin_server.h"


#include "dwin_common.h"


#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_RSSI -127
#define DEFAULT_AUTHMODE WIFI_AUTH_WPA2_PSK

#define RETRY_CONNECT_APSTA 5

#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       2
#define ESPNOW_PMK "pmk1234567890123"
#define ESPNOW_LMK "Lmk1234567890123"
#define TIMEOUT_SNTP 15


#define EVENT_OFF_WIFI      (WIFI_EVENT_STA_START \
                            | WIFI_EVENT_STA_STOP \
                            | WIFI_EVENT_STA_CONNECTED \
                            | WIFI_EVENT_STA_DISCONNECTED)

#define EVENT_ON_WIFI (WIFI_EVENT_WIFI_READY)


#define WAKE_INTERVAL 100

dwin_handler_t wifi_sta_handler;
dwin_handler_t wifi_ap_handler;
dwin_handler_t wifi_set_mode_handler;

dwin_handler_t server_handler;



void espnow_task_rx(void *pv);
void espnow_task_tx(void *pv);
void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void espnow_rx_cb(const esp_now_recv_info_t *recv_data, const uint8_t *data, int data_len);
static esp_err_t add_peer(uint8_t *mac_addr,  bool encrypt);

esp_err_t add_peer(uint8_t *mac_addr, bool encrypt);
esp_err_t modif_peer(uint8_t *mac_addr, bool encrypt);

void wifi_set_mode_handler(void* arg, esp_event_base_t event_base,
                                int32_t action, void* event_data);

void wifi_sta_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

void server_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);