#include "show_screen_handlers.h"

const int NORMAL_FONT = 2, FONT_INFO = 2, FONT_SECOND_INFO = 1, FONT_BUTTON = 3;

const char *MES_ON = "[ON]";
const char *MES_OFF = "[OFF]";

const char *WEEK_DAY[SIZE_WEEK] = {
    "Sunday",
    "Monday",
    "Tuesday", 
    "Wednesday", 
    "Thursday", 
    "Friday", 
    "Saturday"
};

const char* ITEM_COLOR_NAME[] = {
    "Clock", 
    "Desc", 
    "Info", 
};

const char* COLOR_NAME[] = { 
    "WHITE", 
    "CAEN",
    "BLUE", 
    "GREY",
    "VIOLET", 
    "LEMON", 
    "RED", 
    "GREEN",
    "AQUA",
    "ORANGE"
};

void show_info_handler(main_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{

}

void show_device_handler(main_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{
    
}

void show_ap_handler(main_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{
    dwin_set_pic(INFO_PIC);
    vTaskDelay(DELAY_SHOW_ITEM);
    print_start(2, 2, WHITE, FONT_INFO);
    if(event_data == NULL){
        send_str(
                "Connect to the AP \n\r with the name: \"%s\",\r\n and enter the paswword: \"%s\".",
                AP_WIFI_SSID,
                AP_WIFI_PWD);
    } else {
        uint8_t *mac = (uint8_t *)event_data;
        if(state == STATION_JOINE){
           send_str( 
                "Station "MACSTR" join.\r\n Follow the link \"%s\"", MAC2STR(mac), MY_IP);
        } else {
            send_str("Station "MACSTR" leave.", MAC2STR(mac));
        }
    }
    print_end();
}

/* if negative ssid number need show tail data */
void show_ssid_handler(main_data_t * main_data,
                                int32_t ap_count, 
                                void* event_data) 
{
    wifi_ap_record_t *ap_info = *(wifi_ap_record_t**)event_data;
    print_start(1, 0, color_DESC, NORMAL_FONT);
    send_str("N|       SSID         |RSSI |Ch.");
    print_end();
    bool first_page;
    int i = 0;
    if(ap_count < 0){
        ap_count = ap_count*-1 - MAX_SSID_PEER_SCREEN;
        first_page = false;
        ap_info += MAX_SSID_PEER_SCREEN;
        i = MAX_SSID_PEER_SCREEN;
    } else{
        first_page = true;
    }
    for (; i<ap_count && i<MAX_SSID_PEER_SCREEN; i++, ap_info++) {
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(i+1, 0, GET_COLOR_AREA(i), NORMAL_FONT);
        send_str("\n\r%-d| %18.18s | %2.2d | %d ", i+1, ap_info->ssid, ap_info->rssi, ap_info->primary);
        if(ap_count > MAX_SSID_PEER_SCREEN && i == MAX_SSID_PEER_SCREEN-1) send_str_dwin(first_page ? "  >>" : "<<");
        print_end();
    }
}



void show_settings_handler(main_data_t * main_data,
                            int32_t id, 
                            void* event_data) 
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    for(uint8_t i=0; i<5; i++) {
        vTaskDelay(DELAY_SHOW_ITEM/2);
        switch (i) {
            case 0 :
                print_start(0, 7, xEventGroup&BIT_PROCESS 
                                    ? COLOR_ENABLE
                                    : COLOR_DISABLE,
                                    FONT_SECOND_INFO);
                if(xEventGroup&BIT_PROCESS){
                    send_str("Connection attempt...");
                }else if(xEventGroup&BIT_CON_STA_OK) {
                    send_str("WiFi Ok |%s",
                        xEventGroup&BIT_WEATHER_OK
                        ? "weather OK"
                        : xEventGroup&BIT_RESPONSE_400_SERVER
                            ? "bad key"
                            : "no internet");
                } else if(xEventGroup&BIT_SSID_FOUND){
                    send_str(" Wrong password WiFi");
                } else {
                    send_str(" SSID not found");
                }
                break;
            case 1:
                print_start(2, 1, GET_COLOR_AREA(AREA_SSID), NORMAL_FONT);
                send_str("SSID  : %s", name_SSID);
                break;
            case 2:
                print_start(3, 1, GET_COLOR_AREA(AREA_PASSWORD), NORMAL_FONT);
                send_str("Password : %s", 
                                    xEventGroup&BIT_SECURITY || area_SCREEN != AREA_PASSWORD
                                        ? "*** WiFi password ***"
                                        : pwd_WIFI);
                break;
            case 3:
                print_start(4, 1, GET_COLOR_AREA(AREA_CITY), NORMAL_FONT);
                send_str("Name city : %s", name_CITY);
                break;
            case 4:
                print_start(5, 1, GET_COLOR_AREA(AREA_API), NORMAL_FONT);
                send_str("Key ");
                if(area_SCREEN == AREA_API) {
                    send_str(api_KEY);
                } else {
                    send_str("*** Key for openweather.com ***");
                }
                break;
            default : break;
        }
        print_end();
    }
}

void show_notify_handler(main_data_t * main_data,
                            int32_t cur_day, 
                            void* event_data)
{
    for(uint8_t day_count=0; day_count<SIZE_WEEK; day_count++) {
        print_start(day_count, 
                        0, 
                        cur_day == day_count 
                            ? WHITE 
                            : BLUE,
                        3);
        send_str("%.3s %s", WEEK_DAY[day_count], IS_DAY_ACTIVE(day_count) ? MES_ON : MES_OFF);
        print_end();
    }
    vTaskDelay(DELAY_SHOW_ITEM);
    send_in_frame(0, 14, FONT_INFO, 
                        IS_DAY_ACTIVE(cur_day) 
                        ? COLOR_ENABLE 
                        : COLOR_DISABLE, 
                        WEEK_DAY[cur_day]);

    bool area_min = false;
    for(int notif_count=0, area=0; notif_count<NOTIF_PER_DAY; area++) {  
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(2+notif_count, 
                6 + area%2*5, 
                GET_COLOR_AREA(area), 
                3);      
        send_str(" %2.2d  %s", 
                        area_min
                            ? VALUE_NOTIF_MIN(notif_count, cur_day)
                            : VALUE_NOTIF_HOUR(notif_count, cur_day),
                        area_min 
                            ? IS_NOTIF_ACTIVE(notif_count, cur_day) 
                                ? MES_ON
                                : MES_OFF
                            : ":"
                    );
        if(area_min)notif_count++;
        area_min = !area_min;
        print_end();
    }
}

void show_color_screen_handler(main_data_t * main_data,
                                    int32_t id, 
                                    void* event_data) 
{
    for(int i=0; i<COLOR_INTERFACE_NUMBER; i++) {
        print_start(0, i*5+1, GET_COLOR(colors_INTERFACE[i]), 3);
        send_str(" %s%s%s ", 
                    area_SCREEN == i
                    ? "["
                    : " ",
                    ITEM_COLOR_NAME[i],
                    area_SCREEN == i
                    ? "]"
                    : "");
        print_end();
    }
    uint16_t row=2, column = 1;
    vTaskDelay(DELAY_SHOW_ITEM/2);
    for(int color_count=0; color_count<SIZE_USED_COLORS; color_count++) {
        print_start(row++, column, GET_COLOR(color_count), 3);
        send_str( "%s %s",
                    COLOR_NAME[color_count],
                    GET_COLOR(color_count) == GET_COLOR(colors_INTERFACE[area_SCREEN])
                    ? "<--"
                    : "" );
                    if(color_count == SIZE_USED_COLORS/2){
                        column = 12;
                        row = 2;
                    }
        print_end();
    }
    print_text_box(290, 220, 120, 35, BLACK, YELLOW, 2, "Set color");
}

void show_state_handler(main_data_t *main_data,
                            int32_t id, 
                            void* event_data) 
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    send_in_frame(5, 45, 1, xEventGroup&BIT_ESPNOW_ALLOW
                                ? COLOR_ENABLE
                                : COLOR_DISABLE, 
                            xEventGroup&BIT_ESPNOW_ALLOW
                                ? "ESPNOW ON"
                                : "ESPNOW OFF");
    vTaskDelay(DELAY_SHOW_ITEM);
    send_in_frame(9, 45,  1, xEventGroup&BIT_SECURITY
                                ? COLOR_ENABLE 
                                : COLOR_DISABLE,
                            xEventGroup&BIT_SECURITY
                                ? "Security ON" 
                                : "Security OFF");
    vTaskDelay(DELAY_SHOW_ITEM);
    send_in_frame(13, 45, 1, xEventGroup&BIT_SOUNDS_ALLOW
                                ? COLOR_ENABLE 
                                : COLOR_DISABLE,  
                            xEventGroup&BIT_SOUNDS_ALLOW
                                ? "Sound ON"
                                : "Sound OFF");
    vTaskDelay(DELAY_SHOW_ITEM);
    send_in_frame(17, 45, 1, xEventGroup&BIT_SNTP_ALLOW
                                ? COLOR_ENABLE 
                                : COLOR_DISABLE,  
                            xEventGroup&BIT_SNTP_ALLOW
                                ? "SNTP ON"
                                : "STNP OFF");
    vTaskDelay(DELAY_SHOW_ITEM);
    print_start(2, 7, COLOR_DISABLE, 1);
    send_str("State device %s\n\n\r WiFi: %s.\n\n\r ESPNOW: %s.\n\n\r openweather.com: %s.\n\n\r SNTP: %s. %s%.15s%s%.15s",
                MY_DEVICE_NAME,
                xEventGroup&BIT_CON_STA_OK
                    ? "connect"
                    : xEventGroup&BIT_SSID_FOUND
                        ? "disconnect\t-\twrong\tpassword"
                        : "disconnect\t-\tSSID\tnot\tfound",
                xEventGroup&BIT_ESPNOW_ALLOW
                    ? xEventGroup&BIT_ESPNOW_CONECT
                        ? xEventGroup&BIT_SEN_1&&sensor_DATA
                            ? xEventGroup&BIT_SEN_2&&sensor_DATA
                                ? "connect\twith\tsensores"
                                : "connect\twith\tsensor"
                            : "device"
                        : "no\tdevices"
                    : "denied",
                xEventGroup&BIT_WEATHER_OK
                    ? "work"
                    : xEventGroup&BIT_RESPONSE_400_SERVER
                        ? "not\twork - wrong key api"
                        : "not\twork - no internet",
                xEventGroup&BIT_SNTP_ALLOW
                    ? xEventGroup&BIT_SNTP_WORK
                        ? "work"
                        : "not work - no internet"
                    : "denied",
                xEventGroup&BIT_SEN_1&&sensor_DATA
                    ? "\n\n\r Sensore 1: "
                    : "",
                xEventGroup&BIT_SEN_1&&sensor_DATA
                    ? sensor_DATA[0].name
                    : "",
                xEventGroup&BIT_SEN_2&&sensor_DATA
                    ? "\n\n\r Sensore 2: "
                    : "",
                xEventGroup&BIT_SEN_1&&sensor_DATA
                    ? sensor_DATA[1].name
                    : "");
    print_end();
    print_text_box(290, 220, 43, 35, BLACK, YELLOW, 2, "Set");
    print_text_box(140, 220, 80, 35, BLACK, YELLOW, 2, "Update");
}



void show_clock_handler(main_data_t * main_data,
                                    int32_t offset_time, 
                                    void* event_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    struct tm *cur_time = (struct tm *)event_data;
    for(uint8_t i=0; i<8; i++) {
        vTaskDelay(DELAY_SHOW_ITEM);
        switch (i) {
            case 0:
                print_start(3, 1, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("year 20%2.2d", cur_time->tm_year);
                break;
            case 1:
                print_start(3, 10, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("month %2.2d", cur_time->tm_mon+1);
                break;
            case 2:
                print_start(3, 18, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("day %2.2d", cur_time->tm_mday);
                break;
            case 3:
                print_start(5, 1, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("hour %2.2d", cur_time->tm_hour);
                break;
            case 4:
                print_start(5, 10, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("min %2.2d", cur_time->tm_min);
                break;
            case 5:
                print_start(5, 18, GET_COLOR_AREA(i), NORMAL_FONT);
                send_str("sec %2.2d", cur_time->tm_sec);
                break;
            case 6:
                print_start(0, 2, color_DESC, 1);
                send_str("Mode update time [%s], there is %sactual time.",
                    xEventGroup&BIT_SNTP_ALLOW
                        ? "SNTP"
                        : "manual",
                    !(xEventGroup&BIT_IS_TIME)
                        ? "no "
                        : "");
                break;
            case 7:
                print_start(2, 2, color_DESC, 1);
                send_str("Service SNTP %s work. Offset time :  [+] %+d [-]",
                        xEventGroup&BIT_SNTP_WORK
                        ? ""
                        : "not ",
                        (int)offset_time);
                break;
            default : break;
        }
        print_end();
    }
}




void show_main_handler(main_data_t * main_data,
                                int32_t id, 
                                void* time_pv) 
{
    if((uint8_t)id == DETAILS_SCREEN){
        print_start(0,9, GREEN, FONT_BUTTON);
        send_str_dwin(name_CITY);
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(4, 14, 
                        color_INFO,
                        FONT_SECOND_INFO);
        send_str( "%2d:00    %2d:00    %2d:00    %2d:00    %2d:00", 
                            dt_TX, 
                            (dt_TX+3)%24, 
                            (dt_TX+6)%24, 
                            (dt_TX+9)%24, 
                            (dt_TX+12)%24);
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(3, 0, 
                        PoP[1] > 60 
                            ? VIOLET 
                            : color_INFO, 
                        FONT_INFO);
        send_str("Rain      %2d%%   %2d%%   %2d%%   %2d%%   %2d%%", 
                            PoP[0],
                            PoP[1],
                            PoP[2],
                            PoP[3],
                            PoP[4]);
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(5, 0, 
                        get_color_temp(temp_FEELS_LIKE[0]),
                        FONT_INFO);
        send_str( "Temp t*C  %+3d   %+3d   %+3d   %+3d   %+3d",
                        temp_FEELS_LIKE[0],
                        temp_FEELS_LIKE[1],
                        temp_FEELS_LIKE[2],
                        temp_FEELS_LIKE[3],
                        temp_FEELS_LIKE[4]);
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM*2);
        print_lines(get_y_points(temp_FEELS_LIKE, NUMBER_ITEM_WEATHER, 80), NUMBER_ITEM_WEATHER, 70, 470, 260);
    } else {
    struct tm *cur_time = (struct tm *)time_pv;
    for(uint32_t i=0; i<5; i++) {
        vTaskDelay(DELAY_SHOW_ITEM);
        switch (i) {
            case 0:
            {
                print_start(3, 3, color_CLOCK, 6);     
                send_str("%2.2d : %2.2d", cur_time->tm_hour, cur_time->tm_min);
                break;
            }
            case 1:
            {
                print_start(8, 11, color_CLOCK, 2);
                send_str("%s %d\r\n     %s", 
                            WEEK_DAY[cur_time->tm_wday], 
                            cur_time->tm_mday,
                            (uint8_t)id == IS_NOTIFICATION
                                ? "[!]"
                                : "");
                break;
            }
            case 2:
            {
                if(weather_PIC == NO_WEATHER_PIC && !temp_INDOOR) return;
                print_start(1, 0, get_color_temp(temp_FEELS_LIKE[0]), FONT_INFO);
                if(weather_PIC != NO_WEATHER_PIC){
                    send_str(" Outdoor            t*C %d\n\r Feels like         t*C %d\n\r Chance of rain        %d%%", 
                                    temp_OUTDOOR, 
                                    temp_FEELS_LIKE[0],
                                    PoP[0]);
                }
                if(temp_INDOOR) {
                    print_end();
                    print_start(3, 0, get_color_temp(temp_INDOOR[0].tem), FONT_INFO);
                    send_str("\n\r Inside        t*C %f2.1", temp_INDOOR[0].tem);
                }
                break;
            }
            case 3:
            {
                print_start(3, 6, LEMON, FONT_BUTTON);
                send_str("%8s", description_WEATHER);
                break;
            }
            case 4:
            {
                print_start(23, 17, color_INFO, FONT_SECOND_INFO);
                send_str("Sunrise  %d:%2.2d  Sunset %d:%2.2d",
                            sunrise_HOUR,
                            sunrise_MIN,
                            sunset_HOUR,
                            sunset_MIN);
                break;
            }
            default   : break;
            }
            print_end();
        }
    }
}

void show_timer_handler(main_data_t *main_data, 
                                int32_t key, 
                                void* event_data) 
{
    if((uint8_t)key == TIMER_RUN){
        print_start(0, 5, color_CLOCK, FONT_INFO);
        send_str_dwin(asctime(get_time_tm()));
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM);
        print_start(2, 2, 
                    timer_MIN > 2
                    ? color_CLOCK 
                    : timer_MIN == 1 
                        ? LEMON
                        : ORANGE, 6);
        if(timer_HOUR){
            send_str("%.d : %.d : %2.2d",
                        timer_HOUR,
                        timer_MIN,
                        timer_SEC);
        } else if(timer_MIN) {
            send_str("  %d : %2.2d",
                        timer_MIN,
                        timer_SEC);
        } else {
            send_str("    %d", timer_SEC); 
        }        
        print_end();
    } else {
        for(int i=0; i<SIZE_TIMER; i++){
            vTaskDelay(DELAY_SHOW_ITEM/2);
            print_start(1, (i+1)*2, GET_COLOR_AREA(i), 6);
            send_str("%2.2d", timer_DATA[i]);   
            print_end();
        }
    }
}


void welcome()
{
    const int H = 10;
    const int W = 24;
    for (int h_count = 1; h_count < H; h_count++) {
        vTaskDelay(DELAY_SHOW_ITEM);
        for (int w_count = h_count % 2 == 0 ? 5 : 3, countView = 0; w_count < W; countView++) {
            if (countView % 1000 == 0) {
                w_count++;
                if (w_count % (h_count % 2 == 0 ? 2 : 3) == 0) {
                    print_start(h_count, w_count, LEMON, 2);
                    send_str("%d", (w_count * h_count * 3) % 100 );
                    print_end();
                }
            }
        }
    }
    print_start(2, 2, WHITE, 6);
    send_str("WAIT...");
    print_end();
}