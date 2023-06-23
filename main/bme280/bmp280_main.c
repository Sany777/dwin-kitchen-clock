#include "bmp280_main.h"

bmx280_t* bmx280;

esp_err_t init_bmp280(void)
{
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = false,
        .scl_pullup_en = false,
        .master = {
            .clk_speed = 100000
        }
    };
    DWIN_CHECK_AND_RETURN(i2c_param_config(I2C_NUM_0, &i2c_cfg));
    DWIN_CHECK_AND_RETURN(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    bmx280 = bmx280_create(I2C_NUM_0);
    if (!bmx280) { 
        return ESP_FAIL;
    }
    DWIN_CHECK_AND_RETURN(bmx280_init(bmx280));
    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    DWIN_CHECK_AND_RETURN(bmx280_configure(bmx280, &bmx_cfg));
    return ESP_OK;
}

void read_sensor_handler(main_data_t* main_data)
{
    bmx280_setMode(bmx280, BMX280_MODE_FORCE);
    do {
        vTaskDelay(pdMS_TO_TICKS(10));
    } while(bmx280_isSampling(bmx280));
    float tmp = NO_TEMP;
    bmx280_readoutFloat(bmx280, &tmp, &pressure_BM280, 0);
    temp_BM280 = (int)tmp;
}
