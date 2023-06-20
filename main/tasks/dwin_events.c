#include "dwin_events.h"

uint8_t cur_screen_id = MAIN_SCREEN;

void check_net_data(main_data_t* main_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_CON_STA_OK){
        write_memory(main_data, DATA_PWD);
        write_memory(main_data, DATA_SSID);
    } else {
        read_memory(main_data, DATA_PWD);
        read_memory(main_data, DATA_SSID);
    }
}

void direction_task(void *pv)
{
    uint8_t data_in[2] = {0};
    main_data_t *main_data = (main_data_t*)pv;
    while(1) {
        if(xQueueReceive(queue_direct, 
                            data_in,
                            portMAX_DELAY) == pdTRUE)
        {
            if(cur_screen_id != MAIN_SCREEN 
                    && cur_screen_id != TIMER_SCREEN 
                    && cur_screen_id != SERVER_SCREEN)
            {
                set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
            }
            if(KEY_IS_SET_TASK(data_in[0])){
                area_SCREEN = 0;
                screen_handler(main_data, KEY_CLOSE, 0);
                cur_screen_id = data_in[0];
                screen_handler(main_data, KEY_INIT, 0);
            } else  if(data_in[0] > START_SERVICE_EVENTS && data_in[0] < END_SERVICE_EVENTS){
                xQueueSend(queue_service, &data_in[0], 200);
            } else {
                screen_handler(main_data, data_in[0], data_in[1]);
            }

        }
    }
}


void show_task(void *main_data)
{
    show_queue_data_t data_in = {0};
    while(1) {
        if(xQueueReceive(queue_show, &data_in, portMAX_DELAY) == pdTRUE){
            show_handler(main_data, data_in.command, data_in.data);   
            if(data_in.data){
                free(data_in.data);
                data_in.data = NULL;
            }
        }
    }
}


void service_task(void *main_data)
{
    uint8_t key;
    while(1) {
        if(xQueueReceive(queue_service, &key, portMAX_DELAY) == pdTRUE){
            if(key == GET_WEATHER){
                get_weather(main_data, key);
            } else if (key == INIT_SNTP || key == STOP_SNTP){
                set_sntp(main_data, key);
            } else if(key == CHECK_NET_DATA){
                check_net_data(main_data);
            } else {
                set_wifi(main_data, key);
            }
        }
    }
}

void vApplicationIdleHook(void)
{ 
    TickType_t us_time_sleep = TIMER_WAKEUP_LONG_TIME_US;
    while (1) {
        uart_wait_tx_idle_polling(UART_DWIN);  
        ESP_ERROR_CHECK(gpio_sleep_set_direction(
                            RXD_PIN, 
                            GPIO_MODE_INPUT));
        ESP_ERROR_CHECK(gpio_sleep_set_pull_mode(
                            RXD_PIN, 
                            GPIO_PULLUP_ONLY));
        ESP_ERROR_CHECK(uart_set_wakeup_threshold(
                            UART_DWIN, 
                            UART_WAKEUP_THRESHOLD));
        ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(
                        UART_DWIN));                
        ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(us_time_sleep)); 
        ESP_ERROR_CHECK(esp_light_sleep_start());  

    }
}
