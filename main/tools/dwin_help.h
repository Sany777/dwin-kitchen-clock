#pragma once 
#include "dwin_common.h"

void fix_scale_height(uint16_t *h_points, 
                const int number,
                const uint16_t height);
char *get_pos_data_str_from_uri(const char *uri_str, 
                                const char *base_path);

char *get_chip(int model_id);