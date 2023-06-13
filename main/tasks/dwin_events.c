#include "dwin_events.h"

esp_event_loop_handle_t slow_service_loop, fast_service_loop;
uint16_t cur_screen_id;
ESP_EVENT_DEFINE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DEFINE_BASE(SNTP_EVENTS);

handlers_dwin_t screens_handlers[SIZE_LIST_TASKS] = {
    {
        .main_handler    = main_screen_handler,
        .show_handler    = show_main_handler,
    },
    {
        .main_handler    = clock_handler,
        .show_handler    = show_clock_handler,
    },
    {
        .main_handler    = search_screen_handler,
        .show_handler    = show_ssid_handler,
    },
    {
        .main_handler    = setting_screen_handler,
        .show_handler    = show_settings_handler,
    },
    {
        .main_handler    = set_color_screen_handler,
        .show_handler    = show_color_screen_handler,
    },
    {
        .main_handler    = notifications_screen_handler,
        .show_handler    = show_notify_handler,
    },
    {
        .main_handler    = ap_screen_handler,
        .show_handler    = show_ap_handler,
    },

    {
        .main_handler    = state_screen_handler,
        .show_handler    = show_state_handler,
    },
    {
        .main_handler    = timer_screen_handler,
        .show_handler    = show_timer_stop_handler,
    },
};

void init_dwin_events(main_data_t *main_data) 
{
    const esp_event_loop_args_t my_loop_args = {
        .queue_size = TASK_EVENT_SIZE,
        .task_name = NULL
    };
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &slow_service_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &fast_service_loop));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                ESPNOW_EVENTS,
                                ESP_EVENT_ANY_ID,
                                set_espnow_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                SNTP_EVENTS,
                                ESP_EVENT_ANY_ID,
                                init_sntp_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET_EVENTS,
                                GET_WEATHER,
                                get_weather_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                fast_service_loop,
                                WIFI_SET_EVENTS,
                                ESP_EVENT_ANY_ID,
                                set_mode_wifi_handler,
                                (void *)main_data,
                                NULL
                            ));

    
    for(int i=0; i<SIZE_SERVICE_TASK; i++){
        xTaskCreate(
            sevice_tasks[i].pTask, 
            "service",
            sevice_tasks[i].stack, 
            (void * const)main_data, 
            sevice_tasks[i].priority,
            NULL
        );
    }
    EventBits_t xEventGroup;
    do{
        send_hello();
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_DWIN_RESPONSE_OK, false, false, 1000);
    }while(!(xEventGroup&BIT_DWIN_RESPONSE_OK));
    dwin_set_pic(NO_WEATHER_PIC);
    // set_color(GREEN, WHITE);

// clear_screen();
// uart_write_bytes(UART_DWIN, send, 20);
// send_str_dwin(send);
    // vTaskDelay(1500/portTICK_PERIOD_MS);
    // print_broken_line(points, 10, 10, 100);
    // print_histogram(points, 7, 10, 400, 260);
    // set_text_box(10, 10, 475, 267);
    // fill_area(20, 20, RED);

//  uart_write_bytes(UART_DWIN, points, 4);


    // xEventGroupSetBits(dwin_event_group, BIT_SSID_FOUND|BIT_IS_TIME|BIT_CON_STA_OK|BIT_SEN_2);
    // start_espnow();
    // esp_event_post_to(slow_service_loop, ESPNOW_SET, STOP_ESPNOW, NULL, 0, WAIT_SERVICE);
    // vTaskDelay(5000/portTICK_PERIOD_MS);
    // start_espnow();
    // esp_event_post_to(slow_service_loop, ESPNOW_SET, PAUSE_ESPNOW, NULL, 0, WAIT_SERVICE);
    // start_espnow();

    // esp_event_post_to(
    //         direct_loop,
    //         EVENTS_MANAGER,
    //         SERVER_SCREEN,
    //         NULL,
    //         0,
    //         TIMEOUT_PUSH_KEY
    //     );
    // vTaskDelay(50000/portTICK_PERIOD_MS);


}





void check_net_data_handler(main_data_t* main_data)
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

void test_clock_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)                                                     
{
    send_str_dwin(asctime(get_time_tm()));
}



void direction_task(void *pv)
{
    main_data_t *main_data = (main_data_t*)pv;
    uint8_t rx_buf[2];
    while(1) {
        if(xQueueReceive(queue_direct, rx_buf, portMAX_DELAY) == pdTRUE){
            if(rx_buf[0] && KEY_IS_SET_TASK(rx_buf[0])){
                    area_SCREEN = 0;
                    screen_handler(main_data, KEY_CLOSE, 0);
                    cur_screen_id = GET_SCREEN_TASK(rx_buf[0]);
                    screen_handler(main_data, KEY_INIT, 0);
            } else {
                screen_handler(main_data, rx_buf[0], rx_buf[1]);
            }
        }
    }
}



void show_task(void *main_data)
{
    show_queue_data_t data;
    while(1) {
        if(xQueueReceive(queue_show, &data, portMAX_DELAY) == pdTRUE){
            show_handler(main_data, data.command, data.data);
        }
    }
}


void slow_service_task(void *pv)
{
    while(1) {
        esp_event_loop_run(slow_service_loop, portMAX_DELAY);
    }
}

void fast_service_task(void *pv)
{
    while(1) {
        esp_event_loop_run(fast_service_loop, portMAX_DELAY);
    }
}

void vApplicationIdleHook( void )
{ 
    TickType_t us_time_sleep = TIMER_WAKEUP_LONG_TIME_US;
    while (1) {
        
        sleep_dwin(us_time_sleep);
    }
}
