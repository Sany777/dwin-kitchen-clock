#pragma once

#include "dwin_common.h"


#define SIZE_BUF_FLOAT          6
#define MAX_DIGIT_INT_NUMBER    9
#define TWO_DIGIT               2
#define BYTE_WEAK_DAY           3
#define SIZE_BUF_INT            (MAX_DIGIT_INT_NUMBER)
#define SIZE_BUF_UINT8          (TWO_DIGIT)
void print_hide_pwd(const char* pwd);
void send_str(const char * format, ... );

#define send_char(c)                                                        \
                            do{                                             \
                                const uint8_t arr[1] ={c};                     \
                                uart_write_bytes(UART_DWIN, arr, 1);        \
                             }while(0)

#define send_str_dwin(str)                                                          \
                            do{                                                     \
                                uart_write_bytes(UART_DWIN, str, strlen(str));  \
                            }while(0)

#define send_message_dwin(message_str)                                                  \
                                        do{                                             \
                                            dwin_set_pic(NO_WEATHER_PIC);               \
                                            vTaskDelay(DELAY_CHANGE_PIC);               \
                                            send_in_frame(2, 3, 3, LEMON, message_str); \
                                            vTaskDelay(DELAY_SHOW_MESSAGE);             \
                                        }while(0)




void send_in_frame(uint8_t row, 
                    const uint8_t column, 
                    const uint8_t font, 
                    const uint16_t color, 
                    const char *message
                    );
