#include "dwin_drivers.h"
#include "show_data.h"

int dwin_init_update()
{
    const uint8_t INIT_UPDATE[] = {
        FRAME_HEADER,
        COMMAND_INIT_UPDATE,
        FRAME_END,
    };
	return uart_write_bytes(UART_DWIN, INIT_UPDATE, sizeof(INIT_UPDATE));
}

int dwin_send_hello()
{
    const uint8_t HELLO_COMMAND[] = {
        FRAME_HEADER,
        0x00,
        FRAME_END
    };   
    return uart_write_bytes(UART_DWIN, HELLO_COMMAND, sizeof(HELLO_COMMAND));
}

  				
int dwin_print_end()
{
    const uint8_t END[] = { FRAME_END };
	return uart_write_bytes(UART_DWIN, END, sizeof(END));
}


int dwin_hide_rectangle()
{
    const uint8_t RECTANGLE_OFF[] = {
        FRAME_HEADER, 
        COMMAND_OFF_RECTANGLE, 
        FRAME_END,
    };
    return uart_write_bytes(UART_DWIN, RECTANGLE_OFF, sizeof(RECTANGLE_OFF));
}		

int dwin_clear_screen()
{
    const uint8_t dwin_clear_screen[] = {
        FRAME_HEADER,
        COMMAND_CLEAR_SCREEN,
        FRAME_END
    };
    return uart_write_bytes(UART_DWIN, dwin_clear_screen, sizeof(dwin_clear_screen));
}				

void dwin_set_cursor(uint16_t row, uint16_t column, const uint16_t text_color, size_t font) 
{
	column = column * font * 8;
	row = row * font * 11;
    dwin_set_cursor_absolute(row, column, text_color, font);
}

void dwin_clock_get() 
{
    const uint8_t GET_TIME[] = {
        FRAME_HEADER, 
        COMMAND_GET_TIME, 
        FRAME_END
    };
    EventBits_t state_bits = xEventGroupClearBits(dwin_event_group,BIT_IS_TIME);
    do{
        uart_write_bytes(UART_DWIN, GET_TIME, sizeof(GET_TIME));
        state_bits = xEventGroupWaitBits(dwin_event_group,BIT_IS_TIME, false, false, 1000);
    }while(!(state_bits&BIT_IS_TIME));
}

void dwin_sound_buzzer(const uint8_t loud)
{
    const uint8_t full_command_buzzer[] = {
        FRAME_HEADER, 
        COMMAND_BUZZER, 
        loud, 
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, full_command_buzzer, sizeof(full_command_buzzer));
}

void dwin_set_picture(const uint8_t picture_id)
{
    if(picture_id < END_LIST_PIC){ 	
       const uint8_t SET_PIC[] = {
            FRAME_HEADER, 
            COMMAND_SET_PIC, 
            picture_id,
            FRAME_END
        };						
        uart_write_bytes(UART_DWIN, SET_PIC, sizeof(SET_PIC));
    }
} 	

void dwin_set_brightness(const uint8_t brightness)
{
    show_data_send_char(FRAME_HEADER);
    show_data_send_char(COMMAND_SET_BRIGHT);
    if(brightness > 0 && brightness < 100){
        show_data_send_char((brightness*64)/100);
    }
    dwin_print_end();
}

void dwin_set_cursor_absolute(const uint16_t row, 
                        const uint16_t column, 
                        const uint16_t text_color, 
                        const uint8_t font) 
{
    const uint8_t print_command[] = {
        FRAME_HEADER, 
        COMMAND_PRINT, 
        column/256, 
        column%256, 
        row/256, 
        row%256, 
        0x00,
        0x81,
        font,
        text_color/256,
        text_color%256,
        0xFF,
        0XFF
    };
	uart_write_bytes(UART_DWIN, print_command, sizeof(print_command));
}

void dwin_save_picture(const uint8_t pic)
{
    const uint8_t save_command[] = {
        FRAME_HEADER, 
        COMMAND_PIC_SAVE, 
        pic,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, save_command, sizeof(save_command));
}

void dwin_set_text_box(const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e)
{
    const uint8_t dwin_set_text_box[] = {
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
    uart_write_bytes(UART_DWIN, dwin_set_text_box, sizeof(dwin_set_text_box));
}

void dwin_set_color(const uint16_t foreground, const uint16_t background)
{
    const uint8_t dwin_set_color[] = {
        FRAME_HEADER,
        COMMAND_SET_COLOR,
        foreground/256,
        foreground%256,
        background/256,
        background%256,
        FRAME_END
    }; 
    uart_write_bytes(UART_DWIN, dwin_set_color, sizeof(dwin_set_color));
}

void dwin_draw_circle(  const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill  )
{
    const uint8_t CIRCULAR[] = {
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


void dwin_draw_lines( const uint16_t *points, 
                    const size_t number_point,
                    uint16_t x_start,
                    const size_t width,
                    const uint16_t y)
{
    uint16_t step = width/number_point;
    show_data_send_char(0XAA);
    show_data_send_char(0X56);
    for(uint16_t i=0, tmp=0; i<number_point; i++){
        show_data_send_char(x_start/256);
        show_data_send_char(x_start%256);
        tmp = points ? points[i] : 20;
        show_data_send_char((y - tmp )/256);
        show_data_send_char((y - tmp)%256);
        x_start += step;
    }
    dwin_print_end(); 
}


void dwin_draw_text_box(const uint16_t x, 
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
    dwin_set_color(color_rect, BLACK);
    dwin_draw_rectangle(x, y, x_e, y_e, true);
    dwin_set_cursor_absolute(y+font*2,x+font*2,color_text, font);
    show_data_send_str_dwin(str);
    dwin_print_end();
}

void dwin_draw_rectangle(const uint16_t x, 
                const uint16_t y, 
                const uint16_t x_e, 
                const uint16_t y_e, 
                const bool fill)
{
    const uint8_t RECTANGLE_ON[] = {
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
    const uint8_t clock_send[] = {
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

void dwin_fill_area(const uint16_t xs, 
                    const uint16_t ys, 
                    const uint16_t color)
{
    const uint8_t fill_area_command[] = {
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


void dwin_clock_on(const uint16_t row, 
                    const  uint16_t column, 
                    const  uint16_t textColor, 
                    const  uint8_t font)
{
    const uint8_t clock_on_command[] = {
        FRAME_HEADER,
        CLOCK_ON_COMMAND,
        font,
        textColor/256,
        textColor%256,
        column/256,
        column%256, 
        row/256,
        row%256,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, clock_on_command, sizeof(clock_on_command));
}

void dwin_clock_off(void)
{
    const uint8_t clock_off_command[] = {
        FRAME_HEADER,
        CLOCK_OFF_COMMAND,
        FRAME_END
    };
    uart_write_bytes(UART_DWIN, clock_off_command, sizeof(clock_off_command));
}
