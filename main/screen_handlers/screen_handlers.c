#include "screen_handlers.h"


void search_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static wifi_ap_record_t* ap_info; 
    static uint16_t ap_count;
    static bool init;
    if(key == KEY_CLOSE) {
        if(init){
            free(ap_info);
            ap_info = NULL;
            esp_wifi_clear_ap_list();
            init = false;
        }
        return;
    }
    if(key == KEY_SYNC || key == KEY_INIT) {
        if(!init) {
            esp_event_post_to(fast_service_loop, WIFI_SET, INIT_SCAN_NETWORKS, NULL, 0, TIMEOUT_SEND_EVENTS);
            ap_info = (wifi_ap_record_t*) calloc(MAX_SCAN_LIST_SIZE, sizeof(wifi_ap_record_t));
            if(!ap_info)return;
            init = true;
        }
        esp_event_post_to(fast_service_loop, WIFI_SET, START_SCAN_NETWORKS, NULL, 0, TIMEOUT_SEND_EVENTS);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
        if(ap_count > MAX_SCAN_LIST_SIZE)ap_count = MAX_SCAN_LIST_SIZE;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));
    } else if(KEY_IS_AREA_SCAN_SSID(key)) {
        if(area_SCREEN == GET_AREA_VALUE(key)) {
            strncpy(name_SSID, (const char*)ap_info[area_SCREEN].ssid, MAX_STR_LEN);
            dwin_set_pic(SEARCH_PIC);
            vTaskDelay(DELAY_CHANGE_PIC);
            send_message_dwin("The SSID has been set");
            vTaskDelay(DELAY_SHOW_MESSAGE);
        } else {
            if(GET_AREA_VALUE(key) < ap_count) area_SCREEN = GET_AREA_VALUE(key);
        }
    }
    dwin_set_pic(SEARCH_PIC);
    esp_event_post_to(show_loop, EVENTS_SHOW, ap_count, ap_info, sizeof(ap_info), TIMEOUT_SEND_EVENTS);
}

void ap_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{

    

    if(key == KEY_INIT) {
        start_ap();
        dwin_set_pic(NO_WEATHER_PIC);
        esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
    } else if(key == KEY_CLOSE) {

        start_sta();
    }

}



void device_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    // 
    
    // if(key == KEY_DELETE_DATA && device_show) {
    //     free(device_show);
    //     return;
    // }
    // if(key == KEY_CLOSE) {

    //     return;
    // }
    // dwin_set_pic(SEARCH_PIC);
    // esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, device_show, sizeof(device_inf_t), TIMEOUT_SEND_EVENTS);
}

void setting_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{

    // ESP_LOGI(TAG," %s", pwd_WIFI);
    	
    // static uint8_t pos, max;
    // static char *selected_buf = NULL;
    
    // if(key == KEY_DELETE_DATA || key == KEY_CLOSE) {
    //     return;
    // }

    // if(key == KEY_INIT) {
    //     key = KEY_START_AREA;
    //     set_ON_STATE(DATA_PREPARING);
    //     // xTaskNotify(task_service_wifi, INIT_STA, eSetBits);
    // }
    // if(key_is_AREA_SETTING(key)) {
    //     area_SCREEN = GET_AREA_VALUE(key);
    //     switch(area_SCREEN) {
    //         case AREA_PASSWORD : selected_buf = pwd_WIFI; max = SIZE_BUF; break;
    //         case AREA_CITY     : selected_buf = name_CITY; max = SIZE_BUF; break;
    //         case AREA_API      : selected_buf = api_KEY; max = SIZE_BUF_API; break;
    //         case AREA_SSID     : selected_buf = name_SSID; max = SIZE_BUF; break;
    //         default : break;
    //     }
    //     pos = strlen(selected_buf);
    // } else if(key_is_CHAR(key)) {
    //     if(pos < max-1) {
    //         selected_buf[pos++] = key;
    //     }
    // } else if(key == KEY_BACSPACE) {
    //     selected_buf[pos--] = '\0';
    // } else if(key == KEY_DELETE) {
    //     bzero(selected_buf, pos+1);
    // } else if(key == KEY_ENTER) {
    //     write_memory(main_data, DATA_API);
    //     write_memory(main_data, DATA_CITY);
    //     write_memory(main_data, DATA_PWD);
    //     write_memory(main_data, DATA_SSID);
    // } else if(key == KEY_SYNC && strlen(name_SSID) && strlen(pwd_WIFI) > 7) {
    //         set_OFF_STATE(DATA_PREPARING);
    //         // xTaskNotify(task_service_wifi, INIT_STA, eSetBits);
    // }

    // dwin_set_pic(SEARCH_PIC);
    // esp_event_post_to(show_loop, 
    //                 EVENTS_SHOW, 
    //                 KEY_UPDATE_SCREEN, 
    //                 NULL, 
    //                 0, 
    //                 TIMEOUT_SEND_EVENTS);
}

void main_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
// 	
//     static weather_t *data_weather = NULL;
//     static temperature_t temperature_indoor = NO_TEMP_SENSOR;
//     static bool menu_active = false;
//     if(key == KEY_DELETE_DATA) {
//         if(data_weather) free(data_weather);
//         return;
//     }

//     if(key == KEY_INIT) {
//         if(data_weather == NULL)data_weather = calloc(1, sizeof(weather_t));
//     } else if(key == KEY_CLOSE) {
//         menu_active = !menu_active;
//         if(menu_active) {

//           dwin_set_pic(MENU_PIC);
//           return;
//         } 
//     }

//     dwin_set_pic(SEARCH_PIC);
//     bool is_data_weather = data_weather != NULL && data_weather->weather_pic;
//     esp_event_post_to(show_loop,
//                 EVENTS_SHOW, 
//                 (int32_t)temperature_indoor, 
//                 is_data_weather 
//                  ? data_weather
//                  : NULL,
//                 is_data_weather 
//                  ? sizeof(weather_t *)
//                  : 0, 
//                 TIMEOUT_SEND_EVENTS);
// }

// void clock_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
// {
//     

//     if(key_is_AREA_CLOCK(key)) {
//         area_SCREEN = GET_AREA_VALUE(key);
//     } else if(key_is_NUMBER(key)) {
//         selected_VALUE_CLOCK = get_NEW_TWO_DIGIT_VALUE(selected_VALUE_CLOCK, get_NUMBER(key));
//         switch(area_SCREEN) {
//             case AREA_YEAR  : if(!is_YEAR(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 0; break;
//             case AREA_MONTH : if(!is_MONTH(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 1; break;
//             case AREA_DAY   : if(!is_DAY(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 1; break;
//             case AREA_HOUR  : if(IS_HOUR(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 0; break;
//             case AREA_MIN   : if(!is_MIN_OR_SEC(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 0; break;
//             case AREA_SEC   : if(!is_MIN_OR_SEC(selected_VALUE_CLOCK)) selected_VALUE_CLOCK = 0; break;
//             default:break;
//         }
//     } else if(key == KEY_ENTER) {
//         dwin_clock_set(main_data);
//     } else if(key == KEY_SYNC) {
//         set_TOGGLE_STATE(SYNC_TIME);
//         if(sync_TIME) {

//         } else {
//             dwin_clock_get();
//         }
//         set_OFF_STATE(DATA_PREPARING);
//     } else if(key == KEY_INIT) {
//         dwin_clock_on(20, 300, WHITE, 2);
//         dwin_set_pic(CLOCK_PIC);
//         set_OFF_STATE(DATA_PREPARING);
//         dwin_clock_get();
//     } 
    

//     dwin_set_pic(SEARCH_PIC);
//     esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);


}

void set_color_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    // 	
    // uint32_t  value_color=0;
 
    //     if(key_is_AREA_CUSTOM(key)) {
    //         area_SCREEN = GET_AREA_VALUE(key);
    //     } else if(key_is_NUMBER(key)) {
    //         colors_INTERFACE[area_SCREEN] = get_NUMBER(key);
    //     } else if(key == KEY_ENTER) {
    //         write_memory(main_data, DATA_COLOUR);
    //     } else if(key == KEY_INIT ) {
    //          dwin_set_pic(CUSTOM_COLOURS_PIC);
    //     } else if(key == KEY_CLOSE) {

    //         return;
    //     }
    // if(key != KEY_CLOSE && key != KEY_DELETE_DATA) {
    //     dwin_set_pic(SEARCH_PIC);
    //     esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
    // }
}


void notification_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    // 	
    // static uint8_t new_value, cur_day;
    // if(key == KEY_CLOSE || key == KEY_DELETE_DATA) {
    //     return;
    // }
    //     if(key_is_AREA_NOTIF(key)) {
    //         area_SCREEN = GET_AREA_VALUE(key);
    //     } else if(key_is_DAY(key)) {
    //         cur_day = get_DAY(key);
    //     } else if(key_is_AREA_TOGGLE_DAY(key)) {
    //         set_TOGGLE_ACTIVE_DAY(get_DAY_TOGGLE(key));
    //     } else if(key_is_NUMBER(key)) {
    //         new_value = get_NUMBER(key);
    //         if(is_AREA_HOUR) {
    //             if(is_HOUR(new_value)) {
    //                 cur_CLOCK[area_SCREEN] = new_value;
    //             }
    //         } else if(is_MIN_OR_SEC(new_value)) {
    //             cur_CLOCK[area_SCREEN] = new_value;
    //         }
    //     } else if(key == KEY_ENTER) {
    //         write_memory(main_data, DATA_NOTIF);
    //         return;
    //     } else if(key_is_AREA_TOGGLE(key)) {
            
            
    //     } else if(key == KEY_CLOSE) {
          
    //         return;
    //     }
    //     dwin_set_pic(SEARCH_PIC);
    //     esp_event_post_to(show_loop, EVENTS_SHOW, cur_day, NULL, 0, TIMEOUT_SEND_EVENTS);
}


void timer_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    // 
    // static int8_t *timer_data = NULL;
    // static uint8_t new_area;
    // static bool run=false;
    // if(key == KEY_DELETE_DATA || key == KEY_CLOSE) {
    //     return;
    // }

    // if(key == KEY_ENTER) {
    //     run =! run;
    //     if(run) {
    //         dwin_set_pic(TIMER_RUN_PIC);

    //         return;
    //     } else  {

    //         dwin_set_pic(TIMER_STOP_PIC);
    //     }
    // }
    // else if(key == KEY_INIT) {
    //     dwin_set_pic(TIMER_STOP_PIC);
    //     if(timer_data == NULL)timer_data = (int8_t *)calloc(SIZE_TIMER, sizeof(int8_t));
    //     else bzero(timer_data, SIZE_TIMER);
    //     check_NULL(timer_data);

    //     run = false;
    // } else if(key == KEY_CLOSE) {

    //     return;
    // }
    // if(!run) {
    //     if(key_is_AREA_TIMERS(key)) {
    //         new_area = GET_AREA_VALUE(key);
    //         if(new_area == area_SCREEN) {
    //             switch(new_area) {
    //                 case AREA_HOUR  :   timer_HOUR += 1; 
    //                                     if(IS_HOUR(timer_HOUR)){timer_HOUR = 0;} break;
    //                 case AREA_MIN   :   timer_MIN += 5;
    //                                     if(!is_MIN_OR_SEC(timer_MIN)) {timer_MIN = 0;} break;
    //                 case AREA_SEC   :   timer_SEC += 10; 
    //                                     if(!is_MIN_OR_SEC(timer_SEC)) {timer_SEC = 0;} break;
    //                 default : break;
    //             }
    //         } else {
    //             area_SCREEN = new_area;
    //         } 
    //     } else if(key_is_NUMBER(key)) {
    //         uint8_t number = get_NUMBER(key);
    //         switch(area_SCREEN) {
    //             case AREA_HOUR  :   timer_HOUR = get_NEW_TWO_DIGIT_VALUE(timer_HOUR, number); 
    //                                 if(IS_HOUR(timer_HOUR)){timer_HOUR = 0;} break;
    //             case AREA_MIN   :   timer_MIN = get_NEW_TWO_DIGIT_VALUE(timer_MIN, number);
    //                                 if(!is_MIN_OR_SEC(timer_MIN)){timer_MIN = 0;} break;
    //             case AREA_SEC   :   timer_SEC = get_NEW_TWO_DIGIT_VALUE(timer_SEC, number); 
    //                                 if(!is_MIN_OR_SEC(timer_SEC)){timer_SEC = 0;} break;
    //             default : break;
    //         }
    //     } 
    // }

    // dwin_set_pic(SEARCH_PIC);
    // esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
}
