// #include "show_screen_handlers.h"


// static const size_t FONT_STANDART = 3, FONT_INFO = 2, FONT_SECOND_INFO = 1, FONT_BUTTON = 4;

// void show_server_handler(void* handler_args, 
//                                     esp_event_base_t base,
//                                     int32_t conected_device, 
//                                     void* event_data) 
// {
//     wifi_ap_record_t *wifi_record = (wifi_ap_record_t*)event_data;
//     static const  main_data_t *data_dwin = NULL; 







// }


// void show_ssid_handler(void* handler_args, 
//                                 esp_event_base_t base, 
//                                 int32_t conected_device, 
//                                 void* event_data) 
// {
//     wifi_ap_record_t *wifi_record = (wifi_ap_record_t*)event_data;
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     vTaskDelay(DELAY_SHOW_ITEM);
//     dwin_print(1, 3, LEMON, FONT_STANDART);
//     if(conected_device == 0) {
//         send_data_dwin("\n\n\nNo networks found'myEOF'");
//         dwin_end_print();
//         return;
//     }
//     if(wifi_record == NULL)return;
//     send_data_dwin("Find 'myEOF'");
//     send_non_zero(conected_device);
//     send_data_dwin(" networks'myEOF'");
//     send_data_dwin("\r\n  Name     Signal  Auth.'myEOF'");
//     dwin_end_print();
//     for(uint8_t i=0; i<conected_device; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM/2);
//         dwin_print(1+i, 2, GET_COLOUR_AREA(i), FONT_INFO);
//         send_non_zero(i+1);
//         send_str_dwin((char)wifi_record->ssid);
//         send_str_dwin(get_auth_mode(wifi_record->authmode));
//         if(wifi_record->rssi > -50) send_str_dwin("  excellent");
//         else if(wifi_record->rssi > -70)send_str_dwin(" good");
//         else send_str_dwin("   bad");
//         wifi_record++;
//         dwin_end_print();
//     }
// }


// void show_device_handler(void* handler_args, 
//                                 esp_event_base_t base, 
//                                 int32_t conected_device, 
//                                 void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL; 
//     // if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     // device_inf_t *list_devices = (device_inf_t*) &data_dwin->list_devices;
//     // if(device == NULL) {
//     //     dwin_print(4, 3, WHITE, FONT_STANDART);
//     //     send_data_dwin("No ones devices finding'myEOF'");
//     //     dwin_end_print();
//     // }

//     // char* name_cur_device = device->name_device_sensor[0];
//     // for(uint8_t i=0; i<5; i++) {
//     //     vTaskDelay(DELAY_SHOW_ITEM);
//     //     switch(i){
//     //         case 0 :
//     //             dwin_print(1, 2, WHITE, FONT_STANDART);
//     //             send_data_dwin("Sensor devices           Timer devices'myEOF'");
//     //             dwin_end_print();
//     //             break;
//     //         case 1 :
//     //             for(uint8_t count_device=0, device_state=START_FLAG_SENSORS && name_cur_device != NULL; count_device<MAX_SLAVE_DEVICES; count_device++,device_state++,name_cur_device++) {
//     //                 if(area_SCREEN == count_device) send_str_dwin("[");
//     //                 dwin_print(2+count_device, 0, get_FLAG_STATE(device_state)?LEMON:BLUE, FONT_INFO);
//     //                 send_non_zero(count_device+1);
//     //                 send_data_dwin(". 'myEOF'");
//     //                 send_str_dwin(name_cur_device);
//     //                 if(area_SCREEN == count_device) send_str_dwin("]");
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
//     //                 if(area_SCREEN == count_device+AREA_TIMER_DEVICE_1) send_str_dwin("[");
//     //                 dwin_print(2+count_device, 0, get_FLAG_STATE(device_state)?LEMON:BLUE, FONT_INFO);
//     //                 send_non_zero(count_device+1);
//     //                 send_data_dwin(". 'myEOF'");
//     //                 send_str_dwin(name_cur_device);
//     //                 if(area_SCREEN == count_device) send_str_dwin("]");
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


// void show_rename_device_handler(void* handler_args, 
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
//                 dwin_print(1, 4, WHITE, FONT_STANDART);
//                 send_str_dwin(device_descr[device->type]);
//                 break;
//             case 1 :
//                 dwin_print(3, 2, WHITE, FONT_STANDART);
//                 send_str_dwin(device->name);
//                 break;
//         }
//         dwin_end_print();
//     }
// }

// void show_notify_handler(void* handler_args, esp_event_base_t base, int32_t day, void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL)  data_dwin = (main_data_t *)handler_args;
//     static uint8_t tmp;
//     uint8_t last_notif = NUMBER_NOTIFICATION_PER_DAY;
    
//     for(uint8_t i=0; i<6; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch(i){
//             case 0 :
//                 for(uint8_t day_count=0; day_count<SIZE_WEEK; day_count++) {
//                     dwin_print(2+day_count, 0, day == day_count ? WHITE : BLUE, FONT_INFO);
//                     send_str_dwin(WEEK_DAY[day_count]);
//                     send_str_dwin(is_ACTIVE_DAY(day_count) ? MES_ON : MES_OFF);
//                     dwin_end_print();
//                 }
//                 dwin_print(9, 0, VIOLET, FONT_INFO);
//                 send_str_dwin("\tFor\tall\tdays");
//                 break;
//             case 1 :
//                 send_in_frame(1, 1, FONT_INFO, is_ACTIVE_DAY(day)?COLOUR_ENABLE:COLOUR_DISABLE, WEEK_WHOLE_NAME[day]);
//                 break;
//             case 2 :
//                 for(int count_hour=0, area=0; count_hour<last_notif; count_hour++, area++) {
//                     tmp = GET_NOTIF_HOUR(day, count_hour);
//                     dwin_print(2+count_hour, 6, GET_COLOUR_AREA(area), FONT_STANDART);
//                     if(IS_HOUR(tmp)){
//                         send_str_dwin("\t\t+");
//                         last_notif = count_hour;
//                     } else {
//                         send_str_dwin("\t\t-\t");
//                         send_two_number(tmp);
//                         send_str_dwin("\t\t:");
//                     }
//                     dwin_end_print();
//                 }
//                 break;
//             case 3 :
//                 for(int area=AREA_MIN_1, count_min=0; count_min<last_notif; count_min++, area++) {
//                     tmp = GET_NOTIF_MIN(day, count_min);
//                     dwin_print(2+count_min, 12, GET_COLOUR_AREA(area), FONT_STANDART);
//                     send_two_number(tmp);
//                 }
//                 break;
//             case 4 :
//                 dwin_print(0, 1, color_CLOCK, FONT_INFO);
//                 send_clock(cur_HOUR, cur_MIN);
//                 break;
//             case 5 :
//                 dwin_print(4, 9, COLOUR_DISABLE, FONT_BUTTON);
//                 if(is_ACTIVE_DAY(day))send_str_dwin("SET\tOFF");
//                 else send_str_dwin("SET\tON");
//                 break;
//             default: break;
//         }
//         dwin_end_print();
//     }
// }

// void show_custom_handler(void* handler_args, 
//                                     esp_event_base_t base, 
//                                     int32_t id, 
//                                     void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL;  
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     static uint16_t color_item, color_area;
//     int i=0;
//     vTaskDelay(DELAY_SHOW_ITEM);
//     for(; i<SIZE_COLOURS_INTERFACE; ) {
//         color_area = GET_COLOUR(colors_INTERFACE[i]);
//         dwin_print(1, i*5, color_area, FONT_STANDART);
//         if(area_SCREEN == i)send_str_dwin("[");
//         send_str_dwin(massage_custom[i]);
//         if(area_SCREEN == i)send_str_dwin("]");
//         dwin_end_print();
//         i++;
//     }
//     color_area = GET_COLOUR(colors_INTERFACE[area_SCREEN]);
//     vTaskDelay(DELAY_SHOW_ITEM);
//     for(int color_it=0; color_it<SIZE_USED_COLOURS; color_it++) {
//         color_item = GET_COLOUR(color_it),
//         dwin_print(i+1, 1, color_item, FONT_STANDART);
//         if(color_item == color_area)send_str_dwin("<--");
//         send_str_dwin(massage_custom[i]);
//         dwin_end_print();
//         i++;
//     }
//     dwin_print(4, 9, COLOUR_DISABLE, FONT_BUTTON);
//     send_str_dwin(massage_custom[i]);
//     dwin_end_print();
// }

// void show_setting_handler(void* handler_args, 
//                                     esp_event_base_t base, 
//                                     int32_t id, 
//                                     void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     for(uint8_t i=0; i<6; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch (i) {
//             case 0 :
//                 dwin_print(0, 0, COLOUR_DISABLE, FONT_SECOND_INFO);
//                 send_data_dwin("| Lighting |'myEOF'");
//                 break;
//             case 1 :
//                 dwin_print(0, 7, DATA_PREPARING ? COLOUR_DISABLE : COLOUR_ENABLE, FONT_SECOND_INFO);
//                 if(DATA_PREPARING) {
//                     switch(state_WIFI) {
//                         // case WL_CONNECTED : send_data_dwin("WIFI connected'myEOF''myEOF'"); break;
//                         // case WL_NO_SSID_AVAIL : send_data_dwin("SSID no available'myEOF'"); break;
//                         // case WL_CONNECT_FAILED : send_data_dwin("wrong password WIFI'myEOF'"); break;
//                         // default : send_data_dwin("disconected"); break;
//                     }
//                     if(internet_OK) {
//                         send_data_dwin("| server OK'myEOF'");
//                     } else {
//                         send_data_dwin("| server lost'myEOF'");
//                     }
//                 } else {
//                     send_data_dwin("Сonnection attempt'myEOF'");
//                 }
//                 break;
//             case 2:
//                 dwin_print(2, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_str_dwin("SSID  : ");
//                 send_str_dwin(name_SSID);
//                 break;
//             case 3:
//                 dwin_print(3, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_str_dwin("Password : 'myEOF'");
//                 if(area_SCREEN == AREA_PASSWORD) {
//                     send_str_dwin(pwd_WIFI);
//                 } else {
//                     send_str_dwin("*****");
//                 }
//                 break;
//             case 4:
//                 dwin_print(4, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_str_dwin("Name city : ");
//                 send_str_dwin(name_CITY);
//                 break;
//             case 5:
//                 dwin_print(5, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_str_dwin("Key ");
//                 if(area_SCREEN == AREA_API) {
//                     send_str_dwin(api_KEY);
//                 } else {
//                     send_str_dwin("*****");
//                 }
//                 break;
//             default : break;
//         }
//         dwin_end_print();
//     }
// }



// void show_clock_handler(void* handler_args, 
//                                     esp_event_base_t base, 
//                                     int32_t id, 
//                                     void* event_data)
// {
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     for(uint8_t i=0; i<6; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch (i) {
//             case 0:
//                 dwin_print(0, 2, COLOUR_DISABLE, FONT_INFO);
//                 if(DATA_PREPARING) {
//                     sync_TIME
//                     ? send_data_dwin("autoupdate time'myEOF'")
//                     : send_data_dwin("manual update time'myEOF'");
//                     internet_OK
//                     ? send_data_dwin(" SNTP ok'myEOF'")
//                     : send_data_dwin(" SNTP fail'myEOF'");
//                 } else {
//                     send_data_dwin("Waiting update data...'myEOF'");
//                 }
//                 break;
//             case 1:
//                 dwin_print(3, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("year 20'myEOF'");
//                 send_two_number(cur_YEAR);
//                 break;
//             case 2:
//                 dwin_print(3, 9, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("month 'myEOF'");
//                 send_two_number(cur_MONTH);
//                 break;
//             case 3:
//                 dwin_print(3, 15, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("day 'myEOF'");
//                 send_two_number(cur_DAY);
//                 break;
//             case 4:
//                 dwin_print(5, 1, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("hour 'myEOF'");
//                 send_two_number(cur_HOUR);
//                 break;
//             case 5:
//                 dwin_print(5, 9, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("min 'myEOF'");
//                 send_two_number(cur_MIN);
//                 break;
//             case 6:
//                 dwin_print(5, 15, GET_COLOUR_AREA(i), FONT_STANDART);
//                 send_data_dwin("sec 'myEOF'");
//                 send_two_number(cur_SEC);
//                 break;
//             default : break;
//         }
//         dwin_end_print();
//     }
// }



// void show_main_handler(void* handler_args, 
//                                 esp_event_base_t base, 
//                                 int32_t data_sensor, 
//                                 void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     weather_t *data_weather = event_data;
//     size_t count = data_weather != NULL ? 5 : 3;
//     for(uint32_t i=0; i<count; i++) {
//         vTaskDelay(DELAY_SHOW_ITEM);
//         switch (i) {
//             case 0:
//                 dwin_print(2, 3, color_CLOCK, 6);
//                 send_clock(cur_HOUR, cur_MIN);
//                 break;
//             case 1:
//                 dwin_print(8, 10, color_CLOCK, 2);
//                 send_str_dwin(WEEK_DAY[cur_WEEK_DAY]);
//                 send_number(cur_DAY);
//                 break;
//             case 2:
//                 if(data_weather == NULL && data_sensor == NO_TEMP_SENSOR)return;
//                 dwin_print(0, 1, color_INFO, FONT_INFO);
//                 if(data_sensor != NO_TEMP_SENSOR) {
//                     send_data_dwin(" inside    t*C     'myEOF'");
//                     send_temperature(data_sensor);
//                 }
//                 if(data_weather != NULL) {
//                     send_data_dwin("\n outside     t*C     'myEOF'");
//                     send_temperature(temp_OUTDOR);
//                     send_data_dwin("'myEOF'\n feels like     t*C   'myEOF'");
//                     send_temperature(temp_FEELS_LIKE);
//                     send_data_dwin("'myEOF'\n rain           %     'myEOF'");
//                     send_temperature(PoP);
//                 }
//                 break;
//             case 3:
//                 dwin_print(3, 4, color_DESC, FONT_STANDART);
//                 send_str_dwin(description_WEATHER);
//                 break;
//             case 4:
//                 dwin_print(20, 12, color_INFO, FONT_SECOND_INFO);
//                 send_data_dwin("sunrise  'myEOF'");
//                 send_clock(sunrise_HOUR, sunrise_MIN);
//                 send_data_dwin("sunset  'myEOF'");
//                 send_clock(sunset_HOUR, sunset_MIN);
//                 break;
//         default   : break;
//         }
//         dwin_end_print();
//     }
// }



// void show_timer_handler(void* handler_args, 
//                                     esp_event_base_t base, 
//                                     int32_t run, 
//                                     void* event_data) 
// {
//     static const  main_data_t *data_dwin = NULL; 
//     if(data_dwin == NULL) data_dwin = (main_data_t *)handler_args;
//     uint8_t *timer_data = (uint8_t *)event_data;
//     if(timer_data == NULL)return;
//     vTaskDelay(DELAY_SHOW_ITEM);
//     dwin_print(0, 12, color_CLOCK, FONT_INFO);
//     send_clock(cur_HOUR, cur_MIN);
//     dwin_end_print();
//     if(run){
//         uint8_t column = timer_HOUR ? 1 : timer_MIN ? 2 : 4;
//         dwin_print(column, 1, color_CLOCK, 6);
//         if(send_non_zero(timer_HOUR)) {
//             send_data_dwin(" : 'myEOF'");
//             send_two_number(timer_MIN);
//             send_data_dwin(" : 'myEOF'");
//             send_two_number(timer_SEC);
//         } else if(send_non_zero(timer_MIN)) {
//             send_data_dwin(" : 'myEOF'");
//             send_two_number(timer_SEC);
//         } else {
//             send_number(timer_SEC);
//         }
//     dwin_end_print();
//     } else {
//         for(int i=0; i<SIZE_TIMER; i++) {
//             dwin_print(i*3, 1, GET_COLOUR_AREA(i), 6);
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
//                     dwin_print(h_count, w_count, LEMON, 2);
//                     send_non_zero((w_count * h_count * 3) % 100);
//                     dwin_end_print();
//                 }
//             }
//         }
//     }
//     dwin_print(3, 7, WHITE, FONT_STANDART);
//     send_str_dwin("WAIT...");
//     dwin_end_print();
// }


