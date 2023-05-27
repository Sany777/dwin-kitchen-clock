#include "dwin_drivers.h"


void dwin_clock_on(const uint16_t row, const uint16_t column, const uint16_t text_color, const uint8_t font) 
{
    CLOCK_ON[CLOCK_FONT] = font;
    CLOCK_ON[CLOCK_COLOUR_1] = text_color>>8;
    CLOCK_ON[CLOCK_COLOUR_2] = text_color<<8;
	CLOCK_ON[CLOCK_COL_1] = column>>8;
    CLOCK_ON[CLOCK_COL_2] = column<<8;
    CLOCK_ON[CLOCK_ROW_1] = row>>8;
    CLOCK_ON[CLOCK_ROW_2] = row<<8;
	uart_write_bytes(UART_DWIN, CLOCK_ON, sizeof(CLOCK_ON));
}


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


void dwin_print(uint16_t row, uint16_t column, const uint16_t text_color, const uint8_t font) 
{
	column *= font * 8;
	row *= font * 8;
    PRINT[PRINT_FONT] = font;
    PRINT[PRINT_COLOUR_1] = text_color>>8;
    PRINT[PRINT_COLOUR_2] = text_color<<8;
	PRINT[PRINT_COL_1] = column>>8;
    PRINT[PRINT_COL_2] = column<<8;
    PRINT[PRINT_ROW_1] = row>>8;
    PRINT[PRINT_ROW_2] = row<<8;
	uart_write_bytes(UART_DWIN, PRINT, sizeof(PRINT));
}

void dwin_clock_set(const main_data_t *data_dwin) 
{
    uint8_t time_to_send[SIZE_SENDING_TIME];
	uart_write_bytes(UART_DWIN, HEADER_CLOCK_SET, sizeof(HEADER_CLOCK_SET));
	for (uint8_t count_cur_time=0, count_send_time=0; count_cur_time<SIZE_TIME; count_cur_time++) {
        if(count_cur_time != INDEX_WEEK_DAY) {
		    time_to_send[count_send_time++] = cur_CLOCK[count_cur_time];
        }
	}
    uart_write_bytes(UART_DWIN, time_to_send, SIZE_SENDING_TIME);
	print_end();
}

void save_pic(const uint8_t pic)
{
    SAVE_PIC[INDEX_VARIABLE_VALUE] = pic;
    uart_write_bytes(UART_DWIN, SAVE_PIC, sizeof(SAVE_PIC));
}

void set_text_box(const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e)
{
    SET_TEXT_BOX[2] = x_s<<8;
    SET_TEXT_BOX[3] = x_s>>8;
    SET_TEXT_BOX[4] = y_s<<8;
    SET_TEXT_BOX[5] = y_s>>8;
    SET_TEXT_BOX[6] = x_e<<8;
    SET_TEXT_BOX[7] = x_e>>8;
    SET_TEXT_BOX[8] = y_e<<8;
    SET_TEXT_BOX[9] = y_e>>8;
    uart_write_bytes(UART_DWIN, SET_TEXT_BOX, sizeof(SET_TEXT_BOX));
}

void set_color(const uint16_t foreground, const uint16_t background)
{
    SET_COLOR[2] = foreground<<8;
    SET_COLOR[3] = foreground>>8;
    SET_COLOR[4] = background<<8;
    SET_COLOR[5] = background>>8;
    uart_write_bytes(UART_DWIN, SET_COLOR, sizeof(SET_COLOR));
}

void print_circle(const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill)
{
    CIRCULAR[1] = fill 
                ? COMMAND_CIRCULAR_FILL_FOREGROUND 
                : COMMAND_CIRCULAR_FOREGROUND;
    CIRCULAR[2] = x>>8;
    CIRCULAR[3] = x<<8;
    CIRCULAR[4] = y>>8;
    CIRCULAR[5] = y<<8;
    CIRCULAR[6] = radius>>8;
    CIRCULAR[7] = radius<<8;
    uart_write_bytes(UART_DWIN, CIRCULAR, sizeof(CIRCULAR));
}

#define BYTE_PER_POINT_LINE 2
void print_lines(const uint16_t *points, 
                const int number_point, 
                bool foreground)
{
    const size_t len_command = 
                    number_point*BYTE_PER_POINT_LINE
                    + LEN_FRAME;
    uint8_t *line_command = malloc(len_command);
    if(line_command){
        line_command[0] = FRAME_HEADER;
        line_command[1] = foreground
                        ? COMMAND_LINE_DISPLAY_FOREGROUND
                        : COMMAND_LINE_DISPLAY_BACKGROUND;
        if(line_command){
            for(int command_i=INDEX_VARIABLE_VALUE, 
                        points_i=0; points_i<number_point;)
            {
                line_command[command_i] = points[points_i]<<8;
                line_command[command_i++] = points[points_i++]>>8;
                line_command[command_i] = points[points_i]<<8;
                line_command[command_i++] = points[points_i++]>>8;
            }
        }
        uart_write_bytes(UART_DWIN, line_command, len_command);
        print_end(); 
    }
}



void print_broken_line(const uint16_t *y_points, 
                const int points_number,
                const uint16_t x_start,
                const uint16_t x_end)
{
    const size_t len_command = points_number * BYTE_PER_POINT_LINE
                                    + LEN_FRAME;
    uint8_t *line_command = (uint8_t *)malloc(len_command);
    if(line_command){
        line_command[0] = FRAME_HEADER;
        line_command[1] = COMMAND_BROKEN_LINE;
        if(line_command){
            for(int command_i=INDEX_VARIABLE_VALUE, 
                        points_i=0; points_i<points_number; 
                        points_i++)
            {
                line_command[command_i++] = y_points[points_i]<<8;
                line_command[command_i++] = y_points[points_i]>>8;
            }
        }
        uart_write_bytes(UART_DWIN, line_command, len_command);
        print_end(); 
    }
}

void print_rect(const uint16_t x_s, const uint16_t y_s, const uint16_t x_e, const uint16_t y_e)
{
    RECTANGLE_ON[2] = x_s<<8;
    RECTANGLE_ON[3] = x_s>>8;
    RECTANGLE_ON[4] = y_s<<8;
    RECTANGLE_ON[5] = y_s>>8;
    RECTANGLE_ON[2] = x_e<<8;
    RECTANGLE_ON[3] = x_e>>8;
    RECTANGLE_ON[4] = y_e<<8;
    RECTANGLE_ON[5] = y_e>>8;
    uart_write_bytes(UART_DWIN, RECTANGLE_ON, sizeof(RECTANGLE_ON));
}

