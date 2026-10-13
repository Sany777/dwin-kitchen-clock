#pragma once

/* Board wiring and fixed hardware settings. */
#define DEVICE_DWIN_UART_PORT 0
#define DEVICE_DWIN_UART_BAUD_RATE 115200
#define DEVICE_DWIN_UART_BUFFER_SIZE 512
#define DEVICE_DWIN_UART_TX_GPIO 21
#define DEVICE_DWIN_UART_RX_GPIO 20

/* Set to the wired DWIN BUSY GPIO to enable transmit flow control; -1 disables it. */
#define DEVICE_DWIN_BUSY_GPIO (-1)
#define DEVICE_DWIN_BUSY_LEVEL 0

#define DEVICE_BUZZER_LOUD_LEVEL 80
#define DEVICE_BUZZER_NORMAL_LEVEL 60

#define DEVICE_AP_IP "http://192.168.4.1"
#define DEVICE_AP_WIFI_SSID "NASA"
#define DEVICE_AP_WIFI_PASSWORD "dwin Second"
#define DEVICE_AP_WIFI_CHANNEL 1
