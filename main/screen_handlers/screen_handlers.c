#include "screen_handlers.h"

enum state_task{
    NO_CHANGE,
    INIT_TASK,
    DEINIT_TASK,
    INIT_FAIL,
    INIT_OK,
};

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
}

void ap_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(command == KEY_INIT) {
        set_new_event(INIT_AP);
    } else if(command == KEY_CLOSE) {
        esp_wifi_stop();
    }
}


void setting_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t pos, max;
    static char *selected_buf;
    static int pic;
    static bool need_write;
    bool preparing = false;
    if(command == KEY_INIT) {
        area_SCREEN = END_AREA_SETTINGS;
        selected_buf = NULL;
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
            case AREA_PASSWORD : selected_buf = pwd_WIFI; max = MAX_STR_LEN; break;
            case AREA_CITY    : selected_buf = name_CITY; max = MAX_STR_LEN; break;
            case AREA_API     : selected_buf = api_KEY; max = MAX_STR_LEN; break;
            case AREA_SSID    : selected_buf = name_SSID; max = MAX_STR_LEN; break;
            default : break;
        }
        pos = strlen(selected_buf);
    } else if(command == KEY_SYNC) {
        set_new_event(GET_WEATHER);
        preparing = true;
    } else if(command == UPDATE_DATA_COMPLETE) {
        dwin_set_pic(pic);
    } else if(command == KEY_ENTER && !need_write) {
        need_write = true;
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
        } else if(KEY_IS_SYMBOL(command)){
            if(pos < max) {
                if(area_SCREEN == AREA_CITY){
                    if( KEY_IS_CHAR(symbol) 
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
}

void main_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t step, last_step;
    static bool menu_active, details;
    if(command == KEY_INIT){
        step = INIT_TASK;
    } else if(command == KEY_DETAILS_SCREEN){
        if(!details){
            details = true;
            set_periodic_event(KEY_DETAILS_SCREEN, 
                                30,
                                ONLY_ONCE);
        } else {
            remove_periodic_event(KEY_DETAILS_SCREEN);
            details = false;
        }
    } else if(command == KEY_CLOSE) {
        menu_active = !menu_active;     
        if(menu_active){
            step = DEINIT_TASK;
        } else {
            step = INIT_TASK;
        }
    } else  if(command == UPDATE_DATA_COMPLETE){
        if(weather_PIC != NO_WEATHER_PIC){
            if(step != INIT_OK) step = INIT_OK;
        } else if(step != INIT_FAIL){
            step = INIT_FAIL;
        }
    } else if(command == KEY_DEINIT){
        step = DEINIT_TASK;
    }
if(last_step != step){
    last_step = step;
    switch(step){
        case INIT_TASK :
        {
            set_periodic_event(GET_WEATHER, 
                                DELAI_FIRST_UPDATE_WEATHER, 
                                ONLY_ONCE);
            set_periodic_event(UPDATE_DATA_COMPLETE, 
                                DELAI_UPDATE_TIME_ON_SCREEN, 
                                RELOAD_COUNT);
            break;
        }
        case DEINIT_TASK :
        {
            remove_periodic_event(GET_WEATHER);
            remove_periodic_event(UPDATE_DATA_COMPLETE);
            return;
        }
        case INIT_FAIL :
        {
            set_periodic_event(GET_WEATHER, 
                                DELAI_UPDATE_WEATHER_FAIL, 
                                RELOAD_COUNT);
            return;
        }
        case INIT_OK :
        {
            set_periodic_event(GET_WEATHER, 
                                DELAI_UPDATE_WEATHER, 
                                RELOAD_COUNT);
            break;
        }
            default : break;
        }
    
    }
    show_screen(details ? 1 : 0, NULL, 0);
    
}

void clock_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static int32_t offset = -100;
    static struct tm *dwin_time;
    if(KEY_IS_AREA_CLOCK(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
    } else if(KEY_IS_NUMBER(command)) {
        switch(area_SCREEN) {
            case AREA_YEAR : 
            {
                dwin_time->tm_year = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_year, GET_NUMBER(command));
                if(!IS_YEAR(dwin_time->tm_year)) dwin_time->tm_year = 0;
                break;
            }
            case AREA_MONTH :
            {
                dwin_time->tm_mon = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_mon, GET_NUMBER(command));
                if(!IS_MONTH(dwin_time->tm_mon)) dwin_time->tm_mon = 1;
                break;
            }
            case AREA_DAY  : 
            {
                dwin_time->tm_mday = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_mday, GET_NUMBER(command));
                if(!IS_DAY(dwin_time->tm_mday)) dwin_time->tm_mday = 1;
                break;
            }
            case AREA_HOUR : 
            {
                dwin_time->tm_hour = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_hour, GET_NUMBER(command));
                if(!IS_HOUR(dwin_time->tm_hour)) dwin_time->tm_hour = 0;
                break; 
            }
            case AREA_MIN  : 
            {
                dwin_time->tm_min = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_min, GET_NUMBER(command));
                if(!IS_MIN_OR_SEC(dwin_time->tm_min)) dwin_time->tm_min = 0; 
                break; 
            }
            case AREA_SEC  : 
            {
                dwin_time->tm_sec = GET_NEW_TWO_DIGIT_VALUE(dwin_time->tm_sec, GET_NUMBER(command));
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
        set_new_event(INIT_SNTP);
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
    }
    show_screen(offset, &dwin_time, sizeof(dwin_time));
}

void state_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
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
}

void set_color_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    if(KEY_IS_AREA_CUSTOM(command)){
        area_SCREEN = GET_AREA_VALUE(command);
    } else if(KEY_IS_AREA_TOGGLE(command)){
            colors_INTERFACE[area_SCREEN] = GET_COLOR(GET_AREA_VALUE_TOGGLE(command));
    } else if(command == KEY_ENTER){
        write_memory(main_data, DATA_COLOR);
    }
    if(command != KEY_CLOSE){
        show_screen(UPDATE_DATA_COMPLETE, NULL, 0);
    }
}


void notifications_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t cur_day, cur_notif, cur_type_data;
    if(command == KEY_CLOSE) {
        return;
    }
    if(command == KEY_INIT){
        cur_day = 0;
        cur_notif = 0;
        cur_type_data = AREA_YEAR;
        

    } else if(KEY_IS_AREA_NOTIF(command)) {
        area_SCREEN = GET_AREA_VALUE(command);
        cur_notif = GET_NOTIF_NUMBER(area_SCREEN);
        cur_type_data = GET_NOTIF_TYPE_DATA(area_SCREEN);
    } else if(KEY_IS_DAY(command)) {
        cur_day = GET_DAY(command);
    } else if(KEY_IS_AREA_TOGGLE_DAY(command)) {
       TOOGLE_DAY_NOTIF(GET_DAY_TOGGLE(command)); 
    } else if(KEY_IS_NUMBER(command)) {
        uint8_t number = GET_NUMBER(command);
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
    } else if(command == KEY_ENTER) {
        write_memory(main_data, DATA_NOTIF);
        return;
    } else if(KEY_IS_AREA_TOGGLE(command)) {
        TOOGLE_NOTIF(GET_AREA_VALUE_TOGGLE(command), cur_day);
    }
    show_screen(cur_day, &cur_day, sizeof(cur_day));
}


void timer_screen_handler(main_data_t* main_data, uint8_t command, char symbol)
{
    static uint8_t new_area;
    static bool timer_run;
    static int count_buzer = NUMBER_SIG_BUZ;
    if(command == KEY_CLOSE) {
        if(timer_run){
            remove_periodic_event(KEY_DECREMENT);
            timer_run = false;
        }
        return;
    }
    if(command == KEY_ENTER){
        if(!timer_run){
            timer_run = true;
            set_periodic_event(KEY_DECREMENT, 1, RELOAD_COUNT);
        }
    } else if(command == KEY_DECREMENT) {
        if(timer_SEC == 0 && timer_MIN == 0 && timer_HOUR){
            if(count_buzer){
                dwin_buzer(LOUD_BUZZER);
                count_buzer--;
                return;
            } else {
                dwin_set_pic(TIMER_STOP_PIC);
                timer_run = false;
                count_buzer = NUMBER_SIG_BUZ;
                remove_periodic_event(KEY_DECREMENT);
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
    } else if(command == KEY_PAUSA) {
        if(timer_run){
            remove_periodic_event(KEY_INCREMENT);
            timer_run = false;
        }
    } else if(command == KEY_INIT) {
        area_SCREEN = AREA_MIN;
        if(timer_run){
            remove_periodic_event(KEY_INCREMENT);
            timer_run = false;
        }
        timer_HOUR = 0;
        timer_MIN = 10;
        timer_SEC = 0;
    }
    if(KEY_IS_AREA_TIMERS(command)) {
        new_area = GET_AREA_VALUE(command);
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
    } else if(KEY_IS_NUMBER(command)) {
        switch(area_SCREEN) {
            case AREA_HOUR : 
            {
              timer_HOUR = GET_NEW_TWO_DIGIT_VALUE(timer_HOUR, GET_NUMBER(command)); 
              if(!IS_HOUR(timer_HOUR))timer_HOUR = 0; 
              break;
            }
            case AREA_MIN  : 
            {
                timer_MIN = GET_NEW_TWO_DIGIT_VALUE(timer_MIN, GET_NUMBER(command));
                if(!IS_MIN_OR_SEC(timer_MIN))timer_MIN = 0;
                break;
            }
            case AREA_SEC  :
            {
               timer_SEC = GET_NEW_TWO_DIGIT_VALUE(timer_SEC, GET_NUMBER(command)); 
               if(!IS_MIN_OR_SEC(timer_SEC))timer_SEC = 0;
               break;
            }  
            default : break;
        }
    }
    show_screen(timer_run ? TIMER_RUN : KEY_STOP, NULL, 0);
}
