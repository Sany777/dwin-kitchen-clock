#pragma once

#include "dwin_common.h"


#define SIZE_BUF_FLOAT          6
#define MAX_DIGIT_INT_NUMBER    9
#define TWO_DIGIT               2
#define BYTE_WEAK_DAY           3
#define SIZE_BUF_INT            (MAX_DIGIT_INT_NUMBER)
#define SIZE_BUF_UINT8          (TWO_DIGIT)

void send_dwin_str(const char * format, ... );

#define send_char(c)                                                        \
                            do{                                             \
                                const char arr[1] ={c};                     \
                                uart_write_bytes(UART_DWIN, arr, 1);        \
                             }while(0)

#define send_str_dwin(str)                                                      \
                            do{                                                 \
                                size_t str_len = strlen(str);                   \
                                if(str_len < MAX_DWIN_DATA){                    \
                                    uart_write_bytes(UART_DWIN, str, str_len);  \
                                }                                               \
                            }while(0)

#define send_message_dwin(message_str)                                                  \
                                        do{                                             \
                                            dwin_set_pic(NO_WEATHER_PIC);               \
                                            vTaskDelay(DELAY_CHANGE_PIC);               \
                                            send_in_frame(2, 3, 3, LEMON, message_str); \
                                            vTaskDelay(DELAY_SHOW_MESSAGE);             \
                                        }while(0)



int send_with_eof(const char *data_to_send);
void send_number(int number);
void send_without_zero(const uint8_t number);
void send_without_zero(const uint8_t number);
void send_clock(const uint8_t hour, const uint8_t min);
void send_temperature(temperature_t int_as_float);
void send_in_frame(uint8_t row, 
                    const uint8_t column, 
                    const uint8_t font, 
                    const uint16_t color, 
                    const char *message
                    );
void show_server(uint8_t *mac, char *str);
