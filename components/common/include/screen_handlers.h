#pragma once

#include "dwin_config.h"
#include "dwin_data_types.h"
#include "dwin_defines.h"
#include "screen_keys.h"




void screen_handler_ap(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_settings(dwin_data_t *main_data, uint8_t command,
                            char symbol);
void screen_handler_main(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_clock(dwin_data_t *main_data, uint8_t command, char symbol);
void handle_state_screen(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_notifications(dwin_data_t *main_data, uint8_t command,
                                 char symbol);
void screen_handler_timer(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_info(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_device(dwin_data_t *main_data, uint8_t command, char symbol);
void screen_handler_wifi_scan(dwin_data_t *main_data, uint8_t command,
                             char symbol);
void handle_color_settings_screen(dwin_data_t *main_data, uint8_t command,
                                  char symbol);
