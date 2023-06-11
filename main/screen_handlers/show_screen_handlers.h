#pragma once

#include "dwin_common.h"


static const char *MES_ON = "[ON]";
static const char *MES_OFF = "[OFF]";

static const char *WEEK_DAY[SIZE_WEEK] = {
    "Sunday",
    "Monday",
    "Tuesday", 
    "Wednesday", 
    "Thursday", 
    "Friday", 
    "Saturday"
};

static const char* ITEM_CUSTOM_NAME[] = {
    "Clock", 
    "Desc", 
    "Info", 
};

static const char* COLOR_NAME[] = { 
    "WHITE", 
    "CAEN",
    "BLUE", 
    "GREY"
    "VIOLET", 
    "LEMON", 
    "RED", 
};



void show_ap_handler(void* main_data, 
                            esp_event_base_t base,
                            int32_t state, 
                            void* event_data);
void show_ssid_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t ssids_number, 
                            void* event_data) ;
void show_net_settings_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t id, 
                            void* event_data);
void show_notify_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t cur_day, 
                            void* event_data);
void show_color_screen_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t id, 
                            void* event_data);
void show_settings_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t id, 
                            void* event_data);
void show_clock_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t offset_time, 
                            void* event_data);
void show_main_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t data_sensor, 
                            void* event_data);
void show_timer_run_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t run, 
                            void* event_data);
void show_timer_stop_handler(void* main_data, 
                            esp_event_base_t base, 
                            int32_t run, 
                            void* event_data);
void show_details_weather(main_data_t * main_data);
void welcome();

