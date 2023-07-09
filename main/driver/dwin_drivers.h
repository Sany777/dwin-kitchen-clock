#pragma once 

#include "dwin_commands.h"
#include "dwin_common.h"
#include <time.h>
/*data identifier from dwin*/

#define KEY_GET_CLOCK (155)

/*index byte in RX buffer*/
#define INDEX_IDENTIF_DATA_IN_RX    (1)
#define INDEX_START_DATA_IN_RX      (2)
#define INDEX_IDENTIF_CHAR_IN_RX    (3)

#define cancel_text_box()  send_hello()
void dwin_clock_get();
int init_update_dwin();
int send_hello();
int print_end();
int print_end();
int clear_screen();


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
void print_lines(const uint16_t *points, 
                    const size_t number_point,
                    uint16_t x_start,
                    const size_t width,
                    const uint16_t y);
void print_rect(const uint16_t x_s, 
				const uint16_t y_s, 
				const uint16_t x_e, 
				const uint16_t y_e,
				const bool fill);

void print_start_pos(uint16_t row, uint16_t column, 
                        const uint16_t text_color,
                        const uint8_t font);
void dwin_clock_set(const struct tm *tmptr);
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

