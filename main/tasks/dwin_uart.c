#include "dwin_uart.h"


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
    uart_set_pin(UART_DWIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    esp_log_level_set("Uart", ESP_LOG_INFO);
}

void uart_event_task(void *pv)
{
    bool heder_ok;    
	int byte_rx_count, rxBytes;
    uint8_t *buf_RX = (uint8_t*) malloc(UART_BUF_SIZE);
    assert(buf_RX);
    uart_event_t event;
for(;;) {
    if(xQueueReceive(dwin_uart_events_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
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
                        if(buf_RX[INDEX_IDENTIF_DATA_IN_RX] == TOUCH_CODE) {
                            xQueueSend(queue_direct, &buf_RX[INDEX_START_DATA_IN_RX], 300);
                        } else if(buf_RX[INDEX_IDENTIF_DATA_IN_RX] == KEY_GET_CLOCK){
                            struct tm tm_time = {
                                .tm_year = HEX_TO_DEC(buf_RX[3])+100,
                                .tm_mon  = HEX_TO_DEC(buf_RX[4]),
                                .tm_mday = HEX_TO_DEC(buf_RX[5]),
                                .tm_hour = HEX_TO_DEC(buf_RX[7]),
                                .tm_min  = HEX_TO_DEC(buf_RX[8]),
                                .tm_sec  = HEX_TO_DEC(buf_RX[9]),
                            };
                            set_time_tm(&tm_time);
                            set_new_command(UPDATE_TIME_COMPLETE);
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
