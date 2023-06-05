#include "show_screen_handlers.h"

const int FONT_STANDART = 3, FONT_INFO = 2, FONT_SECOND_INFO = 1, FONT_BUTTON = 4;

void show_ap_handler(void* main_data, 
                        esp_event_base_t base,
                        int32_t state, 
                        void* event_data)
{
    print_start(1, 2, WHITE, FONT_STANDART);
    if(!event_data){
        send_str(
                "Connect to the AP with the name %s, enter the %s paswword and and go to the address in your browser %s",
                AP_WIFI_SSID,
                AP_WIFI_PWD,
                MY_IP);
    } else {
        uint8_t *mac = (uint8_t *)event_data;
        if(state == STATION_JOINE){
           send_str( 
                "Station "MACSTR" join.", MAC2STR(mac));
        } else {
             send_str(
                    "Station "MACSTR" leave.", MAC2STR(mac));
        }
    }
    print_end();
}



void show_ssid_handler(void* main_data, 
                                esp_event_base_t base, 
                                int32_t ssid_number, 
                                void* event_data) 
{
    wifi_ap_record_t *wifi_record = (wifi_ap_record_t*)event_data;
    print_start(1, 3, LEMON, FONT_STANDART);
    if(ssid_number == 0) {
         send_str("\n\nNo networks found");
    } else {
        send_str("Find %d SSID \r\n  Name      Signal  Auth.", (int)ssid_number);
    }
    print_end();
    for(int i=0; i<ssid_number; i++) {
        vTaskDelay(DELAY_SHOW_ITEM/2);
        print_start(1+i, 0, GET_COLOR_AREA(i), FONT_INFO);
        send_str(" %d. %10.10s %u %8.8s", 
                    i, 
                    (char*) wifi_record->ssid, 
                    (uint8_t) wifi_record->rssi, 
                    get_auth_mode(wifi_record->authmode));
        wifi_record++;
        print_end();
    }
}



void show_setting_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t id, 
                            void* event_data) 
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    for(uint8_t i=0; i<5; i++) {
        vTaskDelay(DELAY_SHOW_ITEM);
        switch (i) {
            case 0 :
                print_start(0, 7, xEventGroup&BIT_PROCESS 
                                ? COLOUR_ENABLE
                                : COLOUR_DISABLE,
                                FONT_SECOND_INFO);
                if(xEventGroup&BIT_PROCESS){
                    send_str("Connection attempt...");
                }else if(xEventGroup&BIT_CON_STA_OK) {
                    send_str("WiFi connected ");
                    if(xEventGroup&BIT_WEATHER_OK) {
                        send_str("| server OK");
                    } else {
                        send_str("|bad api data for openweather.com");
                    }
                } else if(xEventGroup&BIT_SSID_FOUND){
                    send_str(" Wrong password WiFi");
                } else {
                    send_str(" SSID not found");
                }
                break;
            case 1:
                print_start(2, 1, GET_COLOR_AREA(i), FONT_STANDART);
                send_str("SSID  : %s", name_SSID);
                break;
            case 2:
                print_start(3, 1, GET_COLOR_AREA(i), FONT_STANDART);
                send_str("Password : %s", 
                                    xEventGroup&BIT_SECURITY || area_SCREEN != AREA_PASSWORD
                                    ? "*** WiFi password ***"
                                    : pwd_WIFI);
                break;
            case 3:
                print_start(4, 1, GET_COLOR_AREA(i), FONT_STANDART);
                send_str("Name city : %s", name_CITY);
                break;
            case 4:
                print_start(5, 1, GET_COLOR_AREA(i), FONT_STANDART);
                send_str("Key ");
                if(area_SCREEN == AREA_API) {
                    send_str(api_KEY);
                } else {
                    send_str("*** api openweather.com key ***");
                }
                break;
            default : break;
        }
        print_end();
    }
}


// void show_device_handler(void* main_data, 
//                                 esp_event_base_t base, 
//                                 int32_t conected_device, 
//                                 void* event_data) 
// {
//     static const  main_data_t *main_data = NULL; 
//     // if(main_data == NULL) main_data = (main_data_t *)main_data;
//     // device_inf_t *list_devices = (device_inf_t*) &main_data->list_devices;
//     // if(device == NULL) {
//     //     print_start(4, 3, WHITE, FONT_STANDART);
//     //     send_str("No ones devices finding");
//     //     dwin_end_print();
//     // }

//     // char* name_cur_device = device->name_device_sensor[0];
//     // for(uint8_t i=0; i<5; i++) {
//     //     vTaskDelay(DELAY_SHOW_ITEM);
//     //     switch(i){
//     //         case 0 :
//     //             print_start(1, 2, WHITE, FONT_STANDART);
//     //             send_str("Sensor devices           Timer devices");
//     //             dwin_end_print();
//     //             break;
//     //         case 1 :
//     //             for(uint8_t count_device=0, device_state=START_FLAG_SENSORS && name_cur_device != NULL; count_device<MAX_SLAVE_DEVICES; count_device++,device_state++,name_cur_device++) {
//     //                 if(area_SCREEN == count_device) send_str("[");
//     //                 print_start(2+count_device, 0, get_FLAG_STATE(device_state)?LEMON:BLUE, FONT_INFO);
//     //                 send_non_zero(count_device+1);
//     //                 send_str(". ");
//     //                 send_str(name_cur_device);
//     //                 if(area_SCREEN == count_device) send_str("]");
//     //                 dwin_end_print();
//     //             }
//     //             break;
//     //         case 2 :
//     //             send_in_frame(1, 1, FONT_INFO, WHITE, "RENAME");
//     //             send_in_frame(
//     //                 1, 
//     //                 1, 
//     //                 FONT_INFO, 
//     //                 WHITE, 
//     //                 get_FLAG_STATE(area_SCREEN+START_FLAG_SENSORS)
//     //                 ? "CONNECT"
//     //                 : "DISCONNECT");
//     //                 break;
//     //         case 3 :
//     //             name_cur_device = device->name_device_timer[0];
//     //             for(uint8_t count_device=0, device_state=START_FLAG_TIMERS; count_device<MAX_SLAVE_DEVICES && name_cur_device; count_device++,device_state++,name_cur_device++) {
//     //                 if(area_SCREEN == count_device+AREA_TIMER_DEVICE_1) send_str("[");
//     //                 print_start(2+count_device, 0, get_FLAG_STATE(device_state)?LEMON:BLUE, FONT_INFO);
//     //                 send_non_zero(count_device+1);
//     //                 send_str(". ");
//     //                 send_str(name_cur_device);
//     //                 if(area_SCREEN == count_device) send_str("]");
//     //                 dwin_end_print();
//     //             }
//     //             break;
//     //         case 4 :
//     //             if(area_SCREEN >= AREA_TIMER_DEVICE_1)send_in_frame(1, 1, FONT_INFO, WHITE, "CONFIGURE");
//     //             break;
//     //         default: break;
//     //     }
//     // }
// }


// void show_rename_device_handler(void* main_data, 
//                                         esp_event_base_t base, 
//                                         int32_t id, 
//                                         void* event_data) 
// {
//     const device_info_show_t *device = (device_info_show_t*) event_data;
//     if(device == NULL)return;
//     for(uint8_t i=0; i<2; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch(i){
//             case 0 :
//                 print_start(1, 4, WHITE, FONT_STANDART);
//                 send_str(device_descr[device->type]);
//                 break;
//             case 1 :
//                 print_start(3, 2, WHITE, FONT_STANDART);
//                 send_str(device->name);
//                 break;
//         }
//         dwin_end_print();
//     }
// }

// void show_notify_handler(void* main_data, esp_event_base_t base, int32_t day, void* event_data) 
// {
//     static const  main_data_t *main_data = NULL; 
//     if(main_data == NULL)  main_data = (main_data_t *)main_data;
//     static uint8_t tmp;
//     uint8_t last_notif = NUMBER_NOTIFICATION_PER_DAY;
    
//     for(uint8_t i=0; i<6; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch(i){
//             case 0 :
//                 for(uint8_t day_count=0; day_count<SIZE_WEEK; day_count++) {
//                     print_start(2+day_count, 0, day == day_count ? WHITE : BLUE, FONT_INFO);
//                     send_str(WEEK_DAY[day_count]);
//                     send_str(is_ACTIVE_DAY(day_count) ? MES_ON : MES_OFF);
//                     dwin_end_print();
//                 }
//                 print_start(9, 0, VIOLET, FONT_INFO);
//                 send_str("\tFor\tall\tdays");
//                 break;
//             case 1 :
//                 send_in_frame(1, 1, FONT_INFO, is_ACTIVE_DAY(day)?COLOUR_ENABLE:COLOUR_DISABLE, WEEK_WHOLE_NAME[day]);
//                 break;
//             case 2 :
//                 for(int count_hour=0, area=0; count_hour<last_notif; count_hour++, area++) {
//                     tmp = GET_NOTIF_HOUR(day, count_hour);
//                     print_start(2+count_hour, 6, GET_COLOR_AREA(area), FONT_STANDART);
//                     if(IS_HOUR(tmp)){
//                         send_str("\t\t+");
//                         last_notif = count_hour;
//                     } else {
//                         send_str("\t\t-\t");
//                         send_two_number(tmp);
//                         send_str("\t\t:");
//                     }
//                     dwin_end_print();
//                 }
//                 break;
//             case 3 :
//                 for(int area=AREA_MIN_1, count_min=0; count_min<last_notif; count_min++, area++) {
//                     tmp = GET_NOTIF_MIN(day, count_min);
//                     print_start(2+count_min, 12, GET_COLOR_AREA(area), FONT_STANDART);
//                     send_two_number(tmp);
//                 }
//                 break;
//             case 4 :
//                 print_start(0, 1, color_CLOCK, FONT_INFO);
//                 send_clock(cur_HOUR, cur_MIN);
//                 break;
//             case 5 :
//                 print_start(4, 9, COLOUR_DISABLE, FONT_BUTTON);
//                 if(is_ACTIVE_DAY(day))send_str("SET\tOFF");
//                 else send_str("SET\tON");
//                 break;
//             default: break;
//         }
//         dwin_end_print();
//     }
// }

// void show_custom_handler(void* main_data, 
//                                     esp_event_base_t base, 
//                                     int32_t id, 
//                                     void* event_data) 
// {
//     static const  main_data_t *main_data = NULL;  
//     if(main_data == NULL) main_data = (main_data_t *)main_data;
//     static uint16_t color_item, color_area;
//     int i=0;
//     vTaskDelay(DELAY_SHOW_ITEM);
//     for(; i<SIZE_COLOURS_INTERFACE; ) {
//         color_area = GET_COLOUR(colors_INTERFACE[i]);
//         print_start(1, i*5, color_area, FONT_STANDART);
//         if(area_SCREEN == i)send_str("[");
//         send_str(massage_custom[i]);
//         if(area_SCREEN == i)send_str("]");
//         dwin_end_print();
//         i++;
//     }
//     color_area = GET_COLOUR(colors_INTERFACE[area_SCREEN]);
//     vTaskDelay(DELAY_SHOW_ITEM);
//     for(int color_it=0; color_it<SIZE_USED_COLOURS; color_it++) {
//         color_item = GET_COLOUR(color_it),
//         print_start(i+1, 1, color_item, FONT_STANDART);
//         if(color_item == color_area)send_str("<--");
//         send_str(massage_custom[i]);
//         dwin_end_print();
//         i++;
//     }
//     print_start(4, 9, COLOUR_DISABLE, FONT_BUTTON);
//     send_str(massage_custom[i]);
//     dwin_end_print();
// }





// void show_clock_handler(void* main_data, 
//                                     esp_event_base_t base, 
//                                     int32_t id, 
//                                     void* event_data)
// {
//     static const  main_data_t *main_data = NULL; 
//     if(main_data == NULL) main_data = (main_data_t *)main_data;
//     for(uint8_t i=0; i<6; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch (i) {
//             case 0:
//                 print_start(0, 2, COLOUR_DISABLE, FONT_INFO);
//                 if(DATA_PREPARING) {
//                     sync_TIME
//                     ? send_str("autoupdate time")
//                     : send_str("manual update time");
//                     internet_OK
//                     ? send_str(" SNTP ok")
//                     : send_str(" SNTP fail");
//                 } else {
//                     send_str("Waiting update data...");
//                 }
//                 break;
//             case 1:
//                 print_start(3, 1, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("year 20");
//                 send_two_number(cur_YEAR);
//                 break;
//             case 2:
//                 print_start(3, 9, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("month ");
//                 send_two_number(cur_MONTH);
//                 break;
//             case 3:
//                 print_start(3, 15, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("day ");
//                 send_two_number(cur_DAY);
//                 break;
//             case 4:
//                 print_start(5, 1, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("hour ");
//                 send_two_number(cur_HOUR);
//                 break;
//             case 5:
//                 print_start(5, 9, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("min ");
//                 send_two_number(cur_MIN);
//                 break;
//             case 6:
//                 print_start(5, 15, GET_COLOR_AREA(i), FONT_STANDART);
//                 send_str("sec ");
//                 send_two_number(cur_SEC);
//                 break;
//             default : break;
//         }
//         dwin_end_print();
//     }
// }



// void show_main_handler(void* main_data, 
//                                 esp_event_base_t base, 
//                                 int32_t data_sensor, 
//                                 void* event_data) 
// {
//     static const  main_data_t *main_data = NULL; 
//     if(main_data == NULL) main_data = (main_data_t *)main_data;
//     weather_t *data_weather = event_data;
//     size_t count = data_weather != NULL ? 5 : 3;
//     for(uint32_t i=0; i<count; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch (i) {
//             case 0:
//                 print_start(2, 3, color_CLOCK, 6);
//                 send_clock(cur_HOUR, cur_MIN);
//                 break;
//             case 1:
//                 print_start(8, 10, color_CLOCK, 2);
//                 send_str(WEEK_DAY[cur_WEEK_DAY]);
//                 send_number(cur_DAY);
//                 break;
//             case 2:
//                 if(data_weather == NULL && data_sensor == NO_TEMP_SENSOR)return;
//                 print_start(0, 1, color_INFO, FONT_INFO);
//                 if(data_sensor != NO_TEMP_SENSOR) {
//                     send_str(" inside    t*C     ");
//                     send_temperature(data_sensor);
//                 }
//                 if(data_weather != NULL) {
//                     send_str("\n outside     t*C     ");
//                     send_temperature(temp_OUTDOR);
//                     send_str("\n feels like     t*C   ");
//                     send_temperature(temp_FEELS_LIKE);
//                     send_str("\n rain           %     ");
//                     send_temperature(PoP);
//                 }
//                 break;
//             case 3:
//                 print_start(3, 4, color_DESC, FONT_STANDART);
//                 send_str(description_WEATHER);
//                 break;
//             case 4:
//                 print_start(20, 12, color_INFO, FONT_SECOND_INFO);
//                 send_str("sunrise  ");
//                 send_clock(sunrise_HOUR, sunrise_MIN);
//                 send_str("sunset  ");
//                 send_clock(sunset_HOUR, sunset_MIN);
//                 break;
//         default   : break;
//         }
//         dwin_end_print();
//     }
// }



// void show_timer_handler(void* main_data, 
//                                     esp_event_base_t base, 
//                                     int32_t run, 
//                                     void* event_data) 
// {
//     static const  main_data_t *main_data = NULL; 
//     if(main_data == NULL) main_data = (main_data_t *)main_data;
//     uint8_t *timer_data = (uint8_t *)event_data;
//     if(timer_data == NULL)return;
//     vTaskDelay(DELAY_SHOW_ITEM);
//     print_start(0, 12, color_CLOCK, FONT_INFO);
//     send_clock(cur_HOUR, cur_MIN);
//     dwin_end_print();
//     if(run){
//         uint8_t column = timer_HOUR ? 1 : timer_MIN ? 2 : 4;
//         print_start(column, 1, color_CLOCK, 6);
//         if(send_non_zero(timer_HOUR)) {
//             send_str(" : ");
//             send_two_number(timer_MIN);
//             send_str(" : ");
//             send_two_number(timer_SEC);
//         } else if(send_non_zero(timer_MIN)) {
//             send_str(" : ");
//             send_two_number(timer_SEC);
//         } else {
//             send_number(timer_SEC);
//         }
//     dwin_end_print();
//     } else {
//         for(int i=0; i<SIZE_TIMER; i++) {
//             print_start(i*3, 1, GET_COLOR_AREA(i), 6);
//             send_two_number(timer_data[i]);
//             dwin_end_print();
//         }
//     }
// }


// void welcome() 
// {
//     const int H = 10;
//     const int W = 24;
//     for (int h_count = 1; h_count < H; h_count++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         for (int w_count = h_count % 2 == 0 ? 5 : 3, countView = 0; w_count < W; countView++) {
//             if (countView % 1000 == 0) {
//                 w_count++;
//                 if (w_count % (h_count % 2 == 0 ? 2 : 3) == 0) {
//                     print_start(h_count, w_count, LEMON, 2);
//                     send_non_zero((w_count * h_count * 3) % 100);
//                     dwin_end_print();
//                 }
//             }
//         }
//     }
//     print_start(3, 7, WHITE, FONT_STANDART);
//     send_str("WAIT...");
//     dwin_end_print();
// }


