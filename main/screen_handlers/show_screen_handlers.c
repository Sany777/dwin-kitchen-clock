#include "show_screen_handlers.h"

static const int NORMAL_FONT = 2, FONT_INFO = 2, FONT_SECOND_INFO = 1, FONT_BUTTON = 3;

static const char *MES_ON = "[ON]";
static const char *MES_OFF = "[OFF]";

#define SIZE_USED_COLORS 10
const uint16_t USED_COLORS[SIZE_USED_COLORS] = {
    WHITE, 
    CAEN, 
    BLUE, 
    GREY, 
    VIOLET, 
    LEMON, 
    RED, 
    GREEN, 
    AQUA, 
    ORANGE
};

static const char* COLOR_NAME[] = { 
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

static const char *WEEK_DAY[SIZE_WEEK] = {
    "Monday",
    "Tuesday", 
    "Wednesday", 
    "Thursday", 
    "Friday", 
    "Saturday",
    "Sunday",
};

static const char* ITEM_COLOR_NAME[] = {
    "Clock", 
    "Desc", 
    "Info", 
};



void show_info_handler(const dwin_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{
    print_start(1, 17, WHITE, FONT_SECOND_INFO);
    uint8_t mac[8];
    send_str("Device %s",CONFIG_MY_DEVICE_NAME);
    if(esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK){
        send_str("\n\r MAC WIFI STA: "MACSTR"", MAC2STR(mac));
    }
    if(esp_read_mac(mac, ESP_MAC_BT) == ESP_OK){
        send_str("\n\r MAC BT : "MACSTR"", MAC2STR(mac));
    }
    send_str("\n\r  Configuration server (WiFi AP)\n\r SSID: %s.\r\n Password: %s.\n\r IP: %s", 
                        CONFIG_AP_WIFI_SSID, 
                        CONFIG_AP_WIFI_PWD, CONFIG_MY_IP);
    print_end();
    print_start(14, 2, WHITE, FONT_SECOND_INFO);
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    send_str("This is %s chip with %d CPU core(s), WiFi%s%s.",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    send_str("\n\r Silicon revision v%d.%d.", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        send_str("\n\r %" PRIu32 "MB %s flash", flash_size / (uint32_t)(1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
        send_str("\n\r Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
    }
    print_end();
}

void show_device_handler(const dwin_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{
    print_start(1, 5, WHITE, FONT_SECOND_INFO);
    device_inf_t *device = get_my_device();
    if(device){
        send_str_dwin("Device ESPNOW");
        int count = 0;
        while(device){
            send_str("\n\n\r %2.0d %s %s \"%s\" MAC: "MACSTR".", 
                        count,
                        count == 0
                            ? "My device"
                            : "Connected device",
                        device->type == SENSOR_TEMP_DEVICE
                            ? "sensor"
                            : device->type == TIMER_DEVICE
                                ? "timer"
                                : "screen",
                        device->name,
                        MAC2STR(device->mac));
            device = SLIST_NEXT(device, next);
            count++;
        }
    } else {
        send_str_dwin("ESPNOW not work");
    }
    print_end();
}

void show_ap_handler(const dwin_data_t * main_data, 
                        int32_t state, 
                        void* event_data)
{
    dwin_set_pic(INFO_PIC);
    vTaskDelay(DELAY_SHOW_ITEM);
    print_start(2, 2, WHITE, FONT_INFO);
    if(event_data == NULL){
        send_str(
                "Connect to the AP \n\r with the name \"%s\"\r\n and paswword: \"%s\".\r\n and go to the address %s",
                    CONFIG_AP_WIFI_SSID,
                    CONFIG_AP_WIFI_PWD,
                    CONFIG_MY_IP
                );
    } else {
        uint8_t *mac = (uint8_t *)event_data;
        if(state == STATION_JOINE){
           send_str( 
                "Station "MACSTR" join.\r\n Go to \"%s\"", MAC2STR(mac), CONFIG_MY_IP);
        } else {
            send_str("Station "MACSTR" leave.", MAC2STR(mac));
        }
    }
    print_end();
}

void show_ssid_handler(const dwin_data_t * main_data,
                                int32_t ap_count, 
                                void* event_data) 
{
    wifi_ap_record_t *ap_info = *(wifi_ap_record_t**)event_data;
    bool first_page = true;
    if(ap_count == 0){
        print_start(1, 0, color_DESC, NORMAL_FONT);
        send_str("No WiFi network found");
        print_end();
    } else {
        int ap_num = 0;
        print_start(1, 0, color_DESC, NORMAL_FONT);
        send_str(" N|       SSID     |Signal|Ch.| Auth.");
        if(ap_count < 0){
            ap_count *= -1;
            first_page = false;
            ap_info += MAX_SSID_PEER_SCREEN;
            ap_num = MAX_SSID_PEER_SCREEN;
        } 
        print_end();
        vTaskDelay(DELAY_SHOW_ITEM/2);
        for(int i=0; ap_num<ap_count && i<MAX_SSID_PEER_SCREEN; i++, ap_num++, ap_info++) {
            print_start(i+1, 0, GET_COLOR_AREA(i), NORMAL_FONT);
            send_str("\n\r%2d|%15.15s |%6.6s| %2.2d| %s",
                            ap_num+1, 
                            ap_info->ssid, 
                            get_rssi_description(ap_info->rssi), 
                            ap_info->primary, 
                            get_auth_mode(ap_info->authmode));
            print_end();
        }
        vTaskDelay(DELAY_SHOW_ITEM/2);
        if(ap_count > MAX_SSID_PEER_SCREEN){
            print_text_box(420, 220, 55, 35, BLACK, YELLOW, 2, 
                                first_page
                                    ? "Next"
                                    : "Prev");
        } 
    }
}


void show_settings_handler(const dwin_data_t * main_data,
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
                    send_str("WiFi Ok, %s",
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
                send_str_dwin("  [SEARCH SSID]");
                break;
            case 1:
                print_start(2, 1, GET_COLOR_AREA(AREA_SSID), NORMAL_FONT);
                send_str("SSID      : %s", name_SSID);
                break;
            case 2:
                uint16_t color = xEventGroup&BIT_WRONG_API_KEY
                                    ? RED
                                    : GET_COLOR_AREA(AREA_PASSWORD);
                print_start(3, 1, color, NORMAL_FONT);
                send_str_dwin("Password  : ");
                if(area_SCREEN != AREA_PASSWORD){
                    send_str_dwin("*** WiFi password ***");
                } else if (xEventGroup&BIT_SECURITY) {
                    print_hide_pwd(pwd_WIFI);
                } else {
                    send_str_dwin(pwd_WIFI);  
                }                                                             
                break;
            case 3:
                print_start(4, 1, GET_COLOR_AREA(AREA_CITY), NORMAL_FONT);
                send_str("Name city : %s", name_CITY);
                break;
            case 4:
                print_start(5, 1, GET_COLOR_AREA(AREA_API), NORMAL_FONT);
                send_str("Key : ");
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

void show_notify_handler(const dwin_data_t * main_data,
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
        send_str(" %2.2d", 
                    area_min
                        ? VALUE_NOTIF_MIN(notif_count, cur_day)
                        : VALUE_NOTIF_HOUR(notif_count, cur_day));
        if(area_min){
            print_end();
            print_start(2+notif_count, 
                15, 
                IS_NOTIF_ACTIVE(notif_count, cur_day)
                    ? YELLOW
                    : CAEN, 
                3);      
            send_str_dwin(IS_NOTIF_ACTIVE(notif_count, cur_day) 
                                ? MES_ON
                                : MES_OFF);
            notif_count++;
        } else {
            send_str_dwin("  :");
        }
        area_min = !area_min;
        print_end();
    }
}

void show_color_screen_handler(const dwin_data_t * main_data,
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

void show_state_handler(const dwin_data_t *main_data,
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
    print_start(1, 7, COLOR_DISABLE, 1);
    send_str("State device %s\n\n\r WiFi: %s.\n\r ESPNOW: %s.\n\r openweather.com: %s.\n\r SNTP: %s.",
                CONFIG_MY_DEVICE_NAME,
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
                    : xEventGroup&BIT_RESPONSE_400_SERVER && xEventGroup&BIT_CON_STA_OK
                        ? "not\twork - wrong key api"
                        : "not\twork - no internet",
                xEventGroup&BIT_SNTP_ALLOW
                    ? xEventGroup&BIT_SNTP_WORK
                        ? "work"
                        : "not work - no internet"
                    : "denied");
    if(sensor_DATA){
        if(xEventGroup&BIT_SEN_1){
            send_str("\n\n\rСonnected to the temperature sensor 1:\n\r  %15.15s, mac: "MACSTR", t*C: ", sensor_DATA[0].name, MAC2STR(sensor_DATA[0].mac), sensor_DATA[0].tem);
        }
        if(xEventGroup&BIT_SEN_2){
            send_str("\n\n\rСonnected to the temperature sensor 2:\n\r  %15.15s, mac: "MACSTR", t*C", sensor_DATA[1].name, MAC2STR(sensor_DATA[1].mac), sensor_DATA[1].tem);
        }
    }
    print_end();
    print_text_box(250, 220, 43, 35, BLACK, YELLOW, 2, "Set");
    print_text_box(110, 220, 80, 35, BLACK, YELLOW, 2, "Update");
}



void show_clock_handler(const dwin_data_t * main_data,
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
                send_str("month %2.2d", cur_time->tm_mon);
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
                send_str("Service SNTP %s work. Offset time: [+] %+d [-]",
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




void show_main_handler(const dwin_data_t * main_data,
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
        set_color(get_color_temp(temp_FEELS_LIKE[0]), WHITE);
        print_lines(get_y_points(temp_FEELS_LIKE, NUMBER_ITEM_WEATHER, 80), NUMBER_ITEM_WEATHER, 70, 470, 260);
    } else {
    struct tm *cur_time = (struct tm *)time_pv;
    for(uint32_t i=0; i<5; i++) {
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
                if(temp_BM280 != NO_TEMP) {
                    print_start(0, 0, get_color_temp(temp_BM280), FONT_INFO);
                    send_str("   Indoor           t*C %d", temp_BM280);
                    print_end();
                }
                if(weather_PIC != NO_WEATHER_PIC){
                    print_start(1, 0, get_color_temp(temp_OUTDOOR), FONT_INFO);
                    send_str("   Outdoor          t*C %d\n\r   Feels like       t*C %d\n\r   Chance of rain       %d%%", 
                                    temp_OUTDOOR, 
                                    temp_FEELS_LIKE[0],
                                    PoP[0]);
                }
                break;
            }
            case 3:
            {
                if(weather_PIC == NO_WEATHER_PIC) return;
                print_start(4, 5, LEMON, FONT_BUTTON);
                send_str("%12.18s", description_WEATHER);
                break;
            }
            case 4:
            {
                if(weather_PIC == NO_WEATHER_PIC)return;
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

void show_timer_handler(const dwin_data_t *main_data, 
                                int32_t key, 
                                void* event_data) 
{
    if((uint8_t)key == TIMER_RUN){
        print_start(0, 5, color_CLOCK, FONT_INFO);
        send_str_dwin(asctime(get_time_tm()));
        print_end();
        print_start(2, 2, 
                    timer_MIN > 2
                    ? color_CLOCK 
                    : timer_MIN == 1 
                        ? LEMON
                        : ORANGE, 6);
        if(timer_HOUR){
            send_str("%d : %2.2d : %2.2d",
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
    print_start(2, 2, RED, 6);
    send_str_dwin("WAIT...");
    print_end();
}