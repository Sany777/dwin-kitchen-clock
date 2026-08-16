#pragma once

#include "dwin_common.h"

void show_info_handler(const dwin_data_t *, int32_t, void*);
void show_screen_ap(const dwin_data_t *,  int32_t,  void*);
void show_screen_ssid(const dwin_data_t *,  int32_t, void*);
void show_screen_settings(const dwin_data_t *, int32_t, void*);
void show_screen_notify(const dwin_data_t *, int32_t, void*);
void show_color_screen_handler(const dwin_data_t *, int32_t, void*);
void show_clock_handler(const dwin_data_t *, int32_t, void*);
void show_screen_main(const dwin_data_t *, int32_t, void*);
void show_screen_timer(const dwin_data_t *, int32_t, void*);
void show_screen_state(const dwin_data_t *,int32_t, void*);
void show_screen_device(const dwin_data_t *, int32_t, void*);
void welcome();

