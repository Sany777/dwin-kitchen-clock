#include "wifi_dwin.h"
 
// char buff[INET6_ADDRSTRLEN];
//             ip_addr_t const *ip = esp_sntp_getserver(i);
//             if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
//                 ESP_LOGI(TAG, "server %d: %s", i, buff);

void set_espnow_handler(void* arg, esp_event_base_t event_base,
                                int32_t action, void* event_data)
{
    static bool init_espnow;
    static bool run_espnow;
    static TaskHandle_t rx_espnow, tx_espnow;
switch(action){
    case START_ESPNOW :
    {
        if(!init_espnow){
            wifi_mode_t mode;
            esp_wifi_get_mode(&mode);
            if(mode != WIFI_MODE_STA){
                start_sta();
                vTaskDelay(1000);
            }
            ESP_ERROR_CHECK(esp_now_init());
            #if CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
                ESP_ERROR_CHECK(esp_now_set_wake_window(WINDOW_ESPNOW_MS));
            #endif
            ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)ESPNOW_PMK) );
            queue_espnow_tx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_send_t));
            queue_espnow_rx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_rx_data_t*));
            assert(queue_espnow_tx);
            assert(queue_espnow_rx);
            xTaskCreate(espnow_task_rx, "espnow_task_rx", 4000, (main_data_t *)arg, 2, rx_espnow);
            xTaskCreate(espnow_task_tx, "espnow_task_tx", 4000, (main_data_t *)arg, 2, tx_espnow);
            ESP_ERROR_CHECK( esp_now_register_send_cb(espnow_send_cb) );
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
            vTaskSuspend(tx_espnow);
            esp_now_unregister_recv_cb();
        }
        xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_RUN);
        break;
    }
    case  STOP_ESPNOW :
    {
        if(init_espnow){
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
            xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_RUN);
            xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_CONECT);
            init_espnow = false;
        }
        break;
    }
    default:break;
    }
}


void set_mode_wifi_handler(void* arg, esp_event_base_t event_base,
                                int32_t action, void* event_data)
{
    static wifi_config_t wifi_config;
    static esp_netif_t *netif = NULL;
    static wifi_mode_t mode = WIFI_MODE_NULL;
    static bool init_sta;
    main_data_t *main_data = (main_data_t *)arg;
switch(action){
    case INIT_AP :
    {
        if(mode == WIFI_MODE_AP)break;
        if(mode != WIFI_MODE_NULL)esp_wifi_stop();
        if(mode == WIFI_MODE_STA && netif){
            esp_netif_destroy_default_wifi(netif);
        }
        netif = esp_netif_create_default_wifi_ap();
        mode = WIFI_MODE_AP;
        memset(&wifi_config, 0, sizeof(wifi_config));
        wifi_config.ap.max_connection = MAX_STA_CONN;
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_config.ap.channel = ESP_WIFI_CHANNEL;
        wifi_config.ap.pmf_cfg.required = false;
        strcpy((char *)wifi_config.ap.ssid, AP_WIFI_SSID);
        strcpy((char *)wifi_config.ap.password, AP_WIFI_PWD);
        assert(netif);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &ap_handler, main_data);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &ap_handler, main_data);       
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &ap_handler, main_data);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, &ap_handler, main_data);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        break;
    }
    case INIT_SCAN_NETWORKS :
    {
        esp_wifi_set_event_mask(EVENT_OFF_WIFI);
        if(mode == WIFI_MODE_STA)break;
    }
    case START_STA :
    {
        if(!init_sta){
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, &wifi_sta_handler, main_data);
            esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_handler, main_data);
            esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_sta_handler, main_data);
            init_sta = true;
        }
        if(mode != WIFI_MODE_NULL)esp_wifi_stop();
        if(mode == WIFI_MODE_AP && netif){
            xEventGroupWaitBits(dwin_event_group, BIT_SERVER_STOP, false, false, SECOND_WAIT_WIFI_BIT);
            esp_netif_destroy_default_wifi(netif);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, &ap_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, &ap_handler);
        }
        memset(&wifi_config, 0, sizeof(wifi_config));
        strncpy((char *)wifi_config.sta.ssid, name_SSID, MAX_STR_LEN);
        strncpy((char *)wifi_config.sta.password, pwd_WIFI, MAX_STR_LEN);
        wifi_config.sta.sae_pwe_h2e = WIFI_AUTH_WPA2_PSK;
        mode = WIFI_MODE_STA;
        netif = esp_netif_create_default_wifi_sta();
        assert(netif);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
            ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
        #endif
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        break;
    }
    case  CLOSE_CUR_CON :
    {
        esp_wifi_stop();
        break;
    }
    case DEINIT_SCAN_NETWORKS :
    {
        ESP_ERROR_CHECK(esp_wifi_set_event_mask(EVENT_ON_WIFI));
        break;
    }
    case START_SCAN_NETWORKS :
    {
        ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
        vTaskDelay(DELAY_SCAN_SSID);
        ESP_ERROR_CHECK(esp_wifi_scan_stop());
        break;
    }
    
    default : break;
    }
}


void ap_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    main_data_t *main_data = (main_data_t*)arg;
    if(event_id == WIFI_EVENT_AP_START){
        show_server(NULL, "Start server");
        if(set_run_webserver(main_data) == ESP_OK){
            xEventGroupClearBits(dwin_event_group, BIT_SERVER_STOP);
        }
    } else if (event_id == WIFI_EVENT_AP_STOP){
        xEventGroupSetBits(dwin_event_group, BIT_SERVER_STOP);
        set_run_webserver(NULL);
        show_server(NULL, "Stop server");
    } else if(event_id == WIFI_EVENT_AP_STACONNECTED){
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        show_server(event->mac, "join.");
    } else if(event_id == WIFI_EVENT_AP_STADISCONNECTED){
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        show_server(event->mac, "leave.");
    }
}


void wifi_sta_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int retry_num;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        retry_num = 0;
        DWIN_SHOW_ERR(esp_wifi_connect());
    } else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP){
        retry_num = RETRY_CONNECT_APSTA;
        ESP_LOGI(TAG, "Sta stop!!!");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED && retry_num < RETRY_CONNECT_APSTA) {
        ESP_LOGI(TAG, "Atempt connecting sta...");
        if(esp_wifi_connect() != ESP_OK){
            if(retry_num == 0) xEventGroupClearBits(dwin_event_group, BIT_WIFI_STA);
            retry_num++;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        retry_num = 0;
        ESP_LOGI(TAG, "Connected STA success!!!");
        xEventGroupSetBits(dwin_event_group, BIT_WIFI_STA);

    }
}


void time_sync(struct timeval *tv)
{
    settimeofday(tv, FORMAT_CLOCK);
    esp_event_post_to(slow_service_loop, EVENTS_SET_TIME, UPDATE_TIME_FROM_ETHER, NULL, 0, WAIT_SERVICE);
}

void init_sntp_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)

{
    EventBits_t xEventGroup = xEventGroupWaitBits(dwin_event_group,                       
                                                          BIT_WIFI_STA, false, false,                               
                                                          FIRST_WAIT_WIFI_BIT);                                     
    if(!(xEventGroup&BIT_WIFI_STA)){                                                      
        start_sta();                                                                      
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_WIFI_STA, 
                                            false, false,                                                                     SECOND_WAIT_WIFI_BIT*3);                                                            
        if(!(xEventGroup&BIT_WIFI_STA))return;                                            
    }
    sntp_set_time_sync_notification_cb(time_sync);
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    ESP_LOGI(TAG, "SNTP start init !!!");
    sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_servermode_dhcp(0);
    sntp_init();
}