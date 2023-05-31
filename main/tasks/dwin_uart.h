#pragma once

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "dwin_common.h"

void uart_event_task(void *pv);
void init_uart();


    // int rxBytes = uart_read_bytes(UART_DWIN, (void*)buf_RX, UART_BUF_SIZE, pdMS_TO_TICKS(2000));
    // if(rxBytes){
    //     buf_RX[rxBytes] = 0;
    //     send_str_dwin((const char *)buf_RX);     

    // }