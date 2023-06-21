#pragma once 

#include "dwin_commands.h"
#include "dwin_common.h"

/*data identifier from dwin*/

#define KEY_GET_CLOCK (155)

/*index byte in RX buffer*/
#define INDEX_IDENTIF_DATA_IN_RX    (1)
#define INDEX_START_DATA_IN_RX      (2)
#define INDEX_IDENTIF_CHAR_IN_RX    (3)

#define init_update_dwin()			uart_write_bytes(UART_DWIN, INIT_UPDATE, sizeof(INIT_UPDATE))
#define send_hello()  				uart_write_bytes(UART_DWIN, HELLO_COMMAND, sizeof(HELLO_COMMAND))
#define print_end()					uart_write_bytes(UART_DWIN, END, sizeof(END));
#define hide_rect() 				uart_write_bytes(UART_DWIN, RECTANGLE_OFF, sizeof(RECTANGLE_OFF))
#define cancel_text_box()			uart_write_bytes(UART_DWIN, HELLO_COMMAND, sizeof(HELLO_COMMAND))
#define clear_screen()				uart_write_bytes(UART_DWIN, CLEAR_SCREEN, sizeof(CLEAR_SCREEN))
#define dwin_clock_get() 	        uart_write_bytes(UART_DWIN, GET_TIME, sizeof(GET_TIME))

void dwin_buzer(const uint8_t loud);
void dwin_set_brightness(const uint8_t brightness);
void dwin_set_pic(const uint8_t picture_id);				
									

void print_start(uint16_t row, uint16_t column, 
					const uint16_t text_color, 
					size_t font) ;
void save_pic(const uint8_t pic);
void set_text_box(const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e);
void set_color(const uint16_t foreground, const uint16_t background);
void print_circle(const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill);
void print_lines(  const uint16_t *points, 
                    const size_t number_point,
                    uint16_t x_start,
                    const size_t width,
					const uint16_t y);
void print_rect(const uint16_t x_s, 
				const uint16_t y_s, 
				const uint16_t x_e, 
				const uint16_t y_e,
				bool fill);

void print_start_pos(uint16_t row, uint16_t column, const uint16_t text_color, uint8_t font);
void dwin_clock_set(const struct tm *);
void fill_area(const uint16_t x_s, 
                const uint16_t y_s, 
                const uint16_t color);
void dwin_clock_on(const  uint16_t row, 
                    const uint16_t column, 
                    const uint16_t textColor, 
                    const uint8_t font);
void dwin_clock_off(void);
void print_text_box(const uint16_t x, 
                        const uint16_t y, 
                        const uint16_t width, 
                        const uint16_t height, 
                        uint16_t color_text, 
                        uint16_t color_rect, 
                        uint16_t font,
                        const char* str);

