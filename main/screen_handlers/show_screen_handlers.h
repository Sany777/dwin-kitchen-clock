#pragma once

#include "dwin_common.h"

void show_info_handler(main_data_t * main_data, 
                        int32_t state, 
                        void* event_data);

void show_ap_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_ssid_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_settings_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_notify_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_color_screen_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_clock_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_main_handler(main_data_t* main_data, 
                                    int32_t, 
                                    void*);
void show_timer_handler(main_data_t *main_data, 
                                int32_t key, 
                                void*);
void welcome();

