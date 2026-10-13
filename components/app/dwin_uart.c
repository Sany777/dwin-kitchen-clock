#include "dwin_uart.h"
#include "dwin_drivers.h"
#include "dwin_commands.h"
#include "dwin_timer.h"
#include "wifi_dwin.h"
#include "dwin_help.h"
#include "driver/gpio.h"

#define DWIN_FRAME_END_SIZE 4

static bool
frame_has_end(const uint8_t *frame, size_t length)
{
    return length >= DWIN_FRAME_END_SIZE &&
           frame[length - 4] == 0xCC && frame[length - 3] == 0x33 &&
           frame[length - 2] == 0xC3 && frame[length - 1] == 0x3C;
}

static void
handle_dwin_frame(const uint8_t *frame, size_t length)
{
    if (length < 2) {
        return;
    }

    if (frame[INDEX_IDENTIF_DATA_IN_RX] == TOUCH_CODE) {
        if (length < INDEX_START_DATA_IN_RX + sizeof(uint16_t) + DWIN_FRAME_END_SIZE) {
            ESP_LOGW("Uart", "Ignoring short touch frame (%u bytes)", (unsigned)length);
            return;
        }
        const uint16_t touch = (uint16_t)frame[INDEX_START_DATA_IN_RX] |
                               ((uint16_t)frame[INDEX_START_DATA_IN_RX + 1] << 8);
        ESP_LOGI("Uart", "Touch received: command=0x%02x symbol=0x%02x",
                 frame[INDEX_START_DATA_IN_RX], frame[INDEX_START_DATA_IN_RX + 1]);
        if (xQueueSend(direct_queue, &touch, pdMS_TO_TICKS(300)) != pdTRUE) {
            ESP_LOGW("Uart", "Touch queue full; dropping event 0x%04x", touch);
        }
    } else if (frame[INDEX_IDENTIF_DATA_IN_RX] == KEY_GET_CLOCK) {
        if (length < 14) {
            ESP_LOGW("Uart", "Ignoring short clock frame (%u bytes)", (unsigned)length);
            return;
        }
        struct tm tm_time = {
            .tm_year = HEX_TO_DEC(frame[3]) + 100,
            .tm_mon  = HEX_TO_DEC(frame[4]),
            .tm_mday = HEX_TO_DEC(frame[5]),
            .tm_hour = HEX_TO_DEC(frame[7]),
            .tm_min  = HEX_TO_DEC(frame[8]),
            .tm_sec  = HEX_TO_DEC(frame[9]),
        };
        wifi_dwin_set_time_tm(&tm_time);
        dwin_timer_set_new_command(UPDATE_TIME_COMPLETE);
    } else {
        xEventGroupSetBits(dwin_event_group, BIT_DWIN_RESPONSE_OK);
    }
}

void init_uart()
{
    const uart_config_t uart_config = {
        .baud_rate = DEVICE_DWIN_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(DEVICE_DWIN_UART_PORT, DEVICE_DWIN_UART_BUFFER_SIZE, 0, SIZE_UART_EVENTS, (QueueHandle_t *)&dwin_uart_events_queue, 0));
    assert(dwin_uart_events_queue);
    ESP_ERROR_CHECK(uart_param_config(DEVICE_DWIN_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(DEVICE_DWIN_UART_PORT, DEVICE_DWIN_UART_TX_GPIO,
                                 DEVICE_DWIN_UART_RX_GPIO, UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(gpio_set_pull_mode(DEVICE_DWIN_UART_RX_GPIO, GPIO_PULLUP_ONLY));
#if DEVICE_DWIN_BUSY_GPIO >= 0
    const gpio_config_t busy_gpio_config = {
        .pin_bit_mask = 1ULL << DEVICE_DWIN_BUSY_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&busy_gpio_config));
#endif
    esp_log_level_set("Uart", ESP_LOG_INFO);
}

void task_uart_event(void *pv)
{
    size_t frame_length = 0;
    uint8_t frame[SIZE_BUF];
    uint8_t buf_RX[DEVICE_DWIN_UART_BUFFER_SIZE];
    uart_event_t event;

    for (;;) {
        if (xQueueReceive(dwin_uart_events_queue, &event, pdMS_TO_TICKS(1000)) != pdTRUE) {
            continue;
        }

        switch (event.type) {
        case UART_DATA: {
            const size_t requested = MIN((size_t)event.size, sizeof(buf_RX));
            ESP_LOGI("Uart", "UART_DATA event: size=%u", (unsigned)event.size);
            const int rx_bytes = uart_read_bytes(DEVICE_DWIN_UART_PORT, buf_RX,
                                                 requested, pdMS_TO_TICKS(20));
            if (rx_bytes > 0) {
                ESP_LOGI("Uart", "Received %d UART bytes", rx_bytes);
                ESP_LOG_BUFFER_HEX_LEVEL("Uart RX", buf_RX, rx_bytes, ESP_LOG_INFO);
            }
            for (int i = 0; i < rx_bytes; i++) {
                const uint8_t byte = buf_RX[i];
                if (frame_length == 0) {
                    if (byte == FRAME_HEADER) {
                        frame[frame_length++] = byte;
                    }
                    continue;
                }

                if (frame_length == sizeof(frame)) {
                    ESP_LOGW("Uart", "Oversized DWIN frame; resynchronizing");
                    frame_length = 0;
                    if (byte == FRAME_HEADER) {
                        frame[frame_length++] = byte;
                    }
                    continue;
                }

                frame[frame_length++] = byte;
                if (frame_has_end(frame, frame_length)) {
                    handle_dwin_frame(frame, frame_length);
                    frame_length = 0;
                }
            }
            break;
        }
        case UART_FIFO_OVF:
        case UART_BUFFER_FULL:
            uart_flush_input(DEVICE_DWIN_UART_PORT);
            xQueueReset(dwin_uart_events_queue);
            frame_length = 0;
            ESP_LOGW("Uart", "UART RX overflow; discarded partial DWIN frame");
            break;
        default:
            break;
        }
    }
}
