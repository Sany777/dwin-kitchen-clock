#pragma once 

#include "dwin_common.h"

esp_err_t init_bmp280(void);
void read_sensor_handler(main_data_t* main_data);
