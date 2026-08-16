#include "espnow_dwin.h"
#include "device_info.h"
#include "sensor.h"
#include "dwin_timer.h"
#include "wifi_dwin.h"

uint8_t BRODCAST_MAC[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define IS_BROADCAST_ADDR(addr)     (memcmp(addr, BRODCAST_MAC, SIZE_MAC) == 0)


static esp_err_t add_peer(uint8_t *mac_addr, bool encrypt)
{
    esp_now_peer_info_t peer = {
        .channel = CONFIG_ESPNOW_CHANNEL,
        .ifidx = WIFI_IF_STA,
        .encrypt = encrypt,
    };
    memcpy(peer.lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
    memcpy(peer.peer_addr, mac_addr, SIZE_MAC);
    return esp_now_add_peer(&peer);
}

void espnow_task_tx(void *pv)
{
    dwin_data_t * const main_data = (dwin_data_t *)pv;
    union {
        hello_package_t hello;
        action_package_t action;
        network_package_t network;
        device_info_package_t device_info;
        time_package_t time;
    } tx_buf;
    espnow_send_t data_to_send;
    data_to_send.action = TRY_AGAIN;

    while (1) {
        uint8_t *parcel = NULL;
        uint8_t *mac = NULL;
        size_t parcel_len = 0;
        uint8_t max_atempt = 0;
        uint32_t res;
        uint8_t send_count = 0;
        bool send_ok = false;

        if (data_to_send.action == REQUEST_ADD_NEW || data_to_send.action == RESPONSE_ADD_NEW) {
            mac = BRODCAST_MAC;
        } else {
            mac = IS_ALL_ADDR(data_to_send.mac) ? NULL : data_to_send.mac;
        }
        parcel_len = SIZE_HELLO_PACKAGE;
    switch(data_to_send.action){
        case RESPONSE_ADD_NEW :;
        case REQUEST_ADD_NEW  : max_atempt = NUMBER_REQUEST_ADD_NEW;
        {
            parcel_len = SIZE_HELLO_PACKAGE;
            parcel = (uint8_t *)&tx_buf.hello;
            tx_buf.hello.action = data_to_send.action;
            break;
        }
        case NEED_TEMP :
        case NEED_TIME :
        case NEED_NETWORK :
        case NEED_DEVICE_INFO :
        {
            uint8_t local_mac[8];
            esp_read_mac(local_mac, ESP_MAC_WIFI_STA);
            memcpy(tx_buf.action.mac, local_mac, SIZE_MAC);
            max_atempt = MAX_ATEMPT_REQUEST;
            parcel = (uint8_t *)&tx_buf.action;
            parcel_len = SIZE_ACTION_PACKAGE;
            tx_buf.action.action = data_to_send.action;
            tx_buf.action.crc = 0;
            tx_buf.action.crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
            break;
        }
        case GIVE_NETWORK :
        {
            EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
            if(xEventGroup&BIT_CON_STA_OK){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_NETWORK_PACKAGE;
                parcel = (uint8_t *)&tx_buf.network;
                strncpy(tx_buf.network.pwd, pwd_WIFI, MAX_STR_LEN);
                strncpy(tx_buf.network.ssid, name_SSID, MAX_STR_LEN);
                tx_buf.network.crc = 0;
                tx_buf.network.crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
            } 
            break;
        }
        case GIVE_DIVICE_INFO :
        {
            device_inf_t *my_device = device_info_get_my();
            if(my_device){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_INFO_PACKAGE;
                parcel = (uint8_t *)&tx_buf.device_info;
                strncpy(tx_buf.device_info.name, my_device->name, MAX_NAME_DEVICE);
                tx_buf.device_info.type = MY_DEVICE_TYPE;
                tx_buf.device_info.crc = 0;
                tx_buf.device_info.crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
            }
            break;
        }
        case GIVE_TIME :
        {
            EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
            if(xEventGroup&BIT_IS_TIME){
                max_atempt = MAX_ATEMPT_SEND_DATA;
                parcel_len = SIZE_TIME_PACKAGE;
                parcel = (uint8_t *)&tx_buf.time;
                time_t tmp;
                time(&tmp);
                tx_buf.time.time = tmp;
                tx_buf.time.crc = 0;
                tx_buf.time.crc = esp_crc16_le(UINT16_MAX, parcel, parcel_len);
            }
            break;
        }
        default : break;
    }
    
    while(parcel && send_count++ < max_atempt){
        if(esp_now_send(mac, (const uint8_t*)parcel, parcel_len) != ESP_OK){
            break;
        }
        if(xTaskNotifyWait(0x00, ULONG_MAX, &res, (send_count+1)*TIMEOUT_SEND) == pdTRUE
                && res == ESP_NOW_SEND_SUCCESS)
        {
            send_ok = true;
            break;
        } 
    }
    if(xQueueReceive(espnow_tx_queue, &data_to_send,
                                send_ok
                                ? portMAX_DELAY 
                                : WAIT_NEXT_REQEST_ADD) != pdTRUE)
    {
        data_to_send.action = TRY_AGAIN;
    }
}
}

void espnow_task_rx(void *pv)
{
    dwin_data_t * const main_data = (dwin_data_t *)pv;
    uint16_t crc;
    espnow_rx_data_t data_rx;
    espnow_send_t data_tx;
    vTaskDelay(DEALAY_START_ESPNOWTASK);
    espnow_rx_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_rx_data_t));
    assert(espnow_rx_queue);
for(;;){

if(xQueueReceive(espnow_rx_queue, &data_rx, portMAX_DELAY) == pdTRUE){
    data_tx.action = 0;
    if(data_rx.is_brodcast_dst){
        if(data_rx.parcel_len == SIZE_HELLO_PACKAGE){
            hello_package_t *hello_package = (hello_package_t *)data_rx.parcel;
            bool new_device = !esp_now_is_peer_exist(data_rx.mac);
            if(new_device)add_peer(data_rx.mac, true);
            if(hello_package->action == REQUEST_ADD_NEW){
                data_tx.action = RESPONSE_ADD_NEW;
            } else if(hello_package->action == RESPONSE_ADD_NEW && new_device) {
                vTaskDelay(RANDOM_DELAY);
                data_tx.action = NEED_DEVICE_INFO;
            }
        }
    } else {
    switch(data_rx.parcel_len){
        case SIZE_SENSOR_PACKAGE :
        {
            sensor_package_t *sensor_package = (sensor_package_t *)data_rx.parcel;
            crc = sensor_package->crc;
            sensor_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)sensor_package, SIZE_SENSOR_PACKAGE)){
                sensor_set_data(main_data, 
                                data_rx.mac, 
                                sensor_package->temperature, 
                                sensor_package->humidity);
            }
            break;
        }
        case SIZE_INFO_PACKAGE :
        {
            device_info_package_t *device_info_package = (device_info_package_t *)data_rx.parcel;
            crc = device_info_package->crc;
            device_info_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)device_info_package, SIZE_INFO_PACKAGE)){
                if(device_info_get_before(data_rx.mac) == NULL){
                    device_inf_t *new_device = device_info_create(device_info_package->type, 
                                                    data_rx.mac,
                                                    device_info_package->name);
                    if(new_device){
                        device_info_add_to_list(new_device);
                        EventBits_t xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_ESPNOW_CONECT);
                        memcpy(data_tx.mac, new_device->mac, SIZE_MAC);
                        if(!(xEventGroup&BIT_CON_STA_OK)){
                            data_tx.action = NEED_NETWORK;
                            xQueueSend(espnow_tx_queue, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
                            if(!(xEventGroup&BIT_IS_TIME)){
                                data_tx.action = NEED_TIME;
                                xQueueSend(espnow_tx_queue, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
                            }
                        }
                        if(new_device->type == SENSOR_TEMP_DEVICE){
                            EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
                            if(!(xEventGroup&BIT_SEN_1)){
                                xEventGroupSetBits(dwin_event_group, BIT_SEN_1);
                            } else {
                                xEventGroupSetBits(dwin_event_group, BIT_SEN_2);
                            }
                            sensor_set_device(main_data, data_tx.mac, new_device->name);
                            data_tx.action = NEED_TEMP;
                            dwin_timer_set_periodic_event(GET_REMOTE_SENSOR, DELAY_GET_TEMPERATURE, RELOAD_COUNT);
                        } else {
                            data_tx.action = 0;
                        }
                    }
                }
            }
            break;
        }
        case SIZE_ACTION_PACKAGE :
        {
            action_package_t *action_package = (action_package_t *)data_rx.parcel;
            crc = action_package->crc;
            action_package->crc = 0;
            if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)action_package, SIZE_ACTION_PACKAGE) 
                && memcmp(action_package->mac, data_rx.mac, SIZE_MAC) == 0)
            {
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
            EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
            if(!(xEventGroup&BIT_IS_TIME)){
                time_package_t *time_package = (time_package_t *)data_rx.parcel;
                crc = time_package->crc;
                time_package->crc = 0;
                if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)time_package, SIZE_TIME_PACKAGE)){
                    time_t new_time = (time_t)time_package->time;
                    struct timeval tv = {
                        .tv_sec = new_time
                    };
                    set_time_tv(&tv);
                }
            }
            break;
        }
        case SIZE_NETWORK_PACKAGE :
        {
            EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
            if(!(xEventGroup&BIT_CON_STA_OK)){
                network_package_t *network_package = (network_package_t *)data_rx.parcel;
                crc = network_package->crc;
                network_package->crc = 0;
                if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)network_package, SIZE_NETWORK_PACKAGE)){
                    strncpy(name_SSID, network_package->ssid, MAX_STR_LEN);
                    strncpy(pwd_WIFI, network_package->pwd, MAX_STR_LEN);
                    dwin_timer_set_periodic_event(CHECK_NET_DATA, 20, ONLY_ONCE);
                }
            }
            break;
        }
        default:break;
        }
    }
    if(data_tx.action){
        memcpy(data_tx.mac, data_rx.mac, SIZE_MAC);
        xQueueSend(espnow_tx_queue, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
    }

    }
}
}

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
            .is_brodcast_dst = IS_BROADCAST_ADDR(recv_data->des_addr),
            .rssi = recv_data->rx_ctrl->rssi,
            .parcel_len  = data_len
        };
        if(data_len <= sizeof(data_rx.parcel)){
            memcpy(data_rx.parcel, data, data_len);
            memcpy(data_rx.mac, recv_data->src_addr, SIZE_MAC);
            xQueueSend(espnow_rx_queue, &data_rx, 300);
        }
    }
}
