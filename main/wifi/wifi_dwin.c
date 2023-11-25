#include "wifi_dwin.h"
 
void set_wifi(dwin_data_t* main_data, uint8_t action)
{
    static bool init_espnow;
    static bool run_espnow;
    static wifi_config_t wifi_config;
    static esp_netif_t *netif = NULL;
    static wifi_mode_t mode = WIFI_MODE_NULL;
    static bool init_sta;
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
switch(action){
    case START_ESPNOW :
    {
        if(!init_espnow){
            if(xEventGroup&BIT_DENIED_STA) return;
            wifi_mode_t mode;
            esp_wifi_get_mode(&mode);
            if(!rx_espnow )xTaskCreate(espnow_task_rx, "espnow_task_rx", 5000, main_data, 5, &rx_espnow);
            if(!tx_espnow)xTaskCreate(espnow_task_tx, "espnow_task_tx", 5000, main_data, 5, &tx_espnow);
            if(!rx_espnow || !tx_espnow)return;
            ESP_ERROR_CHECK(esp_now_init());
            ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );
            ESP_ERROR_CHECK( esp_now_register_send_cb(espnow_send_cb) );
            ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_rx_cb));
            #if CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
                ESP_ERROR_CHECK(esp_now_set_wake_window(WINDOW_ESPNOW_MS));
            #endif
            init_espnow = true;
            run_espnow = true;
        } else if(!run_espnow){
            run_espnow = true;
            vTaskResume(tx_espnow);
            esp_now_register_recv_cb(espnow_rx_cb);
        }
        xEventGroupSetBits(dwin_event_group, BIT_ESPNOW_RUN);
        break;
    }
    case PAUSE_ESPNOW :
    {
        if(init_espnow && run_espnow){
            run_espnow = false;
            vTaskSuspend(tx_espnow);
            esp_now_unregister_recv_cb();
        }
        break;
    }
    case  STOP_ESPNOW :
    {
        if(init_espnow){
            run_espnow = false;
            init_espnow = false;
            esp_now_unregister_recv_cb();
            esp_now_unregister_send_cb();
            vTaskDelay(1000);
            vTaskDelete(rx_espnow);
            rx_espnow = NULL;
            espnow_rx_data_t *data_rx;
            while(xQueueReceive(queue_espnow_rx, &data_rx, 0) == pdTRUE){
                if(data_rx){
                    if(data_rx->parcel){
                        free(data_rx->parcel);
                    }
                    free(data_rx);
                }
            }
            vQueueDelete(queue_espnow_rx);
            queue_espnow_rx = NULL;
            if(tx_espnow){
                vTaskDelete(tx_espnow);
                tx_espnow = NULL;
            } 
            if(queue_espnow_tx){
                vQueueDelete(queue_espnow_tx);
                queue_espnow_tx = NULL;
            }
            esp_now_deinit();
            xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_RUN|BIT_ESPNOW_CONECT);
        }
        break;
    }
    case INIT_AP :
    {
        if(mode != WIFI_MODE_AP){
            if(mode != WIFI_MODE_NULL){
                if(xEventGroup&BIT_ESPNOW_RUN){
                    set_new_command(STOP_ESPNOW);
                }
                esp_wifi_stop();
                vTaskDelay(400);
            }
            if(netif){
                esp_netif_destroy_default_wifi(netif);
                netif = NULL;  
            }
            netif = esp_netif_create_default_wifi_ap();
            if(!netif){
                set_new_command(MAIN_SCREEN);
                return;
            }
            mode = WIFI_MODE_AP;
            memset(&wifi_config, 0, sizeof(wifi_config));
            wifi_config.ap.max_connection = MAX_STA_CONN;
            wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
            wifi_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;
            wifi_config.ap.pmf_cfg.required = false;
            strcpy((char *)wifi_config.ap.ssid, CONFIG_AP_WIFI_SSID);
            strcpy((char *)wifi_config.ap.password, CONFIG_AP_WIFI_PWD);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &ap_handler, main_data);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &ap_handler, main_data);       
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &ap_handler, main_data);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, &ap_handler, main_data);
            ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
        }
        ESP_ERROR_CHECK(esp_wifi_start());
        break;
    }
    case START_STA :
    {
        if(xEventGroup&BIT_DENIED_STA) return;
        if(!init_sta){
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, &wifi_sta_handler, main_data);
            esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_handler, main_data);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_sta_handler, main_data);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP, &wifi_sta_handler, main_data);
            init_sta = true;
        } else {
            esp_wifi_stop();
        }
        if(mode == WIFI_MODE_AP){
            esp_netif_destroy_default_wifi(netif);
            netif = NULL;
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, &ap_handler);
        }
        if(netif == NULL)netif = esp_netif_create_default_wifi_sta();
        assert(netif);
        memset(&wifi_config, 0, sizeof(wifi_config));
        strncpy((char *)wifi_config.sta.ssid, name_SSID, MAX_STR_LEN);
        strncpy((char *)wifi_config.sta.password, pwd_WIFI, MAX_STR_LEN);
        wifi_config.sta.sae_pwe_h2e = WIFI_AUTH_WPA2_PSK;
        mode = WIFI_MODE_STA;
        DWIN_SHOW_ERR(esp_wifi_set_mode(WIFI_MODE_STA));
        #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
        #endif
        DWIN_SHOW_ERR(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        DWIN_SHOW_ERR(esp_wifi_start());
        if(xEventGroup&BIT_ESPNOW_ALLOW && !(xEventGroup&BIT_ESPNOW_RUN)){
            set_periodic_event(INIT_ESPNOW, 30, ONLY_ONCE);
        }
        break;
    }
    case  CLOSE_CUR_CON :
    {
        esp_wifi_stop();
        if(netif){
            esp_netif_destroy_default_wifi(netif);
            netif = NULL;
             if(mode == WIFI_MODE_AP){
                esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &ap_handler);
                esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &ap_handler);
                esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, &ap_handler);
                esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, &ap_handler);
            }
            mode = WIFI_MODE_NULL;
        }
        break;
    }
    
    default : break;
    }
}


void ap_handler(void* main_data, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{      
    if (event_id == WIFI_EVENT_AP_STOP){
        set_new_command(MAIN_SCREEN);
    } else if(event_id == WIFI_EVENT_AP_STACONNECTED){
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        show_screen(STATION_JOINE, event->mac, SIZE_MAC);
        set_new_command(STATION_JOINE);
    } else if(event_id == WIFI_EVENT_AP_STADISCONNECTED){
        set_new_command(MAIN_SCREEN);
    }
}
#include "freertos/event_groups.h"

void wifi_sta_handler(void* data, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int retry_num;
    EventBits_t xEventGroup = xEventGroupGetBitsFromISR(dwin_event_group);
    BaseType_t pxHigherPriorityTaskWoken;
    if(!(xEventGroup&BIT_DENIED_STA)){
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
            retry_num = 0;
            esp_wifi_connect();
        } else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP){
            retry_num = RETRY_CONNECT_APSTA;
        } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED && retry_num < RETRY_CONNECT_APSTA) {
            if(esp_wifi_connect() != ESP_OK){
                if(retry_num == RETRY_CONNECT_APSTA){
                    if(xEventGroup&BIT_CON_STA_OK) {
                        xEventGroupClearBitsFromISR(dwin_event_group, BIT_CON_STA_OK);
                    }
                    wifi_event_sta_disconnected_t *event_sta_disconnected = (wifi_event_sta_disconnected_t *) event_data;
                    if(event_sta_disconnected->reason == WIFI_REASON_NO_AP_FOUND
                        || event_sta_disconnected->reason == WIFI_REASON_HANDSHAKE_TIMEOUT)
                    {
                        if(xEventGroup&BIT_SSID_FOUND){
                            xEventGroupClearBitsFromISR(dwin_event_group, BIT_SSID_FOUND);
                        }
                    } else {
                        xEventGroupSetBitsFromISR(dwin_event_group, BIT_SSID_FOUND, &pxHigherPriorityTaskWoken);
                    }
                    xEventGroupClearBitsFromISR(dwin_event_group, BIT_PROCESS);
                }
                retry_num++;
            }
        } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            dwin_data_t*main_data = (dwin_data_t*) data;
            retry_num = 0;
            xEventGroupSetBitsFromISR(dwin_event_group, BIT_CON_STA_OK|BIT_SSID_FOUND, 
            &pxHigherPriorityTaskWoken);
            xEventGroupClearBitsFromISR(dwin_event_group, BIT_PROCESS);
            if(!(xEventGroup&BIT_WEATHER_OK)){
                set_new_command(GET_WEATHER);
            }
            if(usd_Sale == DWIN_NO_DATA)set_periodic_event(UPDATE_CURRENCY, 10, RELOAD_COUNT);
            if(xEventGroup&BIT_SNTP_ALLOW && !(xEventGroup&BIT_IS_TIME)){
                set_new_command(INIT_SNTP);
            }
        }
    } else {
        xEventGroupClearBitsFromISR(dwin_event_group, BIT_CON_STA_OK);
    }
}


void set_time_cb(struct timeval *tv)
{
    set_time_tv(tv);
    set_dwin_clock();
    if(esp_sntp_get_sync_interval() < SYNCH_10_HOUR){
        esp_sntp_set_sync_interval(SYNCH_10_HOUR);
        xEventGroupSetBits(dwin_event_group, BIT_SNTP_WORK);
    }
}

void set_sntp(dwin_data_t* main_data, uint8_t action)
{
    if(action == INIT_SNTP){
        if(!esp_sntp_enabled()){
            esp_sntp_set_time_sync_notification_cb(set_time_cb);
            esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
            esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
            esp_sntp_setservername(0, "pool.ntp.org");
            esp_sntp_setservername(1, "time.windows.com");
            sntp_servermode_dhcp(0);
            esp_sntp_set_sync_interval(SYNC_15_MIN);
            esp_sntp_init();
        } else {
            esp_sntp_restart();
        }
    } else if(action == STOP_SNTP && esp_sntp_enabled()){
        esp_sntp_stop();
        xEventGroupClearBits(dwin_event_group, BIT_SNTP_WORK);
    }
}