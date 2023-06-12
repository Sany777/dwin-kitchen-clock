#pragma once 
#include "dwin_common.h"


uint16_t *get_y_points(  int8_t *points, 
                            const int number,
                            const uint16_t height );
char *get_data_from_uri( const char *uri_str, 
                         const char *base_path );
char *get_chip(int model_id);
void set_timezone(int hour);
void set_time_tv(struct timeval *tv);
void set_time_tm(struct tm *timeptr, const bool update_dwin);
struct tm* get_time_tm();