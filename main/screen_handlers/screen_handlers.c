#include "screen_handlers.h"

enum state_task{
    NO_CHANGE,
    INIT_TASK,
    DEINIT_TASK,
    INIT_FAIL,
    INIT_OK,
};

void search_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static wifi_ap_record_t* ap_info; 
    static uint16_t ap_count, ap_count_show;
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
        ap_count_show = ap_count;
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
    } else if(key == KEY_NEXT && ap_count > MAX_SSID_PEER_SCREEN){
        /* next or previous page */
        ap_count_show *= -1;
    }
    dwin_set_pic(SEARCH_PIC);
    esp_event_post_to(show_loop, EVENTS_SHOW, ap_count_show, ap_info, sizeof(ap_info), TIMEOUT_SEND_EVENTS);
}

void ap_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    if(key == KEY_INIT) {
        start_ap();
        esp_event_post_to(show_loop, EVENTS_SHOW, UPDATE_DATA_COMPLETE, NULL, 0, WAIT_PROCEES);
    } else if(key == KEY_CLOSE) {
        esp_wifi_stop();
    }
}


void setting_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t pos, max;
    static char *selected_buf;
    static int pic;
    static bool need_write;
    if(key == KEY_INIT) {
        area_SCREEN = END_AREA_SETTINGS;
        selected_buf = NULL;
        key = KEY_START_AREA;
        pic = SETTING_LOW_LETTER_PIC;
    } else if(key == KEY_CLOSE) {
        if(need_write){
            write_memory(main_data, DATA_API);
            write_memory(main_data, DATA_CITY);
            write_memory(main_data, DATA_PWD);
            write_memory(main_data, DATA_SSID);
            need_write = false;
        }
        return;
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
        get_weather();
    } else if(key == UPDATE_DATA_COMPLETE) {
        dwin_set_pic(pic);
    } else if(key == KEY_ENTER && !need_write) {
        need_write = true;
    } else if(key == KEY_SETTING_SCREEN_LOW) {
        pic = SETTING_LOW_LETTER_PIC;
    } else if(key == KEY_SETTING_SCREEN_UP) {
        pic = SETTING_UP_LETTER_PIC;
    } else if(key == KEY_SETTING_SCREEN_SYMB) {
        pic = SETTING_SYMBOL_PIC;
    } else if(selected_buf) {
        if(key == KEY_BACKSPACE) {
            selected_buf[pos--] = 0;
        } else if(key == KEY_DELETE) {
            selected_buf[0] = 0;
            pos = 0;
        } else if(KEY_IS_SYMBOL(key)){
            if(pos < max) {
                if(area_SCREEN == AREA_CITY){
                    if( KEY_IS_CHAR(key) 
                       || (key == '-' && pos != 0))
                    {
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
                        UPDATE_DATA_COMPLETE, 
                        NULL, 
                        0, 
                        TIMEOUT_SEND_EVENTS);
}

void main_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t step, last_step;
    static bool menu_active, details;
    if(key == KEY_INIT){
        step = INIT_TASK;
    } else if(key == KEY_DETAILS_SCREEN){
        if(!details){
            dwin_set_pic(NO_WEATHER_PIC);
            details = true;
            set_periodic_event( direct_loop, 
                            EVENTS_MANAGER, 
                            KEY_DETAILS_SCREEN, 
                            30,
                            ONLY_ONCE );
            return;
        } else {
            details = false;
        }
        // xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, false, false, WAIT_PROCEES);
        return;
    } else if(key == KEY_CLOSE) {
        if(menu_active)menu_active = false;
        else menu_active = true;     
        if(menu_active){
            step = DEINIT_TASK;
        } else {
            step = INIT_TASK;
        }
    } else  if(key == UPDATE_DATA_COMPLETE){
        if(weather_PIC != NO_WEATHER_PIC){
            if(step != INIT_OK) step = INIT_OK;
        } else if(step != INIT_FAIL){
            step = INIT_FAIL;
        }
    } else if(key == KEY_DEINIT){
        step = DEINIT_TASK;
        set_periodic_event( direct_loop, 
                            EVENTS_MANAGER, 
                            KEY_MAIN_SCREEN, 
                            DELAY_AUTOCLOSE,
                            ONLY_ONCE );
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
            remove_periodic_event(WIFI_SET, GET_WEATHER);
            remove_periodic_event(EVENTS_SHOW, DATA_SHOW);
            return;
        }
        case INIT_FAIL :
        {
            set_periodic_event(slow_service_loop, 
                                WIFI_SET, 
                                GET_WEATHER, 
                                DELAI_UPDATE_WEATHER_FAIL, 
                                RELOAD_COUNT);
            return;
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
    // esp_event_post_to(show_loop, 
    //         EVENTS_SHOW, 
    //         0, 
    //         NULL, 
    //         0, 
    //         TIMEOUT_SEND_EVENTS);
    show_screen(KEY_DETAILS_SCREEN, NULL);
}

void clock_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static int offset = -100;
    static struct tm * dwin_time;
    if(KEY_IS_AREA_CLOCK(key)) {
        area_SCREEN = GET_AREA_VALUE(key);
    } else if(KEY_IS_NUMBER(key)) {
        switch(area_SCREEN) {
            case AREA_YEAR : 
            {
                dwin_time->tm_year = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_year, GET_NUMBER(key));
                if(!IS_YEAR(dwin_time->tm_year)) dwin_time->tm_year = 0;
                break;
            }
            case AREA_MONTH :
            {
                dwin_time->tm_mon = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_mon, GET_NUMBER(key));
                if(!IS_MONTH(dwin_time->tm_mon)) dwin_time->tm_mon = 1;
                break;
            }
            case AREA_DAY  : 
            {
                dwin_time->tm_mday = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_mday, GET_NUMBER(key));
                if(!IS_DAY(dwin_time->tm_mday)) dwin_time->tm_mday = 1;
                break;
            }
            case AREA_HOUR : 
            {
                dwin_time->tm_hour = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_hour, GET_NUMBER(key));
                if(!IS_HOUR(dwin_time->tm_hour)) dwin_time->tm_hour = 0;
                break; 
            }
            case AREA_MIN  : 
            {
                dwin_time->tm_min = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_min, GET_NUMBER(key));
                if(!IS_MIN_OR_SEC(dwin_time->tm_min)) dwin_time->tm_min = 0; 
                break; 
            }
            case AREA_SEC  : 
            {
                dwin_time->tm_sec = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_sec, GET_NUMBER(key));
                if(!IS_MIN_OR_SEC(dwin_time->tm_sec)) dwin_time->tm_sec = 0;
                break; 
            }
            default:break;
        }
    } else if(key == KEY_ENTER) {
        set_timezone(offset);
        write_memory(main_data, DATA_OFFSET);
        set_time_tm(dwin_time, true);
    } else if(key == KEY_SYNC) {
        EventBits_t xEventGroup = 
                    xEventGroupGetBits(dwin_event_group);                                                                 
        if(xEventGroup&BIT_SNTP_ALLOW){
            xEventGroupClearBits(dwin_event_group, BIT_SNTP_ALLOW);
        } else {
            xEventGroupSetBits(dwin_event_group, BIT_SNTP_ALLOW);
        }
        write_memory(main_data, DATA_FLAGS); 
    } else if(key == KEY_INIT) {
        if(!dwin_time){
            dwin_time = malloc(sizeof(struct tm));
            assert(dwin_time);
        }
        time_t raw_time =  time(NULL);
        gmtime_r(&raw_time, dwin_time);
        if(offset == -100){
            read_memory(&offset, DATA_OFFSET);
        }
        start_sntp();
    } else if(key == KEY_INCREMENT){
        if(offset < 23){
            offset++;
        } else {
            offset = 0;
        }  
    } else if(key == KEY_DECREMENT){
         if(offset > -23){
            offset--;
         } else {
            offset = 0;
         }
    } else if(key == KEY_CLOSE){
        free(dwin_time);
        dwin_time = NULL;
    }
    esp_event_post_to(show_loop, EVENTS_SHOW, offset, dwin_time, sizeof(dwin_time), TIMEOUT_SEND_EVENTS);
}

void state_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(key == KEY_SOUND_TOGGLE){
        if(xEventGroup&BIT_SOUNDS_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SOUNDS_ALLOW);
        } else {
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SOUNDS_ALLOW);
        }
    } else if(key == KEY_ESPNOW_TOGGLE){
        if(xEventGroup&BIT_ESPNOW_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_ALLOW);
            stop_espnow();
       } else {
            start_espnow();
            xEventGroupSetBits(dwin_event_group, BIT_ESPNOW_ALLOW);
       }
    } else if(key == KEY_SNTP_TOGGLE){
        if(xEventGroup&BIT_SNTP_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SNTP_ALLOW);
            stop_sntp();
        } else {
            start_sntp();
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SNTP_ALLOW);
        }
    } else if(key == KEY_SECURITY){
        if(xEventGroup&BIT_SECURITY){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SECURITY);
        } else {
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SECURITY);
        }
    } else if(key == KEY_ENTER){
        write_memory(NULL, DATA_FLAGS);
    }
    esp_event_post_to(show_loop, EVENTS_SHOW, UPDATE_DATA_COMPLETE, &xEventGroup, sizeof(xEventGroup), TIMEOUT_SEND_EVENTS);
}

void set_color_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    if(KEY_IS_AREA_CUSTOM(key)){
        area_SCREEN = GET_AREA_VALUE(key);
    } else if(KEY_IS_AREA_TOGGLE(key)){
        uint8_t index_color = GET_AREA_VALUE_TOGGLE(key);
        if(index_color < COLOR_INTERFACE_NUMBER){
            colors_INTERFACE[area_SCREEN] = GET_COLOR(index_color);
        }
    } else if(key == KEY_ENTER){
        write_memory(main_data, DATA_COLOR);
    }
    if(key != KEY_CLOSE){
        esp_event_post_to(show_loop, EVENTS_SHOW, UPDATE_DATA_COMPLETE, NULL, 0, TIMEOUT_SEND_EVENTS);
    }
}


void notifications_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t cur_day, cur_notif, cur_type_data;
    if(key == KEY_CLOSE) {
        return;
    }
    if(key == KEY_INIT){
        cur_day = 0;
        cur_notif = 0;
        cur_type_data = AREA_HOUR;
    } else if(KEY_IS_AREA_NOTIF(key)) {
        area_SCREEN = GET_AREA_VALUE(key);
        cur_notif = GET_NOTIF_NUMBER(area_SCREEN);
        cur_type_data = GET_NOTIF_TYPE_DATA(area_SCREEN);
    } else if(KEY_IS_DAY(key)) {
        cur_day = GET_DAY(key);
    } else if(KEY_IS_AREA_TOGGLE_DAY(key)) {
       TOOGLE_DAY_NOTIF(GET_DAY_TOGGLE(key)); 
    } else if(KEY_IS_NUMBER(key)) {
        uint8_t number = GET_NUMBER(key);
        switch(cur_type_data){
            case AREA_HOUR : 
            {
                uint8_t new_hour = VALUE_NOTIF_HOUR(cur_notif, cur_day);
                new_hour = GET_NEW_TWO_DIGIT_VALUE(new_hour, number);
                if(!IS_HOUR(new_hour)) new_hour = 0;
                SET_NOTIF_HOUR(cur_notif, cur_day, new_hour);
                break; 
            }
            case AREA_MIN  : 
            {
                uint8_t new_min = VALUE_NOTIF_MIN(cur_notif, cur_day);
                new_min = GET_NEW_TWO_DIGIT_VALUE(new_min, number);
                if(!IS_MIN_OR_SEC(new_min)) new_min = 0;
                SET_NOTIF_MIN(cur_notif, cur_day, new_min);
                break; 
            }
            default:break;
        }
    } else if(key == KEY_ENTER) {
        write_memory(main_data, DATA_NOTIF);
        return;
    } else if(KEY_IS_AREA_TOGGLE(key)) {
        TOOGLE_NOTIF(GET_AREA_VALUE_TOGGLE(key), cur_day);
    }
    esp_event_post_to(show_loop, EVENTS_SHOW, cur_day, NULL, 0, TIMEOUT_SEND_EVENTS);
}



void timer_run_handler(void* data, esp_event_base_t base, int32_t key, void* event_data)
{
    if(!event_data)return;
    int8_t *timer_data = (int8_t*)data;
    static int count_buzer = NUMBER_SIG_BUZ;
    if(timer_SEC == 0 && timer_MIN == 0 && timer_HOUR){
        if(count_buzer){
            dwin_buzer(LOUD_BUZZER);
            count_buzer--;
        } else {
            esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_INIT, NULL, 0, TIMEOUT_SEND_EVENTS);
            count_buzer = NUMBER_SIG_BUZ;
            return;
        }
    } else {
        dwin_set_pic(TIMER_RUN_PIC);
        timer_SEC--;
        if(timer_SEC < 0){
            timer_SEC = 0;
            timer_MIN--;
            if(timer_MIN < 0){
                timer_MIN = 0;
                timer_HOUR--;
                if(timer_HOUR < 0){
                    timer_HOUR = 0;
                }
            }
        }
    }
    show_screen(TIMER_SHOW, timer_data);
}

void timer_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data)
{
    static uint8_t new_area;
    static int8_t *timer_data;
    static esp_event_handler_instance_t run_handler;
    if(key == KEY_CLOSE) {
        if(run_handler){
            ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                    slow_service_loop,
                    EVENTS_SHOW,
                    TIMER_SHOW,
                    run_handler
                ));
            run_handler = NULL;
        }
        return;
    }
    if(key == KEY_ENTER){
        set_periodic_event(
            slow_service_loop,
            EVENTS_SHOW, 
            TIMER_SHOW, 
            1,
            RELOAD_COUNT);
        return;
    }
    if(key == KEY_PAUSA){
        remove_periodic_event(EVENTS_SHOW, TIMER_SHOW);

    } else if(key == KEY_INIT) {
        area_SCREEN = AREA_MIN;
        if(!timer_data){
            timer_data = malloc(SIZE_TIMER);
            if(!timer_data)return;
        } else {
            dwin_set_pic(TIMER_STOP_PIC);
        }
        timer_HOUR = 0;
        timer_MIN = 10;
        timer_SEC = 0;
        if(!run_handler){
            ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                    slow_service_loop,
                    EVENTS_SHOW,
                    TIMER_SHOW,
                    timer_run_handler,
                    (void *)timer_data,
                    &run_handler
                ));
        } else {
            remove_periodic_event(EVENTS_SHOW, TIMER_SHOW);
        }
        vTaskDelay(100);
    }
    if(KEY_IS_AREA_TIMERS(key)) {
        new_area = GET_AREA_VALUE(key);
        if(new_area == area_SCREEN) {
            switch(new_area) {
                case AREA_HOUR :   timer_HOUR += 1; 
                                    if(IS_HOUR(timer_HOUR))timer_HOUR = 0; 
                                    break;
                case AREA_MIN  :   timer_MIN += 5;
                                    if(!IS_MIN_OR_SEC(timer_MIN)) timer_MIN = 0; 
                                    break;
                case AREA_SEC  :   timer_SEC += 10; 
                                    if(!IS_MIN_OR_SEC(timer_SEC)) timer_SEC = 0; 
                                    break;
                default : break;
            }
        } else {
            area_SCREEN = new_area;
        } 
    } else if(KEY_IS_NUMBER(key)) {
        switch(area_SCREEN) {
            case AREA_HOUR : 
            {
              timer_HOUR = GET_NEW_TWO_DIGIT_VALUE(timer_HOUR, GET_NUMBER(key)); 
              if(!IS_HOUR(timer_HOUR))timer_HOUR = 0; 
              break;
            }
            case AREA_MIN  : 
            {
                timer_MIN = GET_NEW_TWO_DIGIT_VALUE(timer_MIN, GET_NUMBER(key));
                if(!IS_MIN_OR_SEC(timer_MIN))timer_MIN = 0;
                break;
            }
            case AREA_SEC  :
            {
               timer_SEC = GET_NEW_TWO_DIGIT_VALUE(timer_SEC, GET_NUMBER(key)); 
               if(!IS_MIN_OR_SEC(timer_SEC))timer_SEC = 0;
               break;
            }  
            default : break;
        }
    }
    show_screen(KEY_STOP, timer_data);
}
