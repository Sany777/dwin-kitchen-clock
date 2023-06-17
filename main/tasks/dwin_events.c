#include "dwin_events.h"

esp_event_loop_handle_t slow_service_loop, fast_service_loop;
uint8_t cur_screen_id = MAIN_SCREEN;
ESP_EVENT_DEFINE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DEFINE_BASE(SNTP_EVENTS);


void init_dwin_events(main_data_t *main_data) 
{
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


    set_new_event(MAIN_SCREEN);


}





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

void test_clock_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)                                                     
{
    send_str_dwin(asctime(get_time_tm()));
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
            if(cur_screen_id != MAIN_SCREEN){
                set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
            }
            if(KEY_IS_SET_TASK(data_in[0])){
                area_SCREEN = 0;
                screen_handler(main_data, KEY_CLOSE, 0);
                cur_screen_id = data_in[0];
                screen_handler(main_data, KEY_INIT, 0);
            } else  if(data_in[0] >= START_SERVICE_EVENTS && data_in[0] <= END_SERVICE_EVENTS){
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



void vApplicationIdleHook( void )
{ 
    TickType_t us_time_sleep = TIMER_WAKEUP_LONG_TIME_US;
    while (1) {
        
        sleep_dwin(us_time_sleep);
    }
}
