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

#define MAX_STA_CONN       2
#define ESPNOW_PMK "rasmk096519qrfac"
#define ESPNOW_LMK "kfns9420147fhsae"
#define TIMEOUT_SNTP 5


#define EVENT_OFF_WIFI      (WIFI_EVENT_STA_START \
                            | WIFI_EVENT_STA_STOP \
                            | WIFI_EVENT_STA_CONNECTED \
                            | WIFI_EVENT_STA_DISCONNECTED)

#define EVENT_ON_WIFI (WIFI_EVENT_WIFI_READY)


#define WAKE_INTERVAL 100




void set_wifi(main_data_t* main_data, uint8_t action);
void wifi_sta_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
void ap_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
void set_sntp(main_data_t* main_data, uint8_t action);
void set_time_tv(struct timeval *tv);

/*
#define BREAK_IF_NO_WIFI_CON()                                                      \
    do{                                                                             \
        EventBits_t xEventGroup = xEventGroupWaitBits(dwin_event_group,             \
                        BIT_CON_STA_OK,                                             \
                        false, false,                                               \
                        FIRST_WAIT_WIFI_BIT);                                       \                               
        if(!(xEventGroup&BIT_CON_STA_OK)){                                          \                                                                  
            start_sta();                                                            \                                                      
            xEventGroup =                                                           \
                        xEventGroupWaitBits(dwin_event_group, BIT_CON_STA_OK,       \
                                            false, false, WAIT_PROCEES*2);          \                                                      
            if(!(xEventGroup&BIT_CON_STA_OK))return;                                \                      
        }                                                                           \
    }while(0)*/