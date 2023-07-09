#pragma once 
#include "dwin_common.h"

void show_screen(int32_t key, const void *data_send, const size_t size_data);
const uint16_t *get_y_points( const int8_t *points, 
                            const int number,
                            const uint16_t height);
const char *get_data_from_uri( const char *uri_str, 
                                const char *base_path);
bool notification_alarm(const dwin_data_t *main_data, 
                            const struct tm* cur_time, 
                            const bool alarm);
const char *get_chip(int model_id);
void set_timezone(int hour);
void set_time_tv(struct timeval *tv);
void set_time_tm(struct tm *timeptr);
struct tm* get_time_tm(void);
void set_dwin_clock();