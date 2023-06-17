#include "screen_handlers.h"




void info_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(command == KEY_INIT) {
        dwin_set_pic(SHOW_INFO_PIC);
    }
}

void device_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{

    if(command == KEY_INIT) {
        dwin_set_pic(SHOW_INFO_PIC);
    }
}


void search_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static wifi_ap_record_t* ap_info; 
    static uint16_t ap_count, ap_count_show;
    static bool init;
    if(command == KEY_CLOSE) {
        if(init){
            free(ap_info);
            ap_info = NULL;
            esp_wifi_clear_ap_list();
            init = false;
        }
        return;
    }
    if(command == KEY_SYNC || command == KEY_INIT) {
        if(!init) {
            esp_event_post_to(fast_service_loop, WIFI_SET_EVENTS, INIT_SCAN_NETWORKS, NULL, 0, TIMEOUT_SEND_EVENTS);
            ap_info = (wifi_ap_record_t*) calloc(MAX_SCAN_LIST_SIZE, sizeof(wifi_ap_record_t));
            if(!ap_info)return;
            init = true;
        }
        esp_event_post_to(fast_service_loop, WIFI_SET_EVENTS, START_SCAN_NETWORKS, NULL, 0, TIMEOUT_SEND_EVENTS);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
        if(ap_count > MAX_SCAN_LIST_SIZE)ap_count = MAX_SCAN_LIST_SIZE;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));
        ap_count_show = ap_count;
    } else if(KEY_IS_AREA_SCAN_SSID(command)) {
        if(area_SCREEN == GET_AREA_VALUE(command)) {
            strncpy(name_SSID, (const char*)ap_info[area_SCREEN].ssid, MAX_STR_LEN);
            dwin_set_pic(SEARCH_PIC);
            vTaskDelay(DELAY_CHANGE_PIC);
            send_message_dwin("The SSID has been set");
            vTaskDelay(DELAY_SHOW_MESSAGE);
        } else {
            if(GET_AREA_VALUE(command) < ap_count) area_SCREEN = GET_AREA_VALUE(command);
        }
    } else if(command == KEY_NEXT && ap_count > MAX_SSID_PEER_SCREEN){
        /* next or previous page */
        ap_count_show *= -1;
    }
    dwin_set_pic(SEARCH_PIC);
    show_screen(ap_count_show, &ap_info, sizeof(ap_info));
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}

void ap_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(command == KEY_INIT) {
        dwin_set_pic(SHOW_INFO_PIC);
        show_screen(UPDATE_DATA_COMPLETE, NULL, 0);
        xEventGroupSetBits(dwin_event_group, BIT_WORK_AP);
        set_new_event(INIT_AP);
    } else if(command == KEY_CLOSE) {
        xEventGroupClearBits(dwin_event_group, BIT_WORK_AP);
        esp_wifi_stop();
    } else if(command == STATION_JOINE){
        remove_periodic_event(MAIN_SCREEN);
    }
}


void setting_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t pos;
    static char *selected_buf;
    static int pic;
    static bool need_write;
    bool preparing = false;
    if(command == KEY_INIT) {
        area_SCREEN = AREA_SSID;
        selected_buf = name_SSID;
        command = KEY_START_AREA;
        pic = SETTING_LOW_LETTER_PIC;
    } else if(command == KEY_CLOSE) {
        if(need_write){
            write_memory(main_data, DATA_API);
            write_memory(main_data, DATA_CITY);
            write_memory(main_data, DATA_PWD);
            write_memory(main_data, DATA_SSID);
            need_write = false;
        }
        return;
    } else if(KEY_IS_AREA_SETTING(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
        switch(area_SCREEN) {
            case AREA_PASSWORD : selected_buf = pwd_WIFI; break;
            case AREA_CITY    : selected_buf = name_CITY; break;
            case AREA_API     : selected_buf = api_KEY;   break;
            case AREA_SSID    : selected_buf = name_SSID; break;
            default : break;
        }
        pos = strlen(selected_buf);
    } else if(command == KEY_SYNC) {
        set_new_event(GET_WEATHER);
        preparing = true;
    } else if(command == UPDATE_DATA_COMPLETE) {
        dwin_set_pic(pic);
    } else if(command == KEY_ENTER) {
        if(need_write)need_write = true;
    } else if(command == KEY_SETTING_SCREEN_LOW) {
        pic = SETTING_LOW_LETTER_PIC;
    } else if(command == KEY_SETTING_SCREEN_UP) {
        pic = SETTING_UP_LETTER_PIC;
    } else if(command == KEY_SETTING_SCREEN_SYMB) {
        pic = SETTING_SYMBOL_PIC;
    } else if(selected_buf) {
        if(command == KEY_BACKSPACE) {
            selected_buf[pos--] = 0;
        } else if(command == KEY_DELETE) {
            selected_buf[0] = 0;
            pos = 0;
        } else if(KEY_IS_SYMBOL(symbol)){
            if(pos < MAX_STR_LEN) {
                if(area_SCREEN == AREA_CITY){
                    if(KEY_IS_CHAR(symbol) 
                       || (symbol == '-' && pos != 0))
                    {
                        if(pos == 0 && symbol >= 'a' && symbol <= 'z') symbol = symbol + 'A' - 'a';
                        selected_buf[pos++] = symbol;
                    }
                } else {
                    selected_buf[pos++] = symbol;
                }
            }
        }
    }
    show_screen(preparing ? 1 : 0, NULL, 0);
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}


void main_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static bool menu_active, details, is_notify;
    static struct tm *cur_time;
    switch(command){
        case KEY_INIT :
        {
            details = false;
            menu_active = false;
            set_periodic_event(UPDATE_TIME_COMPLETE, 
                                DELAI_UPDATE_TIME_ON_SCREEN, 
                                RELOAD_COUNT);
            set_periodic_event(GET_WEATHER, 
                                DELAI_FIRST_UPDATE_WEATHER, 
                                ONLY_ONCE);
            remove_periodic_event(MAIN_SCREEN);
            cur_time = get_time_tm();
            break;
        }
        case KEY_CLOSE :
        {
            set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
            remove_periodic_event(GET_WEATHER);
            remove_periodic_event(UPDATE_TIME_COMPLETE);
            remove_periodic_event(KEY_MENU_SCREEN);
            return;
        }
        case UPDATE_WEATHER_COMPLETE :
        {
            if(weather_PIC == NO_WEATHER_PIC){
                set_periodic_event(GET_WEATHER, 
                                    DELAI_UPDATE_WEATHER_FAIL, 
                                    RELOAD_COUNT);
            } else {
                set_periodic_event(GET_WEATHER, 
                                    DELAI_UPDATE_WEATHER, 
                                    RELOAD_COUNT);
            }
            cur_time = get_time_tm();
            break;
        }
        case KEY_DETAILS_SCREEN :
        {
            if(details){
                details = false;
                set_periodic_event(UPDATE_TIME_COMPLETE, 
                                    DELAI_UPDATE_TIME_ON_SCREEN,
                                    RELOAD_COUNT);
                remove_periodic_event(KEY_DETAILS_SCREEN);
                cur_time = get_time_tm();
            } else {
                details = true;
                remove_periodic_event(UPDATE_TIME_COMPLETE);
                set_periodic_event(KEY_DETAILS_SCREEN, 
                                    30,
                                    ONLY_ONCE);
            }
            break;
        }
        case KEY_MENU_SCREEN :
        {
            if(menu_active){
                menu_active = false;
                set_periodic_event(UPDATE_TIME_COMPLETE, 
                                    DELAI_UPDATE_TIME_ON_SCREEN,
                                    RELOAD_COUNT);
                remove_periodic_event(KEY_MENU_SCREEN);
                cur_time = get_time_tm();
            } else {
                menu_active = true;
                remove_periodic_event(UPDATE_TIME_COMPLETE);
                set_periodic_event(KEY_MENU_SCREEN, 
                                    30,
                                    ONLY_ONCE);
            }
            break;
        }
        case UPDATE_TIME_COMPLETE:
        {
            cur_time = get_time_tm();
            is_notify = notification_alarm(main_data, cur_time, true);
            break;
        }
            default : break;
    }
    if(details){
        dwin_set_pic(NO_WEATHER_PIC);
        show_screen(DETAILS_SCREEN, NULL, 0);
    } else if(menu_active){
        dwin_set_pic(MENU_PIC);
    } else {
        dwin_set_pic(weather_PIC);
        show_screen(is_notify 
                        ? IS_NOTIFICATION
                        : NORMAL_SCREEN,
                    cur_time, 
                    sizeof(struct tm));
    }
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}

void clock_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static int32_t offset = -100;
    static struct tm *dwin_time;

    if(KEY_IS_AREA_CLOCK(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
    } else if(KEY_IS_NUMBER(symbol)) {
        uint8_t val = GET_NUMBER(symbol);
        switch(area_SCREEN) {
            case AREA_YEAR : 
            {
                dwin_time->tm_year = dwin_time->tm_year > 10 ? val : dwin_time->tm_year*10 + val;
                break;
            }
            case AREA_MONTH :
            {
                dwin_time->tm_mon = dwin_time->tm_mon > 1 ? val : dwin_time->tm_mon*10 + val;
                if(!IS_MONTH(dwin_time->tm_mon)) dwin_time->tm_mon = 1;
                break;
            }
            case AREA_DAY  : 
            {
                dwin_time->tm_mday = dwin_time->tm_mday > 3 ? val : dwin_time->tm_mday*10 + val;
                if(!IS_DAY(dwin_time->tm_mday)) dwin_time->tm_mday = 1;
                break;
            }
            case AREA_HOUR : 
            {
                dwin_time->tm_hour = GET_NEW_MIN_SEC_VALUE(dwin_time->tm_hour, val);
                if(!IS_HOUR(dwin_time->tm_hour)) dwin_time->tm_hour = 0;
                break; 
            }
            case AREA_MIN  : 
            {
                dwin_time->tm_min = GET_NEW_MIN_SEC_VALUE(dwin_time->tm_min, val);
                if(!IS_MIN_OR_SEC(dwin_time->tm_min)) dwin_time->tm_min = 0; 
                break; 
            }
            case AREA_SEC  : 
            {
                dwin_time->tm_sec = GET_NEW_MIN_SEC_VALUE(dwin_time->tm_sec, val);
                if(!IS_MIN_OR_SEC(dwin_time->tm_sec)) dwin_time->tm_sec = 0;
                break; 
            }
            default:break;
        }
    } else if(command == KEY_ENTER) {
        set_timezone(offset);
        write_offset(offset);
        set_time_tm(dwin_time, true);
    } else if(command == KEY_SYNC) {
        EventBits_t xEventGroup = 
                    xEventGroupGetBits(dwin_event_group);                                                                 
        if(xEventGroup&BIT_SNTP_ALLOW){
            xEventGroupClearBits(dwin_event_group, BIT_SNTP_ALLOW);
        } else {
            xEventGroupSetBits(dwin_event_group, BIT_SNTP_ALLOW);
            set_new_event(INIT_SNTP);
        }
        write_memory(main_data, DATA_FLAGS); 
    } else if(command == KEY_INIT) {
        if(!dwin_time){
            dwin_time = malloc(sizeof(struct tm));
            assert(dwin_time);
        }
        time_t raw_time =  time(NULL);
        gmtime_r(&raw_time, dwin_time);
        if(offset == -100){
            read_offset(&offset);
        }
    } else if(command == KEY_INCREMENT){
        if(offset < 23){
            offset++;
        } else {
            offset = 0;
        }  
    } else if(command == KEY_DECREMENT){
         if(offset > -23){
            offset--;
         } else {
            offset = 0;
         }
    } else if(command == KEY_CLOSE){
        free(dwin_time);
        dwin_time = NULL;
        return;
    }
    show_screen(offset, &dwin_time, sizeof(dwin_time));
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}

void state_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(command != KEY_CLOSE){
        return;
    }
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(command == KEY_SOUND_TOGGLE){
        if(xEventGroup&BIT_SOUNDS_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SOUNDS_ALLOW);
        } else {
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SOUNDS_ALLOW);
        }
    } else if(command == KEY_ESPNOW_TOGGLE){
        if(xEventGroup&BIT_ESPNOW_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_ESPNOW_ALLOW);
            set_new_event(STOP_ESPNOW);
       } else {
            set_new_event(START_ESPNOW);
            xEventGroupSetBits(dwin_event_group, BIT_ESPNOW_ALLOW);
       }
    } else if(command == KEY_SNTP_TOGGLE){
        if(xEventGroup&BIT_SNTP_ALLOW){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SNTP_ALLOW);
            set_new_event(STOP_SNTP);
        } else {
            set_new_event(INIT_SNTP);
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SNTP_ALLOW);
        }
    } else if(command == KEY_SECURITY){
        if(xEventGroup&BIT_SECURITY){
            xEventGroup = xEventGroupClearBits(dwin_event_group, BIT_SECURITY);
        } else {
            xEventGroup = xEventGroupSetBits(dwin_event_group, BIT_SECURITY);
        }
    } else if(command == KEY_ENTER){
        write_memory(NULL, DATA_FLAGS);
    }
    show_screen(UPDATE_DATA_COMPLETE, &xEventGroup, sizeof(xEventGroup));
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}

void set_color_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(command != KEY_CLOSE){
        return;
    }
    if(KEY_IS_AREA_CUSTOM(command)){
        area_SCREEN = GET_AREA_VALUE(command);
    } else if(KEY_IS_AREA_TOGGLE(command)){
            colors_INTERFACE[area_SCREEN] = GET_COLOR(GET_AREA_VALUE_TOGGLE(command));
    } else if(command == KEY_ENTER){
        write_memory(main_data, DATA_COLOR);
    }
    show_screen(UPDATE_DATA_COMPLETE, NULL, 0);
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
}


void notifications_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t cur_day, cur_notif, cur_type_data;
    if(command == KEY_CLOSE) {
        return;
    }
    if(command == KEY_INIT){
        struct tm *cut_time = get_time_tm();
        cur_day = cut_time->tm_wday;
        cur_notif = 0;
        cur_type_data = AREA_HOUR;
    } else if(KEY_IS_AREA_NOTIF(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
        cur_notif = GET_NOTIF_NUMBER(area_SCREEN);
        cur_type_data = GET_NOTIF_TYPE_DATA(area_SCREEN);
    } else if(KEY_IS_DAY(command)) {
        cur_day = GET_DAY(command);
    } else if(KEY_IS_AREA_TOGGLE_DAY(command)) {
       TOOGLE_DAY_NOTIF(GET_DAY_TOGGLE(command)); 
    } else if(KEY_IS_NUMBER(symbol)) {
        uint8_t number = GET_NUMBER(symbol);
        switch(cur_type_data){
            case AREA_HOUR : 
            {
                uint8_t new_hour = VALUE_NOTIF_HOUR(cur_notif, cur_day);
                new_hour = GET_NEW_HOUR_VALUE(new_hour, number);
                if(!IS_HOUR(new_hour)) new_hour = 0;
                SET_NOTIF_HOUR(cur_notif, cur_day, new_hour);
                break; 
            }
            case AREA_MIN  : 
            {
                uint8_t new_min = VALUE_NOTIF_MIN(cur_notif, cur_day);
                new_min = GET_NEW_MIN_SEC_VALUE(new_min, number);
                if(!IS_MIN_OR_SEC(new_min)) new_min = 0;
                SET_NOTIF_MIN(cur_notif, cur_day, new_min);
                break; 
            }
            default:break;
        }
    } else if(command == KEY_ENTER) {
        write_memory(main_data, DATA_NOTIF);
    } else if(KEY_IS_AREA_TOGGLE(command)) {
        TOOGLE_NOTIF(GET_AREA_VALUE_TOGGLE(command), cur_day);
    }
    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
    show_screen(cur_day, &cur_day, sizeof(cur_day));
}


void timer_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t new_area;
    static bool timer_run;
    static int count_buzer = NUMBER_SIG_BUZ;
    if(command == KEY_CLOSE) {
        dwin_clock_off();
        if(timer_run){
            remove_periodic_event(KEY_DECREMENT);
            timer_run = false;
        }
        return;
    }
    if(command == KEY_ENTER){
        if(!timer_run){
            remove_periodic_event(MAIN_SCREEN);
            dwin_clock_off();
            timer_run = true;
            set_periodic_event(KEY_DECREMENT, 1, RELOAD_COUNT);
        }
    } else if(command == KEY_DECREMENT) {
        if(timer_SEC == 0 && timer_MIN == 0 && timer_HOUR == 0){
            if(count_buzer){
                dwin_buzer(LOUD_BUZZER);
                count_buzer--;
            } else {
                remove_periodic_event(KEY_DECREMENT);
                set_new_event(KEY_INIT);
                count_buzer = NUMBER_SIG_BUZ;
            }
            return;
        } else {
            dwin_set_pic(TIMER_RUN_PIC);
            vTaskDelay(DELAY_SHOW_ITEM);
            timer_SEC--;
            if(timer_SEC < 0){
                timer_SEC = 59;
                timer_MIN--;
                if(timer_MIN < 0){
                    timer_MIN = 59;
                    timer_HOUR--;
                    if(timer_HOUR < 0){
                        timer_HOUR = 0;
                    }
                }
            }
        }
    } else if(command == KEY_PAUSA) {
        if(timer_run){
            dwin_clock_on(10, 200, WHITE, 2);
            dwin_set_pic(TIMER_STOP_PIC);
            remove_periodic_event(KEY_DECREMENT);
            timer_run = false;
        }
    } else if(command == KEY_INIT) {
        dwin_clock_on(10, 200, WHITE, 2);
        area_SCREEN = AREA_TIMER_MIN;
        new_area = area_SCREEN;
        dwin_set_pic(TIMER_STOP_PIC);
        vTaskDelay(DELAY_SHOW_ITEM);
        if(timer_run){
            remove_periodic_event(KEY_DECREMENT);
            timer_run = false;
        }
        timer_HOUR = 0;
        timer_MIN = 10;
        timer_SEC = 0;
    }
    if(KEY_IS_AREA_TIMERS(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
        if(new_area == area_SCREEN) {
            switch(new_area) {
                case AREA_TIMER_HOUR :
                {
                    timer_HOUR = (timer_HOUR + 1)%24; 
                    break;
                }   
                case AREA_TIMER_MIN  :
                {
                    timer_MIN += 5;
                    if(timer_MIN%5)timer_MIN = (timer_MIN/5)*5;
                    if(!IS_MIN_OR_SEC(timer_MIN))timer_MIN = 0;
                    break;
                }   
                case AREA_TIMER_SEC  : 
                {
                    timer_SEC += 10; 
                    if(timer_SEC%10)timer_SEC = (timer_SEC/10)*10;
                    if(!IS_MIN_OR_SEC(timer_SEC))timer_SEC = 0;
                }  
                default : break;
            }
        } else {
            new_area = area_SCREEN;
        } 
    } else if(KEY_IS_NUMBER(symbol)) {
        switch(area_SCREEN) {
            case AREA_TIMER_HOUR : 
            {
              timer_HOUR = GET_NEW_HOUR_VALUE(timer_HOUR, GET_NUMBER(symbol)); 
              break;
            }
            case AREA_TIMER_MIN  : 
            {
                timer_MIN = GET_NEW_MIN_SEC_VALUE(timer_MIN, GET_NUMBER(symbol));
                break;
            }
            case AREA_TIMER_SEC  :
            {
               timer_SEC = GET_NEW_MIN_SEC_VALUE(timer_SEC, GET_NUMBER(symbol)); 
               break;
            }  
            default : break;
        }
    }
    show_screen(timer_run ? TIMER_RUN : KEY_STOP, NULL, 0);
    if(!timer_run){
        set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
    }
}
