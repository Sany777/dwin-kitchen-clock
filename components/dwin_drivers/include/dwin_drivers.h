#pragma once 

#include "dwin_commands.h"
#include "dwin_common.h"
#include <time.h>

#define KEY_GET_CLOCK (155)

#define INDEX_IDENTIF_DATA_IN_RX    (1)
#define INDEX_START_DATA_IN_RX      (2)
#define INDEX_IDENTIF_CHAR_IN_RX    (3)

#define cancel_text_box() dwin_send_hello()

void dwin_clock_get();
int dwin_init_update();
int dwin_send_hello();
int dwin_print_end();
int dwin_clear_screen();

void dwin_sound_buzzer(const uint8_t loud);
void dwin_set_brightness(const uint8_t brightness);
void dwin_set_picture(const uint8_t picture_id);				
void dwin_set_cursor(uint16_t row, uint16_t column, const uint16_t text_color, size_t font);
void dwin_save_picture(const uint8_t pic);
void dwin_set_text_box(const uint16_t x_s, const uint16_t y_s, const uint16_t x_e, const uint16_t y_e);
void dwin_set_color(const uint16_t foreground, const uint16_t background);
void dwin_draw_circle(const uint16_t x, const uint16_t y, const uint16_t radius, const bool fill);
void dwin_draw_lines(const uint16_t *points, const size_t number_point, uint16_t x_start, const size_t width, const uint16_t y);
void dwin_draw_rectangle(const uint16_t x_s, const uint16_t y_s, const uint16_t x_e, const uint16_t y_e, const bool fill);
void dwin_set_cursor_absolute(uint16_t row, uint16_t column, const uint16_t text_color, const uint8_t font);
void dwin_clock_set(const struct tm *tmptr);
void dwin_fill_area(const uint16_t x_s, const uint16_t y_s, const uint16_t color);
void dwin_clock_on(const uint16_t row, const uint16_t column, const uint16_t text_color, const uint8_t font);
void dwin_clock_off(void);
void dwin_draw_text_box(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, uint16_t color_text, uint16_t color_rect, uint16_t font, const char* str);
