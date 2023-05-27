#include "espnow_dwin.h"


// void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
// {
//     if(!mac_addr)return;
//     xTaskNotify(espnow_task_tx, status, eSetValueWithOverwrite);
// }

// void espnow_rx_cb(const esp_now_recv_info_t *recv_data, const uint8_t *data, int data_len)
// {
//     if(recv_data && recv_data->src_addr
//             && recv_data->des_addr
//             && data
//     ){
//         espnow_rx_data_t *data_rx = (espnow_rx_data_t*) malloc(sizeof(espnow_rx_data_t));
//         if(data_rx){
//             data_rx->parcel = malloc(data_len);
//             if(data_rx->parcel){
//                 data_rx->is_brodcast_dst = IS_BROADCAST_ADDR(recv_data->des_addr);
//                 data_rx->rssi = recv_data->rx_ctrl->rssi;
//                 memcpy(data_rx->parcel, data, data_len);
//                 memcpy(data_rx->mac, recv_data->src_addr, SIZE_MAC);
//                 data_rx->parcel_len  = data_len;
//                 if(xQueueSend(queue_espnow_rx, data_rx, 100) == pdTRUE) {
//                     return;
//                 }
//                 free(data_rx->parcel);
//             }
//             free(data_rx);
//         }
//     }
// }

// void remove_espnow_device(const uint8_t *mac_addr)
// {
//     if(mac_addr != NULL){
//         esp_now_del_peer(mac_addr);
//         delete_device_info(mac_addr);
//     }
// }


// void espnow_task_tx(void *pv)
// {
//     main_data_t * const data_dwin = (main_data_t *)pv;
//     const uint8_t *parcel = NULL, *mac;
//     uint8_t max_atempt;
//     size_t parcel_len = 0;
//     espnow_send_t data_to_send = {
//         .action = REQUEST_ADD_NEW,
//     };
//     ESP_ERROR_CHECK(add_peer(BRODCAST_MAC, false));
//     uint32_t res;
//     bool send_ok;
//     size_t send_count;
//     vTaskDelay(pdMS_TO_TICKS(10000));
// for(;;){
//     if(parcel){
//         free(parcel);
//         parcel = NULL;
//     }
//     send_ok = false;
//     send_count = max_atempt = 0;
//     if(data_to_send.action == REQUEST_ADD_NEW || data_to_send.action == RESPONSE_ADD_NEW){
//         mac = BRODCAST_MAC;
//     } else {
//         mac = IS_ALL_ADDR(data_to_send.mac) ? NULL : data_to_send.mac;
//     }
//     switch(data_to_send.action){
//         case RESPONSE_ADD_NEW : max_atempt = NUMBER_RESPONSE_ADD_NEW;
//         case REQUEST_ADD_NEW : max_atempt = NUMBER_REQUEST_ADD_NEW;
//         {
//             hello_package_t *hello_package = malloc(SIZE_HELLO_PACKAGE);
//             parcel_len = SIZE_HELLO_PACKAGE;
//             if(hello_package){
//                 parcel = hello_package;
//                 hello_package->action = data_to_send.action;
//             }
//             break;
//         }
//         case NEED_TIME :
//         case NEED_NETWORK :
//         case NEED_DEVICE_INFO :
//         {
//             action_package_t *action_package = malloc(SIZE_ACTION_PACKAGE);
//             if(action_package){
//                 max_atempt = MAX_ATEMPT_REQUEST;
//                 parcel = action_package;
//                 parcel_len = SIZE_ACTION_PACKAGE;
//                 action_package->action = data_to_send.action;
//                 action_package->crc = 0;
//                 action_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
//             }
//             break;
//         }
//         case GIVE_NETWORK :
//         {
//             size_t size_pwd = strnlen(pwd_WIFI, MAX_STR_LEN);
//             size_t size_ssid = strnlen(name_SSID, MAX_STR_LEN);
//             network_package_t *package = malloc(SIZE_NETWORK_PACKAGE);
//             if(package){
//                 max_atempt = MAX_ATEMPT_SEND_DATA;
//                 parcel_len = SIZE_NETWORK_PACKAGE;
//                 parcel = package;
//                 memcpy(package->pwd, pwd_WIFI, size_pwd);
//                 package->pwd[size_pwd] = '\0';
//                 memcpy(package->ssid, name_SSID, size_ssid);
//                 package->ssid[size_ssid] = '\0';
//                 package->crc = 0;
//                 package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
//             }
//             break;
//         }
//         case GIVE_DIVICE_INFO :
//         {
//             device_inf_t *my_device = get_my_device();
//             if(my_device){
//                 max_atempt = MAX_ATEMPT_SEND_DATA;
//                 device_info_package_t *device_info_package = malloc(SIZE_INFO_PACKAGE);
//                 if(device_info_package){
//                     size_t name_len = strnlen(my_device->name, MAX_NAME_DEVICE);
//                     memcpy(device_info_package->name, my_device->name, name_len);
//                     memcpy(device_info_package->mac, my_device->mac, SIZE_MAC);
//                     parcel_len = SIZE_INFO_PACKAGE;
//                     parcel = device_info_package;
//                     device_info_package->type = MY_DEVICE_TYPE;
//                     device_info_package->crc = 0;
//                     device_info_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
//                     device_info_package->name[name_len] = '\0';
//                 }
//             }
//             break;
//         }
//         case GIVE_TIME :
//         {
//             time_package_t *time_package = malloc(SIZE_TIME_PACKAGE);
//             if(time_package){
//                 max_atempt = MAX_ATEMPT_SEND_DATA;
//                 parcel_len = SIZE_TIME_PACKAGE;
//                 parcel = time_package;
//                 time(&time_package->time);
//                 time_package->crc = 0;
//                 time_package->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)parcel, parcel_len);
//             }
//             break;
//         }
//         default : break;
//     }
//     while(parcel && send_count++ < max_atempt){
//         if(esp_now_send(mac, (const uint8_t*)parcel, parcel_len) != ESP_OK){
//             break;
//         }
//         if(xTaskNotifyWait(0x00, 0xff, &res, (send_count+1)*TIMEOUT_SEND) == pdTRUE
//                 && res == ESP_NOW_SEND_SUCCESS){
//             send_ok = true;
//             break;
//         }
//     }
//     if(data_to_send.action == RESPONSE_ADD_NEW){
//         if(send_ok){
//             vTaskDelay(RANDOM_DELAY);
//             data_to_send.action = NEED_DEVICE_INFO;
//             continue;
//         } else {
//             remove_espnow_device(data_to_send.mac);
//         }
//     }else if(data_to_send.action == NEED_DEVICE_INFO){
//         if(!send_ok || xTaskNotifyWait(0x00, 0xff, &res, TIMEOUT_RESPONSE_DEVICE_INFO) != pdTRUE){
//             remove_espnow_device(data_to_send.mac);
//         }
//     }
//     if(xQueueReceive(queue_espnow_tx, &data_to_send,
//                                 send_ok
//                                 ? portMAX_DELAY 
//                                 : WAIT_NEXT_REQEST_ADD) != pdTRUE 
//                             && send_ok){
//         data_to_send.action = NOPE;
//     }
// }
// }


// void espnow_task_rx(void *pv)
// {
//     main_data_t * const data_dwin = (main_data_t *)pv;
//     /*parcel data*/
//     uint8_t rssi;
//     uint16_t crc;
//     espnow_rx_data_t *data_rx;
//     espnow_send_t data_tx;

// for(;;){
//     data_tx.action = NOPE;
// if(xQueueReceive(queue_espnow_rx, &data_rx, portMAX_DELAY) == pdTRUE){
//     if(data_rx->is_brodcast_dst){
//         if(data_rx->parcel_len == SIZE_HELLO_PACKAGE){
//             hello_package_t *hello_package = (hello_package_t *)data_rx->parcel;
//             if(hello_package->action == REQUEST_ADD_NEW){
//                 data_tx.action = RESPONSE_ADD_NEW;
//                 add_peer(data_rx->mac, true);
//             }else if(hello_package->action == RESPONSE_ADD_NEW
//                     && !esp_now_is_peer_exist(data_rx->mac)){
//                 data_tx.action = NEED_DEVICE_INFO;
//                 add_peer(data_rx->mac, true);
//             }
//         }
//     } else {
//         switch(data_rx->parcel_len){
//         case SIZE_SENSOR_PACKAGE :
//         {
            
//             break;
//         }
//         case SIZE_INFO_PACKAGE :
//         {
//             device_info_package_t *device_info_package = data_rx->parcel;
//             crc = device_info_package->crc;
//             device_info_package->crc = 0;
//             if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)device_info_package, SIZE_INFO_PACKAGE) 
//                     && memcmp(device_info_package->mac, data_rx->mac, SIZE_MAC) == 0){
//                 xTaskNotify(espnow_task_tx, ESP_OK, eSetValueWithOverwrite);
//                 if(get_device_before(data_rx->mac) == NULL){
//                     device_inf_t *new_device = create_device_info(device_info_package->type, 
//                                                     data_rx->mac,
//                                                     device_info_package->name);
//                     add_device_inf_to_list(new_device);
//                 }
//             }
//             break;
//         }
//         case SIZE_ACTION_PACKAGE :
//         {
//             action_package_t *action_package = data_rx->parcel;
//             crc = action_package->crc;
//             action_package->crc = 0;
//             if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)action_package, SIZE_ACTION_PACKAGE)){
//                 if(action_package->action == NEED_TIME){
//                     data_tx.action = GIVE_TIME;
//                 }else if(action_package->action == NEED_NETWORK){
//                     data_tx.action = GIVE_NETWORK;
//                 }else if(action_package->action == NEED_DEVICE_INFO){
//                     data_tx.action = GIVE_DIVICE_INFO;
//                 } else {
//                     break;
//                 }
//             }
//             break;
//         }
//         case SIZE_TIME_PACKAGE :
//         {
//             time_package_t *time_package = data_rx->parcel;
//             crc = time_package->crc;
//             time_package->crc = 0;
//             if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)time_package, SIZE_TIME_PACKAGE)){
//                 static  time_t time;
//                 time = time_package->time;
//                 esp_event_post_to(loop_direct, EVENTS_SET_TIME, UPDATE_TIME_FROM_MS, &time, sizeof(time_t), TIMEOUT_SEND_EVENTS);
//             }
//             break;
//         }
//         case SIZE_NETWORK_PACKAGE :
//         {
//             network_package_t *network_package = data_rx->parcel;
//             crc = network_package->crc;
//             network_package->crc = 0;
//             if(crc == esp_crc16_le(UINT16_MAX, (uint8_t const *)network_package, SIZE_NETWORK_PACKAGE)){
//                 memcpy(name_SSID, network_package->ssid, SIZE_BUF);
//                 memcpy(pwd_WIFI, network_package->pwd, SIZE_BUF);
//             }
//             break;
//         }
//         default:break;
//         }
//     }
//     if(data_tx.action != NOPE){
//         memcpy(data_tx.mac, data_rx->mac, SIZE_MAC);
//         xQueueSend(queue_espnow_tx, &data_tx, WAIT_PUSH_ESPNOW_TO_QUEUE);
//     }
//     if(data_rx){
//         if(data_rx->parcel){
//             free(data_rx->parcel);
//         }
//         free(data_rx);
//         data_rx = NULL;
//     }
//     }
// }
// }

// esp_now_peer_info_t *get_peer(uint8_t *mac_addr, bool encrypt)
// {
//     static esp_now_peer_info_t peer;
//     peer.channel = ESP_WIFI_CHANNEL;
//     peer.ifidx = WIFI_IF_STA;
//     peer.encrypt = encrypt;
//     memcpy(peer.lmk, ESPNOW_LMK, ESP_NOW_KEY_LEN);
//     memcpy(peer.peer_addr, mac_addr, SIZE_MAC);
//     return &peer;
// }

// esp_err_t add_peer(uint8_t *mac_addr, bool encrypt)
// {
//     return esp_now_add_peer(get_peer(mac_addr, encrypt));
// }

// esp_err_t modif_peer(uint8_t *mac_addr, bool encrypt)
// {
//     return esp_now_mod_peer(get_peer(mac_addr, encrypt));
// }


// //esp_register_shutdown_handler()

// //base_mac
// // esp_read_mac()
// // esp_iface_mac_addr_set()
// // esp_chip_data()
// // esp_reset_reason()