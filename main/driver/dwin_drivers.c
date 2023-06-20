#include "dwin_drivers.h"

void print_start(uint16_t row, uint16_t column, const uint16_t text_color, size_t font) 
{
	column = column * font * 8;
	row = row * font * 11;
    print_start_pos(row, column, text_color, font);
}

void dwin_buzer(uint8_t loud)
{
    uint8_t full_command_buzzer[] = {
        FRAME_HEADER, 
        COMMAND_BUZZER, 
        loud, 
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, full_command_buzzer, sizeof(full_command_buzzer));
}

void dwin_set_pic(uint8_t picture_id)
{
    if(picture_id < END_LIST_PIC){ 	
        uint8_t SET_PIC[] = {
            FRAME_HEADER, 
            COMMAND_SET_PIC, 
            picture_id,
            FRAME_END
        };						
        uart_write_bytes(UART_DWIN, SET_PIC, sizeof(SET_PIC));
    }
} 	

void dwin_set_brightness(uint8_t brightness)
{
    if(brightness >= 10 && brightness <= 100){ 
        uint8_t set_bright[] = {
            FRAME_HEADER, 
            COMMAND_SET_BRIGHT, 
            DEC_TO_HEX(brightness),
            FRAME_END
        };
        uart_write_bytes(UART_DWIN, set_bright, sizeof(set_bright));	
    }
}

void print_start_pos(uint16_t row, uint16_t column, const uint16_t text_color, size_t font) 
{
    uint8_t print_command[] = {
        FRAME_HEADER, 
        COMMAND_PRINT, 
        column/256, 
        column%256, 
        row/256, 
        row%256, 
        0x00,
        0x81,
        text_color/256,
        text_color%256,
        font,
        0xFF,
        0XFF
    };
	uart_write_bytes(UART_DWIN, print_command, sizeof(print_command));
}

void save_pic(const uint8_t pic)
{
    uint8_t save_command[] = {
        FRAME_HEADER, 
        COMMAND_PIC_SAVE, 
        pic,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, save_command, sizeof(save_command));
}

void set_text_box(  const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e  )
{
    uint8_t SET_TEXT_BOX[] = {
        FRAME_HEADER, 
        COMMAND_SET_BOX,
        x_s/256,
        x_s%256,
        y_s/256,
        y_s%256,
        x_e/256,
        x_e%256,
        y_e/256,
        y_e%256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, SET_TEXT_BOX, sizeof(SET_TEXT_BOX));
}

void set_color(const uint16_t foreground, const uint16_t background)
{
    uint8_t SET_COLOR[] = {
        FRAME_HEADER,
        COMMAND_SET_COLOR,
        foreground/256,
        foreground%256,
        background/256,
        background%256,
        FRAME_END
    }; 
    uart_write_bytes(UART_DWIN, SET_COLOR, sizeof(SET_COLOR));
}

void print_circle(  const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill  )
{
    uint8_t CIRCULAR[] = {
        FRAME_HEADER,
        fill 
            ? COMMAND_CIRCULAR_FILL_FOREGROUND 
            : COMMAND_CIRCULAR_FOREGROUND,
        x/256,
        x%256,
        y/256,
        y%256,
        radius/256,
        radius%256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, CIRCULAR, sizeof(CIRCULAR));
}


void print_lines( const uint16_t *points, 
                    const size_t number_point,
                    uint16_t x_start,
                    const size_t width,
                    const uint16_t y)
{
    uint16_t step = width/number_point;
    send_char(0XAA);
    send_char(0X56);
    for(uint16_t i=0, tmp=0; i<number_point; i++){
        send_char(x_start/256);
        send_char(x_start%256);
        tmp = points ? points[i] : 20;
        send_char((y - tmp )/256);
        send_char((y - tmp)%256);
        x_start += step;
    }
    print_end(); 
}


void print_text_box(const uint16_t x, 
                        const uint16_t y, 
                        const uint16_t width, 
                        const uint16_t height, 
                        uint16_t color_text, 
                        uint16_t color_rect, 
                        uint16_t font,
                        const char* str)
{
    uint16_t 
        x_e = x+width,
        y_e = y+height;
    set_color(color_rect, BLACK);
    print_rect(x, y, x_e, y_e, true);
    print_start_pos(y+font*2,x+font*2,color_text, font);
    send_str_dwin(str);
    print_end();
}

void print_rect(const uint16_t x, const uint16_t y, const uint16_t x_e, const uint16_t y_e, bool fill )
{
    char RECTANGLE_ON[] = {
        FRAME_HEADER, 
        fill
            ? COMMAND_FILL_RECTANGLE
            : COMMAND_BOX_RECTANGLE, 
        x/256, 
        x%256, 
        y/256,
        y%256, 
        x_e/256, 
        x_e%256, 
        y_e/256, 
        y_e%256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, RECTANGLE_ON, sizeof(RECTANGLE_ON));
}

void dwin_clock_set(const struct tm *tmptr) 
{
    uint8_t clock_send[] = {
        HEADER_SET_CLOCK,
        DEC_TO_HEX(tmptr->tm_year%100),
        DEC_TO_HEX(tmptr->tm_mon),
        DEC_TO_HEX(tmptr->tm_mday),
        DEC_TO_HEX(tmptr->tm_hour),
        DEC_TO_HEX(tmptr->tm_min),
        DEC_TO_HEX(tmptr->tm_sec),
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, clock_send, sizeof(clock_send));
}

void fill_area(const uint16_t xs, const uint16_t ys, const uint16_t color)
{
    uint8_t fill_area_command[] = {
        FRAME_HEADER,
        COMMAND_FILL,
        xs/256,
        xs%256,
        ys/256,
        ys/256,
        color/256,
        color/256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, fill_area_command, sizeof(fill_area_command));
}


void dwin_clock_on(const uint16_t row, const  uint16_t column, const  uint16_t textColor, const  uint8_t font)
{
    char clock_on_command[] = {
        FRAME_HEADER,
        CLOCK_ON_COMMAND,
        font,
        textColor / 256,
        textColor % 256,
        column / 256,
        column % 256, 
        row / 256, row % 256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, clock_on_command, sizeof(clock_on_command));
}

void dwin_clock_off(void)
{
    char clock_off_command[] = {
        FRAME_HEADER,
        CLOCK_OFF_COMMAND,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, clock_off_command, sizeof(clock_off_command));
}