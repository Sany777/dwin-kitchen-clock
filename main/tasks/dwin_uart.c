#include "dwin_uart.h"

#include "driver/gpio.h"

void uart_event_task(void *pv)
{
     const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    QueueHandle_t dwin_uart_events_queue = NULL;
    ESP_ERROR_CHECK(uart_driver_install(UART_DWIN, UART_BUF_SIZE * 2, 0, 10, (QueueHandle_t *)&dwin_uart_events_queue, 0));
    assert(dwin_uart_events_queue);
    ESP_ERROR_CHECK(uart_param_config(UART_DWIN, &uart_config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    uart_set_pin(UART_DWIN, UART_PIN_NO_CHANGE, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
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
    bool heder_ok;    
	size_t byte_rx_count, rxBytes;
    char *buf_RX = (char*) malloc(UART_BUF_SIZE);
    assert(buf_RX);
    char data_intifier;
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
                buf_RX[rxBytes] = 0;
                send_str_dwin(buf_RX);
                while(rxBytes--) {
                    if (heder_ok) {
                        if((buf_RX[byte_rx_count - 3] == 0xCC)      \
                            && (buf_RX[byte_rx_count - 2] == 0x33)  \
                            && (buf_RX[byte_rx_count - 1] == 0xC3)  \
                            && (buf_RX[byte_rx_count] == 0x3C)      \
                        ) {
                            data_intifier = buf_RX[INDEX_IDENTIF_DATA_IN_RX];
                            if(data_intifier == KEY_GET_CLOCK) {
                                uint8_t *buf_time = (uint8_t*) malloc(SIZE_BUF_TIME);
                                if(buf_time == NULL)break;
                                memcpy((uint8_t*)buf_time, (uint8_t*)&buf_RX[INDEX_START_DATA_IN_RX], SIZE_TIME);
                                esp_event_post_to(
                                        loop_service,
                                        EVENTS_SET_TIME,
                                        KEY_GET_CLOCK,
                                        buf_time,
                                        SIZE_BUF_TIME,
                                        TIMEOUT_PUSH_KEY
                                    );
                            } else if(data_intifier == KEY_READ_COMMAND) {
                                uint32_t key = buf_RX[INDEX_START_DATA_IN_RX];
                                if(KEY_IS_SET_TASK(data_intifier)) {
                                    esp_event_post_to(
                                        loop_service,
                                        EVENTS_MANAGER,
                                        key,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );
                                } else {
                                    esp_event_post_to(
                                        loop_service,
                                        EVENTS_DIRECTION,
                                        key,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY
                                    );
                                }
                            }
                            break;
                        } else {
                            byte_rx_count++;
                            if(byte_rx_count >= SIZE_BUF) {
                                break;
                            }
                        }
                    } else if (buf_RX[byte_rx_count] == FRAME_HEADER) {
                        heder_ok = true;
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
