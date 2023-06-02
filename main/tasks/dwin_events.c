#include "dwin_events.h"


esp_event_loop_handle_t 
                direct_loop, 
                show_loop, 
                fast_service_loop,
                slow_service_loop;
                
ESP_EVENT_DEFINE_BASE(EVENTS_SHOW);
ESP_EVENT_DEFINE_BASE(EVENTS_SERVICE);
ESP_EVENT_DEFINE_BASE(EVENTS_DIRECTION);
ESP_EVENT_DEFINE_BASE(EVENTS_MANAGER);
ESP_EVENT_DEFINE_BASE(WIFI_SET);
ESP_EVENT_DEFINE_BASE(ESPNOW_SET);


void init_dwin_events(main_data_t *main_data) 
{
    const esp_event_loop_args_t my_loop_args = {
        .queue_size = TASK_EVENT_SIZE,
        .task_name = NULL
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &direct_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &show_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &fast_service_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &slow_service_loop));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                direct_loop,
                                EVENTS_MANAGER,
                                ESP_EVENT_ANY_ID,
                                screen_change_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET,
                                INIT_SNTP,
                                init_sntp_handler,
                                (void *)main_data,
                                NULL
                            ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET,
                                GET_WEATHER,
                                get_weather_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                fast_service_loop,
                                WIFI_SET,
                                ESP_EVENT_ANY_ID,
                                set_mode_wifi_handler,
                                (void *)main_data,
                                NULL
                            ));
    for(int i=0; i<SIZE_LIST_TASKS; i++) {
            xTaskCreate(
                list_services[i].pTask, 
                "service",
                list_services[i].stack, 
                (void * const)main_data, 
                list_services[i].priority,
                NULL
            );
    }



    get_weather();
    ESP_LOGI(TAG, "get_weather"); 
    vTaskDelay(pdMS_TO_TICKS(5000));
    start_ap();
    ESP_LOGI(TAG, "start_ap"); 
    vTaskDelay(pdMS_TO_TICKS(25000));
    get_weather();
    vTaskDelay(pdMS_TO_TICKS(7000));
    start_ap();
    vTaskDelay(pdMS_TO_TICKS(25000));
    ESP_LOGI(TAG, "start_sntp"); 
    start_sntp();

}


                                                                          \


void screen_change_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)
{
    if(new_screen >= SIZE_LIST_TASKS || new_screen < 0) return;
    static esp_event_handler_instance_t 
                        direction_handler, 
                        show_handler, 
                        service_handler;
    if(direction_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        direct_loop, 
                        EVENTS_DIRECTION, 
                        ESP_EVENT_ANY_ID,  
                        direction_handler
                    ));
        direction_handler = NULL;
    }
    if(show_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        show_loop, 
                        EVENTS_SHOW, 
                        ESP_EVENT_ANY_ID,  
                        show_handler
                    ));
        show_handler = NULL;
    }
    if(service_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        slow_service_loop, 
                        EVENTS_SERVICE, 
                        ESP_EVENT_ANY_ID,  
                        service_handler)
                    );
        service_handler = NULL;
    }
    vTaskDelay(TIMEOUT_SEND_EVENTS);
    if(links_handlers_list[new_screen].main_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                    direct_loop, 
                                    EVENTS_DIRECTION, 
                                    ESP_EVENT_ANY_ID, 
                                    links_handlers_list[new_screen].main_handler, 
                                    main_data,
                                    &direction_handler
                                ));
    }
    if(links_handlers_list[new_screen].show_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                    show_loop, 
                                    EVENTS_SHOW, 
                                    ESP_EVENT_ANY_ID, 
                                    links_handlers_list[new_screen].show_handler, 
                                    main_data,
                                    &show_handler
                                ));
    }
    if(links_handlers_list[new_screen].service_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop, 
                                EVENTS_SERVICE, 
                                ESP_EVENT_ANY_ID, 
                                links_handlers_list[new_screen].service_handler, 
                                main_data,
                                &service_handler
                            ));
    }
    esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_INIT, NULL, 0, TIMEOUT_SEND_EVENTS);
}



void direction_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        // esp_event_loop_run(direct_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_FAST_LOOP);
    }
}


 void show_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        if(show_loop){
            // esp_event_loop_run(show_loop, TICKS_TO_RUN_LOOP);

        }
        vTaskDelay(DELAY_FAST_LOOP);
    }
}

 void fast_services_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(fast_service_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_FAST_LOOP);
    }
}


void slow_services_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(slow_service_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_SLOW_LOOP);
    }
}

void vApplicationIdleHook( void )
{ 
    TickType_t us_time_sleep = TIMER_WAKEUP_LONG_TIME_US;
    while (1) {
        sleep_dwin(us_time_sleep);
    }
}
