#include "wifi_dwin.h"

QueueHandle_t queue_espnow_tx, queue_espnow_rx;
EventGroupHandle_t dwin_event_group;

void wifi_set_mode_handler(void* arg, esp_event_base_t event_base,
                                int32_t action, void* event_data)
{
    static wifi_config_t wifi_config;
    static esp_netif_t *netif = NULL;
    static wifi_mode_t mode = WIFI_MODE_NULL;
    static TaskHandle_t rx_espnow, tx_espnow;
    main_data_t *main_data = (main_data_t *)arg;
switch(action){
    case INIT_AP :
    {
        if(mode == WIFI_MODE_AP)break;
        esp_wifi_stop();
        memset(&wifi_config, 0, sizeof(wifi_config));
        if(mode == WIFI_MODE_STA && netif){
            esp_netif_destroy_default_wifi(netif);
        }
        netif = esp_netif_create_default_wifi_ap();
        mode = WIFI_MODE_AP;
        wifi_config.ap.max_connection = MAX_STA_CONN;
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_config.ap.channel = ESP_WIFI_CHANNEL;
        wifi_config.ap.pmf_cfg.required = false;
        strcpy((char *)wifi_config.ap.ssid, AP_WIFI_SSID);
        strcpy((char *)wifi_config.ap.password, AP_WIFI_PWD);
        assert(netif);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &server_handler, main_data);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &server_handler, main_data);       
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &server_handler, main_data);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, &server_handler, main_data);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        break;
    }
    case INIT_STA :
    {
        ESP_LOGI(TAG, "Init STA");
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, &wifi_sta_handler, main_data);
        esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_handler, main_data);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_sta_handler, main_data);
    }
    case START_STA :
    {
        esp_wifi_stop();
        memset(&wifi_config, 0, sizeof(wifi_config));
        strncpy((char *)wifi_config.sta.ssid, name_SSID, MAX_STR_LEN);
        strncpy((char *)wifi_config.sta.password, pwd_WIFI, MAX_STR_LEN);
        wifi_config.sta.sae_pwe_h2e = WIFI_AUTH_WPA2_PSK;
        if(mode == WIFI_MODE_AP && netif){
            esp_netif_destroy_default_wifi(netif);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &server_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &server_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, &server_handler);
            esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, &server_handler);
        }
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
    case INIT_SCAN_NETWORKS :
    {
        esp_wifi_set_event_mask(EVENT_OFF_WIFI);
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
    case INIT_ESPNOW :
    {
        if(mode == WIFI_MODE_STA){
            ESP_ERROR_CHECK(esp_now_init() );
            #if CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
                ESP_ERROR_CHECK(esp_now_set_wake_window(WINDOW_ESPNOW_MS));
            #endif
            ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)ESPNOW_PMK) );
            queue_espnow_tx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_send_t));
            queue_espnow_rx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_rx_data_t*));
            assert(queue_espnow_tx);
            assert(queue_espnow_rx);
            xTaskCreate(espnow_task_rx, "espnow_task_rx", 4000, main_data, 2, rx_espnow);
            xTaskCreate(espnow_task_tx, "espnow_task_tx", 4000, main_data, 2, tx_espnow);
            // xEventGroupSetBits(dwin_event_group, BIT_INIT_ESPNOW);
        }
        break;
    }
    case START_ESPNOW :
    {
        ESP_ERROR_CHECK( esp_now_register_send_cb(espnow_send_cb) );
        ESP_ERROR_CHECK( esp_now_register_recv_cb(espnow_rx_cb) );
        xEventGroupSetBits(dwin_event_group, BIT_ESPNOW_RUN);
        break;
    }
    case PAUSE_ESPNOW :
    {
        if(tx_espnow)vTaskSuspend(tx_espnow);
        esp_now_unregister_recv_cb();
        break;
    }
    case  STOP_ESPNOW :
    {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        vTaskDelay(1000);
        if(rx_espnow){
            vTaskDelete(rx_espnow);
            rx_espnow = NULL;
        }
        if(queue_espnow_rx){
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
        }
        if(tx_espnow){
            vTaskDelete(tx_espnow);
            tx_espnow = NULL;
        } 
        if(queue_espnow_tx){
            vQueueDelete(queue_espnow_tx);
            queue_espnow_tx = NULL;
        }
        esp_now_deinit();
        // xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_RUN);
        // xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_CONECT);
        // xEventGroupClearBits(dwin_event_group, BIT_INIT_ESPNOW);
        break;
    }
    case  INIT_SNTP :
    {
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_servermode_dhcp(0);
        sntp_init();
        // setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
        // tzset();
    }
    case UPDATE_TIME :
    {
        if(mode != WIFI_MODE_STA)break;
        int retry = 0;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < TIMEOUT_SNTP) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        if(retry < TIMEOUT_SNTP) {
            esp_event_post_to(loop_direct, EVENTS_SET_TIME, UPDATE_TIME_FROM_SNTP, NULL, 0, TIMEOUT_SEND_EVENTS);
        }
        break;
    }
    default : break;
    }
}


void server_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    main_data_t *main_data = (main_data_t*)arg;
    if(event_id == WIFI_EVENT_AP_START){
        show_server(NULL, "Start server");
        if(set_run_webserver(main_data) == ESP_OK){
            xEventGroupSetBits(dwin_event_group, BIT_SERVER_RUN);
        }
    } else if (event_id == WIFI_EVENT_AP_STOP){
        set_run_webserver(NULL);
        xEventGroupClearBits(dwin_event_group, BIT_SERVER_RUN);
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
        ESP_LOGI(TAG, "Connect sta...");
            if(esp_wifi_connect() != ESP_OK){
                if(retry_num == 0) xEventGroupClearBits(dwin_event_group, BIT_WIFI_STA);
                retry_num++;
            }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        retry_num = 0;
        ESP_LOGI(TAG, "Connected STA success!!!");
        xEventGroupSetBits(dwin_event_group, BIT_WIFI_STA);
        xEventGroupClearBits(dwin_event_group, BIT_PROCESS_STA);
    }
}




void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if(!mac_addr)return;
    xTaskNotify(espnow_task_tx, status, eSetValueWithOverwrite);
}

void espnow_rx_cb(const esp_now_recv_info_t *recv_data, const uint8_t *data, int data_len)
{
    if(recv_data && recv_data->src_addr
            && recv_data->des_addr
            && data
    ){
        espnow_rx_data_t *data_rx = (espnow_rx_data_t*) malloc(sizeof(espnow_rx_data_t));
        if(data_rx){
            data_rx->parcel = malloc(data_len);
            if(data_rx->parcel){
                data_rx->is_brodcast_dst = IS_BROADCAST_ADDR(recv_data->des_addr);
                data_rx->rssi = recv_data->rx_ctrl->rssi;
                memcpy(data_rx->parcel, data, data_len);
                memcpy(data_rx->mac, recv_data->src_addr, SIZE_MAC);
                data_rx->parcel_len  = data_len;
                if(xQueueSend(queue_espnow_rx, data_rx, 100) == pdTRUE) {
                    return;
                }
                free(data_rx->parcel);
            }
            free(data_rx);
        }
    }
}

void remove_espnow_device(const uint8_t *mac_addr)
{
    if(mac_addr != NULL){
        esp_now_del_peer(mac_addr);
        delete_device_info(mac_addr);
    }
}


void espnow_task_tx(void *pv)
{
    main_data_t * const main_data = (main_data_t *)pv;
    const uint8_t *parcel = NULL, *mac;
    uint8_t max_atempt;
    size_t parcel_len = 0;
    espnow_send_t data_to_send = {
        .action = REQUEST_ADD_NEW,
    };
    ESP_ERROR_CHECK(add_peer(BRODCAST_MAC, false));
    uint32_t res;
    bool send_ok;
    size_t send_count;
    vTaskDelay(pdMS_TO_TICKS(10000));
for(;;){
    if(parcel){
        free(parcel);
        parcel = NULL;
    }
    send_ok = false;
    send_count = max_atempt = 0;
    if(data_to_send.action == REQUEST_ADD_NEW || data_to_send.action == RESPONSE_ADD_NEW){
        mac = BRODCAST_MAC;
    } else {
        mac = IS_ALL_ADDR(data_to_send.mac) ? NULL : data_to_send.mac;
    }
    switch(data_to_send.action){
        case RESPONSE_ADD_NEW : max_atempt = NUMBER_RESPONSE_ADD_NEW;
        case REQUEST_ADD_NEW : max_atempt = NUMBER_REQUEST_ADD_NEW;
        {
            hello_package_t *hello_package = malloc(SIZE_HELLO_PACKAGE);
            parcel_len = SIZE_HELLO_PACKAGE;
            if(hello_package){
                parcel = hello_package;
                hello_package->action = data_to_send.action;
            }
            break;
        }
        case NEED_TIME :
        case NEED_NETWORK :
        case NEED_DEVICE_INFO :
        {
            action_package_t *action_package = malloc(SIZE_ACTION_PACKAGE);
            if(action_package){
                max_atempt = MAX_ATEMPT_REQUEST;
                parcel = action_package;
                parcel_len = SIZE_ACTION_PACKAGE;
                action_package->action = data_to_send.action;
                action_package->crc = 0;
                action_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
            }
            break;
        }
        case GIVE_NETWORK :
        {
            size_t size_pwd = strnlen(pwd_WIFI, MAX_STR_LEN);
            size_t size_ssid = strnlen(name_SSID, MAX_STR_LEN);
            network_package_t *package = malloc(SIZE_NETWORK_PACKAGE);
            if(package){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_NETWORK_PACKAGE;
                parcel = package;
                memcpy(package->pwd, pwd_WIFI, size_pwd);
                package->pwd[size_pwd] = '\0';
                memcpy(package->ssid, name_SSID, size_ssid);
                package->ssid[size_ssid] = '\0';
                package->crc = 0;
                package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
            }
            break;
        }
        case GIVE_DIVICE_INFO :
        {
            device_inf_t *my_device = get_my_device();
            if(my_device){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                device_info_package_t *device_info_package = malloc(SIZE_INFO_PACKAGE);
                if(device_info_package){
                    size_t name_len = strnlen(my_device->name, MAX_NAME_DEVICE);
                    memcpy(device_info_package->name, my_device->name, name_len);
                    memcpy(device_info_package->mac, my_device->mac, SIZE_MAC);
                    parcel_len = SIZE_INFO_PACKAGE;
                    parcel = device_info_package;
                    device_info_package->type = MY_DEVICE_TYPE;
                    device_info_package->crc = 0;
                    device_info_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
                    device_info_package->name[name_len] = '\0';
                }
            }
            break;
        }
        case GIVE_TIME :
        {
            time_package_t *time_package = malloc(SIZE_TIME_PACKAGE);
            if(time_package){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_TIME_PACKAGE;
                parcel = time_package;
                time(&time_package->time);
                time_package->crc = 0;
                time_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
            }
            break;
        }
        default : break;
    }
    while(parcel && send_count++ < max_atempt){
        if(esp_now_send(mac, (const uint8_t*)parcel, parcel_len) != ESP_OK){
            break;
        }
        if(xTaskNotifyWait(0x00, 0xff, &res, (send_count+1)*TIMEOUT_SEND) == pdTRUE
                && res == ESP_NOW_SEND_SUCCESS){
            send_ok = true;
            break;
        }
    }
    if(data_to_send.action == RESPONSE_ADD_NEW){
        if(send_ok){
            vTaskDelay(RANDOM_DELAY);
            data_to_send.action = NEED_DEVICE_INFO;
            continue;
        } else {
            remove_espnow_device(data_to_send.mac);
        }
    }else if(data_to_send.action == NEED_DEVICE_INFO){
        if(!send_ok || xTaskNotifyWait(0x00, 0xff, &res, TIMEOUT_RESPONSE_DEVICE_INFO) != pdTRUE){
            remove_espnow_device(data_to_send.mac);
        }
    }
    if(xQueueReceive(queue_espnow_tx, &data_to_send,
                                send_ok
                                ? portMAX_DELAY 
                                : WAIT_NEXT_REQEST_ADD) != pdTRUE 
                            && send_ok){
        data_to_send.action = NOPE;
    }
}
}


void espnow_task_rx(void *pv)
{
    main_data_t * const main_data = (main_data_t *)pv;
    /*parcel data*/
    uint8_t rssi;
    uint16_t crc;
    espnow_rx_data_t *data_rx;
    espnow_send_t data_tx;

for(;;){
    data_tx.action = NOPE;
if(xQueueReceive(queue_espnow_rx, &data_rx, portMAX_DELAY) == pdTRUE){
    if(data_rx->is_brodcast_dst){
        if(data_rx->parcel_len == SIZE_HELLO_PACKAGE){
            hello_package_t *hello_package = (hello_package_t *)data_rx->parcel;
            if(hello_package->action == REQUEST_ADD_NEW){
                data_tx.action = RESPONSE_ADD_NEW;
                add_peer(data_rx->mac, true);
            }else if(hello_package->action == RESPONSE_ADD_NEW
                    && !esp_now_is_peer_exist(data_rx->mac)){
                data_tx.action = NEED_DEVICE_INFO;
                add_peer(data_rx->mac, true);
            }
        }
    } else {
        switch(data_rx->parcel_len){
        case SIZE_SENSOR_PACKAGE :
        {
            
            break;
        }
        case SIZE_INFO_PACKAGE :
        {
            device_info_package_t *device_info_package = data_rx->parcel;
            crc = device_info_package->crc;
            device_info_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)device_info_package, SIZE_INFO_PACKAGE) 
                    && memcmp(device_info_package->mac, data_rx->mac, SIZE_MAC) == 0){
                xTaskNotify(espnow_task_tx, ESP_OK, eSetValueWithOverwrite);
                if(get_device_before(data_rx->mac) == NULL){
                    device_inf_t *new_device = create_device_info(device_info_package->type, 
                                                    data_rx->mac,
                                                    device_info_package->name);
                    add_device_inf_to_list(new_device);
                }
            }
            break;
        }
        case SIZE_ACTION_PACKAGE :
        {
            action_package_t *action_package = data_rx->parcel;
            crc = action_package->crc;
            action_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)action_package, SIZE_ACTION_PACKAGE)){
                if(action_package->action == NEED_TIME){
                    data_tx.action = GIVE_TIME;
                }else if(action_package->action == NEED_NETWORK){
                    data_tx.action = GIVE_NETWORK;
                }else if(action_package->action == NEED_DEVICE_INFO){
                    data_tx.action = GIVE_DIVICE_INFO;
                } else {
                    break;
                }
            }
            break;
        }
        case SIZE_TIME_PACKAGE :
        {
            time_package_t *time_package = data_rx->parcel;
            crc = time_package->crc;
            time_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)time_package, SIZE_TIME_PACKAGE)){
                static  time_t time;
                time = time_package->time;
                esp_event_post_to(loop_direct, EVENTS_SET_TIME, UPDATE_TIME_FROM_MS, &time, sizeof(time_t), TIMEOUT_SEND_EVENTS);
            }
            break;
        }
        case SIZE_NETWORK_PACKAGE :
        {
            network_package_t *network_package = data_rx->parcel;
            crc = network_package->crc;
            network_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)network_package, SIZE_NETWORK_PACKAGE)){
                memcpy(name_SSID, network_package->ssid, SIZE_BUF);
                memcpy(pwd_WIFI, network_package->pwd, SIZE_BUF);
            }
            break;
        }
        default:break;
        }
    }
    if(data_tx.action != NOPE){
        memcpy(data_tx.mac, data_rx->mac, SIZE_MAC);
        xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
    }
    if(data_rx){
        if(data_rx->parcel){
            free(data_rx->parcel);
        }
        free(data_rx);
        data_rx = NULL;
    }
    }
}
}

esp_now_peer_info_t *get_peer(uint8_t *mac_addr, bool encrypt)
{
    static esp_now_peer_info_t peer;
    peer.channel = ESP_WIFI_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = encrypt;
    memcpy(peer.lmk, ESPNOW_LMK, ESP_NOW_KEY_LEN);
    memcpy(peer.peer_addr, mac_addr, SIZE_MAC);
    return &peer;
}

esp_err_t add_peer(uint8_t *mac_addr, bool encrypt)
{
    return esp_now_add_peer(get_peer(mac_addr, encrypt));
}

esp_err_t modif_peer(uint8_t *mac_addr, bool encrypt)
{
    return esp_now_mod_peer(get_peer(mac_addr, encrypt));
}


//esp_register_shutdown_handler()

//base_mac
// esp_read_mac()
// esp_iface_mac_addr_set()
// esp_chip_data()
// esp_reset_reason()