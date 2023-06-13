#pragma once 

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"

#include "dwin_common.h"

static const uint8_t BRODCAST_MAC[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define IS_BROADCAST_ADDR(addr)     (memcmp(addr, BRODCAST_MAC, SIZE_MAC) == 0)
#define IS_ALL_ADDR(addr)           ((addr[0] == 0)&&(addr[1])==0)


#define MAX_TIME_GETTING_DATA      (3000/portTICK_PERIOD_MS)
// #define SIZE_MAC                   6










#define SIZE_SENSOR_PACKAGE         (sizeof(sensor_package_t))
#define SIZE_HELLO_PACKAGE          (sizeof(hello_package_t))
#define SIZE_INFO_PACKAGE           (sizeof(device_info_package_t))
#define SIZE_ACTION_PACKAGE         (sizeof(action_package_t))
#define SIZE_TIME_PACKAGE           (sizeof(time_package_t))
#define SIZE_NETWORK_PACKAGE        (sizeof(network_package_t))
/*1-2sec*/
#define RANDOM_DELAY ((1000+esp_random()%1000)/portTICK_PERIOD_MS)
/*15 min*/
#define WAIT_NEXT_REQEST_ADD (900000/portTICK_PERIOD_MS)

#define TIMEOUT_RESPONSE_DEVICE_INFO ((MAX_ATEMPT_SEND_DATA)*TIMEOUT_SEND)

#define MAX_ATEMPT_REQUEST 10
#define MAX_ATEMPT_SEND_DATA 5
#define NUMBER_RESPONSE_ADD_NEW 5
#define NUMBER_REQUEST_ADD_NEW 7
#define TIMEOUT_SEND                (500/portTICK_PERIOD_MS)
#define WAIT_PUSH_ESPNOW_TO_QUEUE   (1000/portTICK_PERIOD_MS)
#define TIME_RESPONSE_ADD           (TIMEOUT_SEND*NUMBER_REQUEST_ADD_NEW)


void espnow_task_rx(void *pv);
void espnow_task_tx(void *pv);
void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void espnow_rx_cb(const esp_now_recv_info_t *recv_data, const uint8_t *data, int data_len);
static esp_err_t add_peer(uint8_t *mac_addr,  bool encrypt);

esp_err_t add_peer(uint8_t *mac_addr, bool encrypt);
esp_err_t modif_peer(uint8_t *mac_addr, bool encrypt);

