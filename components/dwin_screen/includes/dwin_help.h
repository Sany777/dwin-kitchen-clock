#pragma once 
#include "dwin_common.h"
#include <string.h>

void show_screen(int32_t key, const void *data_send, const size_t size_data);
const uint16_t *dwin_help_get_y_points( const float *points, 
                            const uint16_t number,
                            const uint16_t height);
const char *get_data_from_uri( const char *uri_str, 
                                const char *base_path);
bool notification_alarm(const dwin_data_t *main_data, 
                            const struct tm* cur_time, 
                            const bool alarm);
const char *get_chip(int model_id);
void wifi_dwin_set_timezone(int hour);
void set_time_tv(struct timeval *tv);
void wifi_dwin_set_time_tm(struct tm *timeptr);
struct tm* wifi_dwin_get_time_tm(void);
void set_dwin_clock();
void dwin_help_init_currency_val(dwin_data_t *main_data);
