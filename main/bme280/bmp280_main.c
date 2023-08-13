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
    DWIN_CHECK_AND_GO(i2c_param_config(I2C_NUM_0, &i2c_cfg), err);
    DWIN_CHECK_AND_GO(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0), err);
    bmx280 = bmx280_create(I2C_NUM_0);
    if (!bmx280) { 
        goto err;
    }
    DWIN_CHECK_AND_GO(bmx280_init(bmx280), err);
    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    DWIN_CHECK_AND_GO(bmx280_configure(bmx280, &bmx_cfg), err);
    bmx280_setMode(bmx280, BMX280_MODE_CYCLE);

    return ESP_OK;
err:
    i2c_driver_delete(I2C_NUM_0);
    return ESP_FAIL;
}

esp_err_t read_sensor_handler(dwin_data_t* main_data)
{
    do {
        vTaskDelay(pdMS_TO_TICKS(10));
    } while(bmx280_isSampling(bmx280));
    int tmp = NO_TEMP;
    if(bmx280_readout(bmx280, &tmp, 0, 0) == ESP_OK){
        temp_BM280 = (int)tmp/100;
    } else {
        temp_BM280 = NO_TEMP;
    }
    return ESP_OK;
}
