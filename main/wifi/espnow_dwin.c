#include "espnow_dwin.h"

QueueHandle_t queue_espnow_tx = NULL, queue_espnow_rx = NULL;
TaskHandle_t rx_espnow = NULL, tx_espnow = NULL;


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
    uint8_t *parcel = NULL, *mac;
    uint8_t max_atempt;
    size_t parcel_len = 0;
    espnow_send_t data_to_send = {
        .action = REQUEST_ADD_NEW,
    };
    uint32_t res;
    bool send_ok;
    size_t send_count;
    queue_espnow_tx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_send_t));
    assert(queue_espnow_tx);
    vTaskDelay(DEALAY_START_TASK*5);
    add_peer(BRODCAST_MAC, false);
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
    parcel_len = SIZE_HELLO_PACKAGE;
    switch(data_to_send.action){
        case RESPONSE_ADD_NEW : max_atempt = NUMBER_RESPONSE_ADD_NEW;
            ESP_LOGI(TAG, "response ");
        case REQUEST_ADD_NEW : max_atempt = NUMBER_REQUEST_ADD_NEW;
        {
            hello_package_t *hello_package = malloc(SIZE_HELLO_PACKAGE);
            if(hello_package){
                ESP_LOGI(TAG, "Create hello_package");
                parcel_len = SIZE_HELLO_PACKAGE;
                parcel = (uint8_t *)hello_package;
                hello_package->action = data_to_send.action;
            }
            break;
        }
        case NEED_TIME :
        case NEED_NETWORK :
        case NEED_DEVICE_INFO :
        {
            ESP_LOGI(TAG, "Create action_package");
            action_package_t *action_package = malloc(SIZE_ACTION_PACKAGE);
            if(action_package){
                max_atempt = MAX_ATEMPT_REQUEST;
                parcel = (uint8_t *)action_package;
                parcel_len = SIZE_ACTION_PACKAGE;
                action_package->action = data_to_send.action;
                action_package->crc = 0;
                action_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
            }
            break;
        }
        case GIVE_NETWORK :
        {
            ESP_LOGI(TAG, "Create net package");
            network_package_t *network_package = malloc(SIZE_NETWORK_PACKAGE);
            if(network_package){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_NETWORK_PACKAGE;
                parcel = (uint8_t *)network_package;
                strncpy(network_package->pwd, pwd_WIFI, MAX_STR_LEN);
                strncpy(network_package->ssid, name_SSID, MAX_STR_LEN);
                network_package->crc = 0;
                network_package->crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
            }
            break;
        }
        case GIVE_DIVICE_INFO :
        {
            ESP_LOGI(TAG, "Create device_info_package");
            device_inf_t *my_device = get_my_device();
            if(my_device){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                device_info_package_t *device_info_package = malloc(SIZE_INFO_PACKAGE);
                if(device_info_package){
                    strncpy(device_info_package->name, my_device->name, MAX_NAME_DEVICE);
                    memcpy(device_info_package->mac, my_device->mac, SIZE_MAC);
                    parcel_len = SIZE_INFO_PACKAGE;
                    parcel = (uint8_t *)device_info_package;
                    device_info_package->type = MY_DEVICE_TYPE;
                    device_info_package->crc = 0;
                    device_info_package->crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
                }
            }
            break;
        }
        case GIVE_TIME :
        {
            ESP_LOGI(TAG, "Create time_package");
            time_package_t *time_package = malloc(SIZE_TIME_PACKAGE);
            if(time_package){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_TIME_PACKAGE;
                parcel = (uint8_t *)time_package;
                time(&time_package->time);
                time_package->crc = 0;
                time_package->crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
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
                && res == ESP_NOW_SEND_SUCCESS)
        {
            show_server(mac, "sending saccess");
            ESP_LOGI(TAG, " data len : %d", (int)parcel_len);
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

void get_data_espnow(void* args, esp_event_base_t base, int32_t key, void* event_data)
{
    device_inf_t *new_device = (device_inf_t *)event_data;
    ESP_LOGI(TAG, "NEW device with name %s", new_device->name);
    espnow_send_t data_tx;
    memcpy(data_tx.mac, new_device->mac, SIZE_MAC);
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(!(xEventGroup&BIT_WIFI_STA)){
        data_tx.action = NEED_NETWORK;
        xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_WIFI_STA, false, false, 10000/portTICK_PERIOD_MS);
        if(!(xEventGroup&BIT_WIFI_STA) && !(xEventGroup&BIT_IS_TIME)){
            data_tx.action = NEED_TIME;
            xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
        }
    }
    if(new_device->type == SENSOR_TEMP_DEVICE){
        data_tx.action = NEED_TEMP;
        xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
    }
}



void espnow_task_rx(void *pv)
{
    main_data_t * const main_data = (main_data_t *)pv;
    /*parcel data*/
    uint8_t rssi;
    uint16_t crc;
    espnow_rx_data_t data_rx;
    espnow_send_t data_tx;
    vTaskDelay(DEALAY_START_TASK*5);
    queue_espnow_rx = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_rx_data_t));
    assert(queue_espnow_rx);
    ESP_LOGI(TAG, "void espnow_task_rx(void *pv)");
for(;;){
    data_tx.action = NOPE;
if(xQueueReceive(queue_espnow_rx, &data_rx, portMAX_DELAY) == pdTRUE){
    show_server(data_rx.mac, "get parcel");
    if(data_rx.is_brodcast_dst){
        if(data_rx.parcel_len == SIZE_HELLO_PACKAGE){
            hello_package_t *hello_package = (hello_package_t *)data_rx.parcel;
            if(hello_package->action == REQUEST_ADD_NEW){
                data_tx.action = RESPONSE_ADD_NEW;
                add_peer(data_rx.mac, true);
            }else if(hello_package->action == RESPONSE_ADD_NEW
                    && !esp_now_is_peer_exist(data_rx.mac))
            {
                data_tx.action = NEED_DEVICE_INFO;
                add_peer(data_rx.mac, true);
            }
        }
    } else {
    switch(data_rx.parcel_len){
        case SIZE_SENSOR_PACKAGE :
        {
            
            break;
        }
        case SIZE_INFO_PACKAGE :
        {
            device_info_package_t *device_info_package = data_rx.parcel;
            crc = device_info_package->crc;
            device_info_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)device_info_package, SIZE_INFO_PACKAGE) 
                    && memcmp(device_info_package->mac, data_rx.mac, SIZE_MAC) == 0)
            {
                if(get_device_before(data_rx.mac) == NULL){
                    device_inf_t *new_device = create_device_info(device_info_package->type, 
                                                    data_rx.mac,
                                                    device_info_package->name);
                    if(new_device){
                        add_device_inf_to_list(new_device);
                        esp_event_post_to(direct_loop, EVENTS_DIRECTION, GET_DATA_FROM_DEVICE, &new_device, sizeof(device_inf_t*), TIMEOUT_SEND_EVENTS);
                    }
                    
                }
            }
            break;
        }
        case SIZE_ACTION_PACKAGE :
        {
            action_package_t *action_package = data_rx.parcel;
            crc = action_package->crc;
            action_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)action_package, SIZE_ACTION_PACKAGE)){
                if(action_package->action == NEED_TIME){
                    data_tx.action = GIVE_TIME;
                }else if(action_package->action == NEED_NETWORK){
                    data_tx.action = GIVE_NETWORK;
                }else if(action_package->action == NEED_DEVICE_INFO){
                    data_tx.action = GIVE_DIVICE_INFO;
                }
            }
            break;
        }
        case SIZE_TIME_PACKAGE :
        {
            time_package_t *time_package = data_rx.parcel;
            crc = time_package->crc;
            time_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)time_package, SIZE_TIME_PACKAGE)){
                struct timeval tv = {
                    .tv_sec = time_package->time
                };
                set_time_tv(&tv);
            }
            break;
        }
        case SIZE_NETWORK_PACKAGE :
        {
            network_package_t *network_package = data_rx.parcel;
            crc = network_package->crc;
            network_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)network_package, SIZE_NETWORK_PACKAGE)){
                strncpy(name_SSID, network_package->ssid, MAX_STR_LEN);
                strncpy(pwd_WIFI, network_package->pwd, MAX_STR_LEN);
                start_sta();
            }
            break;
        }
        default:break;
        }
    }
    if(data_tx.action != NOPE){
        memcpy(data_tx.mac, data_rx.mac, SIZE_MAC);
        xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
    }
    if(data_rx.parcel){
        free(data_rx.parcel);
    }
    }
}
}

esp_err_t add_peer(uint8_t *mac_addr, bool encrypt)
{
    esp_now_peer_info_t peer = {
        .channel = ESP_WIFI_CHANNEL,
        .ifidx = WIFI_IF_STA,
        .encrypt = encrypt,
    };
    memcpy(peer.lmk, ESPNOW_LMK, ESP_NOW_KEY_LEN);
    memcpy(peer.peer_addr, mac_addr, SIZE_MAC);
    return esp_now_add_peer(&peer);
}

esp_err_t modif_peer(uint8_t *mac_addr, bool encrypt)
{
    esp_now_peer_info_t peer = {
        .channel = ESP_WIFI_CHANNEL,
        .ifidx = WIFI_IF_STA,
        .encrypt = encrypt,
    };
    memcpy(peer.lmk, ESPNOW_LMK, ESP_NOW_KEY_LEN);
    memcpy(peer.peer_addr, mac_addr, SIZE_MAC);
    return esp_now_mod_peer(&peer);
}


//esp_register_shutdown_handler()

//base_mac
// esp_read_mac()
// esp_iface_mac_addr_set()
// esp_chip_data()
// esp_reset_reason()


void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if(!mac_addr)return;
    xTaskNotify(tx_espnow, status, eSetValueWithOverwrite);
}

void espnow_rx_cb(const esp_now_recv_info_t *recv_data, const uint8_t *data, int data_len)
{
    if(recv_data && recv_data->src_addr
            && recv_data->des_addr
            && data)
    {
        espnow_rx_data_t data_rx = {
            .parcel = malloc(data_len),
            .is_brodcast_dst = IS_BROADCAST_ADDR(recv_data->des_addr),
            .rssi = recv_data->rx_ctrl->rssi,
            .parcel_len  = data_len
        };
        if(data_rx.parcel){
            memcpy(data_rx.parcel, data, data_len);
            memcpy(data_rx.mac, recv_data->src_addr, SIZE_MAC);
            if(xQueueSend(queue_espnow_rx, &data_rx, 200) == pdTRUE) {
                return;
            }
            free(data_rx.parcel);
        }
    }
}