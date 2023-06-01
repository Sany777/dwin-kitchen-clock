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
ESP_EVENT_DEFINE_BASE(EVENTS_SET_TIME);
ESP_EVENT_DEFINE_BASE(WIFI_SET);


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
                                    EVENTS_SET_TIME,
                                    ESP_EVENT_ANY_ID,
                                    set_time_handler,
                                    (void *)main_data,
                                    NULL
                                ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                    fast_service_loop,
                                    WIFI_SET,
                                    ESP_EVENT_ANY_ID,
                                    wifi_set_mode_handler,
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



        start_ap();
        vTaskDelay(pdMS_TO_TICKS(25000));
        get_weather();
        // esp_event_post_to(fast_service_loop, WIFI_SET, INIT_STA, NULL, 0, 2000);
        
        // esp_event_post_to(slow_service_loop, WIFI_SET, INIT_ESPNOW, NULL, 0, 1000);
        // xEventGroupWaitBits(dwin_event_group, BIT_INIT_ESPNOW, false, false, (pdMS_TO_TICKS(20000)));
        // esp_event_post_to(slow_service_loop, WIFI_SET, START_ESPNOW, NULL, 0, 1000);

}


                                                                          \


void screen_change_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)
{
    if(new_screen >= SIZE_LIST_TASKS || new_screen < 0) return;
    static esp_event_handler_instance_t 
                        direction_handler = NULL, 
                        show_handler = NULL, 
                        service_handler = NULL;
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


void set_time_handler(void* handler_args, esp_event_base_t base, int32_t method_update, void* time_data)
{
    main_data_t *main_data = (main_data_t *)handler_args;
    switch(method_update) {
        case UPDATE_TIME_FROM_UART:
        {
            uint8_t *new_time = (uint8_t*)time_data;
            if(new_time){
                for(uint8_t i=0, temp=0; i<SIZE_BUF_TIME; i++) {
                    temp = new_time[i];
                    if(i == BYTE_WEAK_DAY) {
                        cur_WEEK_DAY = (temp + 6) % 7;
                    }
                    else {
                        cur_CLOCK[i] = GET_DEC(temp);
                    }
                }
                free(new_time);
            }
            break;
        } 
        case UPDATE_TIME_FROM_MS:
        {
            time_t *time_now = (time_t *)time_data;
            if(!time_now)return;
            struct timeval now = {
                .tv_sec = (time_t)time_now/1000,
                .tv_usec = (suseconds_t)time_now%(uint64_t)1000
            };
            settimeofday(&now, NULL);
        }
        case UPDATE_TIME_FROM_SNTP:
        {
            static struct tm timeinfo;
            time_t time_now;
            time(&time_now);
            localtime_r(&time_now, &timeinfo);
            if(timeinfo.tm_year > 1900) {
                cur_HOUR = timeinfo.tm_hour;
                cur_SEC = timeinfo.tm_sec;
                cur_MIN = timeinfo.tm_min;
                cur_DAY = timeinfo.tm_mday;
                cur_MONTH = timeinfo.tm_mon;
                cur_YEAR = timeinfo.tm_year;
                cur_WEEK_DAY = timeinfo.tm_wday;
                // if(sync_TIME)dwin_clock_set(main_data);
            } 
            // else if(method_update != UPDATE_TIME_FROM_MS) {
            //     xTaskNotify(task_service_wifi, INIT_SNTP, eSetBits);
            // }
            break;
        }
        default : return;
    }
    esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
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
        if(fast_service_loop){
            esp_event_loop_run(fast_service_loop, TICKS_TO_RUN_LOOP);
        }
        vTaskDelay(DELAY_FAST_LOOP);
    }
}


void slow_services_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        if(slow_service_loop){
            esp_event_loop_run(slow_service_loop, TICKS_TO_RUN_LOOP);
        }
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
