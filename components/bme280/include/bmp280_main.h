#pragma once 

#include "dwin_common.h"

esp_err_t bme280_init(void);
esp_err_t bme280_read_sensor(dwin_data_t* main_data);
