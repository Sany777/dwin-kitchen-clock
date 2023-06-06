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
        ESP_ERROR_CHECK(esp_wifi_scan_GET_ap_num(&ap_count));
        if(ap_count > MAX_SCAN_LIST_SIZE)ap_count = MAX_SCAN_LIST_SIZE;
        ESP_ERROR_CHECK(esp_wifi_scan_GET_ap_records(&ap_count, ap_info));
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
  
}

void setting_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t pos, max;
    static char *selected_buf;
    static int pic = SETTING_LOW_LETTER_PIC;
    if(key == KEY_CLOCK_SCREEN){
        return;
    }
    if(key == KEY_INIT) {
        key = KEY_START_AREA;
    } else if(KEY_IS_AREA_SETTING(key)) {
        area_SCREEN = GET_AREA_VALUE(key);
        switch(area_SCREEN) {
            case AREA_PASSWORD : selected_buf = pwd_WIFI; max = MAX_STR_LEN; break;
            case AREA_CITY    : selected_buf = name_CITY; max = MAX_STR_LEN; break;
            case AREA_API     : selected_buf = api_KEY; max = MAX_STR_LEN; break;
            case AREA_SSID    : selected_buf = name_SSID; max = MAX_STR_LEN; break;
            default : break;
        }
        pos = strlen(selected_buf);
    } else if(key == KEY_SYNC) {
        start_sta();
        GET_weather();
    } else if(key == KEY_ENTER) {
        write_memory(main_data, DATA_API);
        write_memory(main_data, DATA_CITY);
        write_memory(main_data, DATA_PWD);
        write_memory(main_data, DATA_SSID);
    } else if(key == KEY_SETTING_SCREEN_LOWER) {
        pic = SETTING_LOW_LETTER_PIC;
    } else if(key == KEY_SETTING_SCREEN_UPER) {
        pic = SETTING_UP_LETTER_PIC;
    } else if(key == KEY_SETTING_SCREEN_SYMB) {
        pic = SETTING_SYMBOL_PIC;
    } else if(selected_buf) {
        if(key == KEY_BACSPACE) {
            selected_buf[pos--] = 0;
        } else if(key == KEY_DELETE) {
            selected_buf[0] = 0;
            pos = 0;
        } else if(KEY_IS_SYMBOL(key)){
            if(pos < max) {
                if(area_SCREEN == AREA_CITY){
                    if(KEY_IS_CHAR(key) || key == '-' && pos != 0 ){
                        if(pos == 0 && key >= 'a' && key <= 'z') key = key + 'A' - 'a';
                        selected_buf[pos++] = key;
                    }
                } else {
                    selected_buf[pos++] = key;
                }
            }
        }
    }
    esp_event_post_to(show_loop, 
                    EVENTS_SHOW, 
                    KEY_UPDATE_SCREEN, 
                    NULL, 
                    0, 
                    TIMEOUT_SEND_EVENTS);
}

enum state_task{
    NO_CHANGE,
    INIT_TASK,
    DEINIT_TASK,
    INIT_FAIL,
    INIT_OK,
};

void main_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t step, last_step;
    if(key == KEY_INIT){
        step = INIT_TASK;
    } else if(key == KEY_CLOSE) {
        menu_active != menu_active;        
        if(menu_active){
            step = DEINIT_TASK;
        } else {
            step = INIT_TASK;
        }
    } else  if(key == UPDATE_DATA_COMPLETE){
        dwin_set_pic(weather_PIC);
        if(weather_PIC != NO_WEATHER_PIC){
            if(mode != INIT_OK) mode = INIT_OK;
        } else if(mode != INIT_FAIL){
            mode = INIT_FAIL;
        }
    }
if(last_step != step){
    last_step = step;
    switch(step){
        case INIT_TASK :
        {
            set_periodic_event(slow_service_loop,
                        WIFI_SET,
                        GET_WEATHER,
                        DELAI_FIRST_UPDATE_WEATHER,
                        ONLY_ONCE);
            set_periodic_event(show_loop,
                        EVENTS_SHOW, 
                        DATA_SHOW, 
                        DELAI_UPDATE_TIME_ON_SCREEN, 
                        RELOAD_COUNT);
            break;
        }
        case DEINIT_TASK :
        {
            remove_periodic_event(slow_service_loop, WIFI_SET, GET_WEATHER);
            remove_periodic_event(EVENTS_SHOW, DATA_SHOW);
        }
        case INIT_FAIL :
        {
            set_periodic_event(slow_service_loop, 
                                WIFI_SET, 
                                GET_WEATHER, 
                                DELAI_UPDATE_WEATHER_FAIL, 
                                RELOAD_COUNT);
            break;
        }
        case INIT_OK :
        {
            set_periodic_event(slow_service_loop, 
                                    WIFI_SET, 
                                    GET_WEATHER, 
                                    DELAI_UPDATE_WEATHER, 
                                    RELOAD_COUNT);
            break;
        }
            default : break;
        }
    
    }
    if(step != DEINIT_TASK){
        esp_event_post_to(show_loop, 
                EVENTS_SHOW, 
                KEY_UPDATE_SCREEN, 
                NULL, 
                0, 
                TIMEOUT_SEND_EVENTS);
    }
}

void clock_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    if(KEY_IS_AREA_CLOCK(key)) {
        area_SCREEN = GET_AREA_VALUE(key);
    } else if(KEY_IS_NUMBER(key)) {
        switch(area_SCREEN) {
            case AREA_YEAR : 
            {
                dwin_time.tm_year = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_year, GET_NUMBER(key));
                if(!IS_YEAR(dwin_time.tm_year)) dwin_time.tm_year = 0; break;
                break;
            }
            case AREA_MONTH :
            {
                dwin_time.tm_mon = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_mon, GET_NUMBER(key));
                if(!IS_MONTH(dwin_time.tm_mon)) dwin_time.tm_mon = 1;
                break;
            }
            case AREA_DAY  : 
            {
                dwin_time.tm_mday = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_mday, GET_NUMBER(key));
                if(!IS_DAY(dwin_time.tm_mday)) dwin_time.tm_mday = 1;
                break;
            }
            case AREA_HOUR : 
            {
                dwin_time.tm_hour = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_hour, GET_NUMBER(key));
                if(!IS_HOUR(dwin_time.tm_hour)) dwin_time.tm_hour = 0;
                break; 
            }
            case AREA_MIN  : 
            {
                dwin_time.tm_min = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_min, GET_NUMBER(key));
                if(!IS_MIN_OR_SEC(dwin_time.tm_min)) dwin_time.tm_min = 0; 
                break; 
            }
            case AREA_SEC  : 
            {
                dwin_time.tm_sec = GET_NEW_TWO_DIGIT_VALUE(dwin_time.tm_sec, GET_NUMBER(key));
                if(!IS_MIN_OR_SEC(dwin_time.tm_sec)) dwin_time.tm_sec = 0;
                break; 
            }
            default:break;
        }
    } else if(key == KEY_ENTER) {
        dwin_clock_set(&dwin_time);
    } else if(key == KEY_SYNC) {
        EventBits_t xEventGroup = 
                    xEventGroupGetBits(dwin_event_group);                                                                 
        if(xEventGroup&BIT_SYNC_TIME_ALLOW){
            xEventGroupClearBits(dwin_event_group, BIT_SYNC_TIME_ALLOW);
        } else {
            xEventGroupSetBits(dwin_event_group, BIT_SYNC_TIME_ALLOW);
        }
        write_memory(main_data, DATA_FLAGS); 
    } else if(key == KEY_INIT) {
        start_sntp();
    }
    esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
}

void set_color_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
 
}


void notification_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{  	
    static uint8_t new_value, cur_day;
    if(key == KEY_CLOSE) {
        return;
    }
    if(KEY_IS_AREA_NOTIF(key)) {
        area_SCREEN = GET_AREA_VALUE(key);
    } else if(KEY_IS_DAY(key)) {
        cur_day = GET_DAY(key);
    } else if(KEY_IS_AREA_TOGGLE_DAY(key)) {
        TOOGLE_NOTIF(GET_DAY_TOGGLE(key));
    } else if(KEY_IS_NUMBER(key)) {
        switch(area_SCREEN){
            case AREA_HOUR : 
            {
                if(!IS_HOUR(new_val)) new_val = 0;
                dwin_time.tm_hour = new_val;
                break; 
            }
            case AREA_MIN  : 
            {
                if(!IS_MIN_OR_SEC(new_val)) new_val = 0; 
                dwin_time.tm_min = new_val;
                break; 
            }
            case AREA_SEC  : 
            {
                if(!IS_MIN_OR_SEC(new_val)) new_val = 0;
                dwin_time.tm_sec = new_val;
                break; 
            }
        }




        new_value = GET_NUMBER(key);
        if(IS_AREA_HOUR) {
            if(IS_HOUR(new_value)) {
                notification_DATA[area_SCREEN] = new_value;
            }
        } else if(IS_MIN_OR_SEC(new_value)) {
            cur_CLOCK[area_SCREEN] = new_value;
        }
    } else if(key == KEY_ENTER) {
        write_memory(main_data, DATA_NOTIF);
        return;
    } else if(KEY_IS_AREA_TOGGLE(key)) {
        
        
    } else if(key == KEY_CLOSE) {
        
        return;
    }
    dwin_set_pic(SEARCH_PIC);
    esp_event_post_to(show_loop, EVENTS_SHOW, cur_day, NULL, 0, TIMEOUT_SEND_EVENTS);
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
    //     if(KEY_IS_AREA_TIMERS(key)) {
    //         new_area = GET_AREA_VALUE(key);
    //         if(new_area == area_SCREEN) {
    //             switch(new_area) {
    //                 case AREA_HOUR :   timer_HOUR += 1; 
    //                                     if(IS_HOUR(timer_HOUR)){timer_HOUR = 0;} break;
    //                 case AREA_MIN  :   timer_MIN += 5;
    //                                     if(!IS_MIN_OR_SEC(timer_MIN)) {timer_MIN = 0;} break;
    //                 case AREA_SEC  :   timer_SEC += 10; 
    //                                     if(!IS_MIN_OR_SEC(timer_SEC)) {timer_SEC = 0;} break;
    //                 default : break;
    //             }
    //         } else {
    //             area_SCREEN = new_area;
    //         } 
    //     } else if(KEY_IS_NUMBER(key)) {
    //         uint8_t number = GET_NUMBER(key);
    //         switch(area_SCREEN) {
    //             case AREA_HOUR :   timer_HOUR = GET_NEW_TWO_DIGIT_VALUE(timer_HOUR, number); 
    //                                 if(IS_HOUR(timer_HOUR)){timer_HOUR = 0;} break;
    //             case AREA_MIN  :   timer_MIN = GET_NEW_TWO_DIGIT_VALUE(timer_MIN, number);
    //                                 if(!IS_MIN_OR_SEC(timer_MIN)){timer_MIN = 0;} break;
    //             case AREA_SEC  :   timer_SEC = GET_NEW_TWO_DIGIT_VALUE(timer_SEC, number); 
    //                                 if(!IS_MIN_OR_SEC(timer_SEC)){timer_SEC = 0;} break;
    //             default : break;
    //         }
    //     } 
    // }

    // dwin_set_pic(SEARCH_PIC);
    // esp_event_post_to(show_loop, EVENTS_SHOW, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
}
