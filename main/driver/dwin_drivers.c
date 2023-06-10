#include "dwin_drivers.h"

void send_chunc(const char *data, const size_t data_len)
{
    static uint32_t addr = 0;
    if(data == NULL || data_len == 0){
        addr = 0;
        return;
    }
    for(int shift=6, i=INDEX_VARIABLE_VALUE; shift>0; shift--, i++){
        SEND_IMG[i] = addr<<shift*8;
    }
    uart_write_bytes(UART_DWIN, SEND_IMG, sizeof(SEND_IMG));
    uart_write_bytes(UART_DWIN, data, data_len);
    print_end();
    if(data_len<MAX_LEN_CHUNC_IMG){
        addr = 0;
    } else {
        addr += data_len;
    }
}

void print_start(uint16_t row, uint16_t column, const uint16_t text_color, size_t font) 
{
	column = column * font * 8;
	row = row * font * 11;
    PRINT[PRINT_FONT] = font;
	PRINT[PRINT_COL_1] = column/256;
    PRINT[PRINT_COL_2] = column%256;
    PRINT[PRINT_ROW_1] = row/256;
    PRINT[PRINT_ROW_2] = row%256;
    PRINT[PRINT_COLOR_1] = text_color/256;
    PRINT[PRINT_COLOR_2] = text_color%256;
	uart_write_bytes(UART_DWIN, PRINT, sizeof(PRINT));
}

void save_pic(const uint8_t pic)
{
    SAVE_PIC[INDEX_VARIABLE_VALUE] = pic;
    uart_write_bytes(UART_DWIN, SAVE_PIC, sizeof(SAVE_PIC));
}

void set_text_box(  const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e  )
{
    SET_TEXT_BOX[2] = x_s/256;
    SET_TEXT_BOX[3] = x_s%256;
    SET_TEXT_BOX[4] = y_s/256;
    SET_TEXT_BOX[5] = y_s/256;
    SET_TEXT_BOX[6] = x_e%256;
    SET_TEXT_BOX[7] = x_e/256;
    SET_TEXT_BOX[8] = y_e%256;
    SET_TEXT_BOX[9] = y_e/256;
    uart_write_bytes(UART_DWIN, SET_TEXT_BOX, sizeof(SET_TEXT_BOX));
}

void set_color(const uint16_t foreground, const uint16_t background)
{
    SET_COLOR[2] = foreground%256;
    SET_COLOR[3] = foreground/256;
    SET_COLOR[4] = background%256;
    SET_COLOR[5] = background/256;
    uart_write_bytes(UART_DWIN, SET_COLOR, sizeof(SET_COLOR));
}

void print_circle(  const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill  )
{
    CIRCULAR[1] = fill 
                ? COMMAND_CIRCULAR_FILL_FOREGROUND 
                : COMMAND_CIRCULAR_FOREGROUND;
    CIRCULAR[2] = x/256;
    CIRCULAR[3] = x%256;
    CIRCULAR[4] = y/256;
    CIRCULAR[5] = y%256;
    CIRCULAR[6] = radius/256;
    CIRCULAR[7] = radius%256;
    uart_write_bytes(UART_DWIN, CIRCULAR, sizeof(CIRCULAR));
}

/* format [x1,y1,x2,y2]*/
#define BYTE_PER_POINT_LINE 2
void print_lines(   const uint16_t *points, 
                    const size_t number_point, 
                    bool foreground  )
{
    send_char(FRAME_HEADER);
    send_char(foreground 
                ? COMMAND_LINE_DISPLAY_FOREGROUND
                : COMMAND_LINE_DISPLAY_BACKGROUND);
    uart_write_bytes(UART_DWIN, points, sizeof(uint16_t)*number_point);
    print_end(); 
}

void print_broken_line( const uint16_t *y_points, 
                        const size_t points_number,
                        const char x_start,
                        const char x_end )
{
    HEADER_SEND_BROKEN_LINE[2] = x_end;
    HEADER_SEND_BROKEN_LINE[3] = x_start;
    send_str_dwin(HEADER_SEND_BROKEN_LINE);
    uart_write_bytes(UART_DWIN, y_points, sizeof(uint16_t)*points_number);
    print_end(); 
}

void print_rect(const uint16_t x_s, const uint16_t y_s, const uint16_t x_e, const uint16_t y_e)
{
    RECTANGLE_ON[2] = x_s%256;
    RECTANGLE_ON[3] = x_s/256;
    RECTANGLE_ON[4] = y_s%256;
    RECTANGLE_ON[5] = y_s/256;
    RECTANGLE_ON[2] = x_e%256;
    RECTANGLE_ON[3] = x_e/256;
    RECTANGLE_ON[4] = y_e%256;
    RECTANGLE_ON[5] = y_e/256;
    uart_write_bytes(UART_DWIN, RECTANGLE_ON, sizeof(RECTANGLE_ON));
}

void dwin_clock_set(struct tm *tmptr) 
{
    uint8_t time_to_send[SIZE_BUF_CLOCK_SET] = {HEADER_SET_CLOCK};
    time_to_send[INDEX_YEAR]    = GET_HEX(tmptr->tm_year);
    time_to_send[INDEX_MONTH]   = GET_HEX(tmptr->tm_mon);
    time_to_send[INDEX_DAY]     = GET_HEX(tmptr->tm_mday);
    time_to_send[INDEX_HOUR]    = GET_HEX(tmptr->tm_hour);
    time_to_send[INDEX_MIN]     = GET_HEX(tmptr->tm_min);
    time_to_send[INDEX_SEC]     = GET_HEX(tmptr->tm_sec);
    uart_write_bytes(UART_DWIN, time_to_send, SIZE_BUF_CLOCK_SET);
	print_end();
}