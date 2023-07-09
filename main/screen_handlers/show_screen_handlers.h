#pragma once

#include "dwin_common.h"

void show_info_handler(const dwin_data_t *, int32_t, void*);
void show_ap_handler(const dwin_data_t *,  int32_t,  void*);
void show_ssid_handler(const dwin_data_t *,  int32_t, void*);
void show_settings_handler(const dwin_data_t *, int32_t, void*);
void show_notify_handler(const dwin_data_t *, int32_t, void*);
void show_color_screen_handler(const dwin_data_t *, int32_t, void*);
void show_clock_handler(const dwin_data_t *, int32_t, void*);
void show_main_handler(const dwin_data_t *, int32_t, void*);
void show_timer_handler(const dwin_data_t *, int32_t, void*);
void show_state_handler(const dwin_data_t *,int32_t, void*);
void show_device_handler(const dwin_data_t *, int32_t, void*);
void welcome();

