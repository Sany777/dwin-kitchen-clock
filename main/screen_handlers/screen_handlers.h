#pragma once


#include "dwin_config.h"
#include "dwin_data_types.h"
#include "screen_keys.h"
#include "dwin_defines.h"

#include "dwin_uart.h"
#include "dwin_drivers.h"
#include "screen_keys.h"
#include "dwin_events.h"
#include "screen_handlers.h"
#include "wifi_dwin.h"

/*screen handlers*/

void ap_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void setting_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void main_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void clock_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void state_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void notifications_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void timer_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void info_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void device_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void search_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);
void set_color_screen_handler(dwin_data_t* main_data, uint8_t command, char symbol);