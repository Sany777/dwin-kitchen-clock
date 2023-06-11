#include "dwin_uart.h"

QueueHandle_t dwin_uart_events_queue;

void init_uart()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_DWIN, UART_BUF_SIZE, 0, SIZE_UART_EVENTS, (QueueHandle_t *)&dwin_uart_events_queue, 0));
    assert(dwin_uart_events_queue);
    ESP_ERROR_CHECK(uart_param_config(UART_DWIN, &uart_config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    uart_set_pin(UART_DWIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // ESP_ERROR_CHECK(gpio_sleep_set_direction(
    //                             RXD_PIN, 
    //                             GPIO_MODE_INPUT));
    // ESP_ERROR_CHECK(gpio_sleep_set_pull_mode(
    //                             RXD_PIN, 
    //                             GPIO_PULLUP_ONLY));
    // ESP_ERROR_CHECK(uart_set_wakeup_threshold(
    //                             UART_DWIN, 
    //                             UART_WAKEUP_THRESHOLD));
    // ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(
    //                         UART_DWIN));
}


void uart_event_task(void *pv)
{
    bool heder_ok;    
	size_t byte_rx_count, rxBytes;
    char *buf_RX = (char*) malloc(UART_BUF_SIZE);
    assert(buf_RX);
    uart_event_t event;
for(;;) {
    if(xQueueReceive(dwin_uart_events_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
        switch(event.type) {
        case UART_DATA:
        {
            rxBytes = uart_read_bytes(UART_DWIN, buf_RX,
                            event.size,
                            portMAX_DELAY);
            byte_rx_count = 0;
            heder_ok = false;
            while(rxBytes--) {
                if (heder_ok) {
                    if((buf_RX[byte_rx_count - 3] == 0xCC)      
                        && (buf_RX[byte_rx_count - 2] == 0x33)  
                        && (buf_RX[byte_rx_count - 1] == 0xC3)  
                        && (buf_RX[byte_rx_count] == 0x3C))
                    {
                        if(buf_RX[INDEX_IDENTIF_DATA_IN_RX] == KEY_READ_COMMAND) {
                            uint32_t key = buf_RX[INDEX_START_DATA_IN_RX];
                            if(key){
                                if(KEY_IS_SET_TASK(key)) {
                                    esp_event_post_to(
                                        direct_loop,
                                        EVENTS_DIRECTION,
                                        KEY_CLOSE,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );
                                    esp_event_post_to(
                                        direct_loop,
                                        EVENTS_MANAGER,
                                        GET_SCREEN_TASK(key),
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );
                                } else {
                                    esp_event_post_to(
                                        direct_loop,
                                        EVENTS_DIRECTION,
                                        key,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );  
                                }
                            } else {
                                key = buf_RX[INDEX_IDENTIF_CHAR_IN_RX];
                                esp_event_post_to(
                                        direct_loop,
                                        EVENTS_DIRECTION,
                                        key,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );
                                }
                        } else if(buf_RX[INDEX_IDENTIF_DATA_IN_RX] == KEY_GET_CLOCK){
                            struct tm tm_time = {
                                .tm_year = GET_DEC(buf_RX[1]),
                                .tm_mon = GET_DEC(buf_RX[2]),
                                .tm_mday = GET_DEC(buf_RX[3]),
                                .tm_hour = GET_DEC(buf_RX[5]),
                                .tm_min = GET_DEC(buf_RX[6]),
                                .tm_sec = GET_DEC(buf_RX[7]),
                            };
                            set_time_tm(&tm_time, false);
                        } else {
                            xEventGroupSetBits(dwin_event_group, BIT_DWIN_RESPONSE_OK);
                        }
                        break;    
                    }
                }  else if (buf_RX[byte_rx_count] == FRAME_HEADER) {
                    heder_ok = true;
                }
                byte_rx_count++;
                if(byte_rx_count >= SIZE_BUF) {
                    break;
                }
            }
            break;
        }
        case UART_FIFO_OVF:
        {
            uart_flush_input(UART_DWIN);
            xQueueReset(dwin_uart_events_queue);
            break;
        }
        case UART_BUFFER_FULL:
        {
            uart_flush_input(UART_DWIN);
            xQueueReset(dwin_uart_events_queue);
            break;
        }
        default:
            break;
        }
    }
    }
}
