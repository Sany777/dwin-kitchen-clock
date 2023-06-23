#pragma once

#include "driver/gpio.h"
#include "dwin_common.h"

#define SIZE_SERVICE_TASK 4

void esp_init(void);
void wifi_init(void);
void set_power_mode_eco(const bool run);
esp_err_t init_bmp280(void);
void read_sensor_handler(main_data_t* main_data);

