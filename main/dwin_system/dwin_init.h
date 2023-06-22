#pragma once

#include "driver/gpio.h"
#include "dwin_common.h"

#define SIZE_SERVICE_TASK 4

void esp_init(void);
void wifi_init(void);
void set_power_mode_eco(const bool run);
void i2c_master_init();
int8_t BME280_I2C_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);
int8_t BME280_I2C_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);
void BME280_delay_msek(uint32_t msek);
void init_bme280(void);
void read_sensor_handler(main_data_t* main_data);

