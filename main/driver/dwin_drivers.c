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
    char SET_TEXT_BOX[] = {
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
    char SET_COLOR[] = {
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
    char CIRCULAR[] = {
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
    const uint16_t step = width/number_point;
    send_char(0XAA);
    send_char(0X56);
    for(uint16_t i=0, tmp=0; i<number_point; i++){
        send_char(x_start/256);
        send_char(x_start%256);
        send_char((y - points[i])/256);
        send_char((y - points[i])%256);
        x_start += step;
    }
    print_end(); 
}

void print_histogram( uint16_t *y_points, 
                        const size_t points_number,
                        const uint16_t x_start,
                        const size_t width,
                        const uint16_t y)
{
    size_t w = 10;

    uint16_t cur, next, count, x = x_start;
    cur = y_points[0]*2;
    next = y_points[1]*2;
    if(cur > next){
        count = w/(cur-next)-1;
    } else {
        count = w/(next - cur)-1;
    }
    
    for( int ii = 0, i = 0, iii = 0; i-1<points_number; ii++, iii++){
        if(ii == 0){
            send_char(0XAA);
            send_char(0x75);       
            send_char((x)/256);
            send_char((x)%256);
            send_char((y)/256);
            send_char((y)%256);
            send_char(0xFF);
        }
            x++;

        send_char(0);
        send_char(cur);
        if(cur != next){
        //    if(iii > count){
            cur > next ? cur-- : cur++;
            iii = 0;
        //    }
        }
        if(ii > w){
            i++;
            ii = 0;
            cur = y_points[i]*2;
            next = y_points[i+1]*2;
            if(cur > next){
                count = w/(cur-next)-1;
            } else {
                count = w/(next - cur)-1;
            }
            print_end(); 
        }
        vTaskDelay(10);
    }
}


void print_rect(const uint16_t x_s, const uint16_t y_s, const uint16_t x_e, const uint16_t y_e, bool fill )
{
    char RECTANGLE_ON[] = {
        FRAME_HEADER, 
        fill
            ? COMMAND_FILL_RECTANGLE
            : COMMAND_BOX_RECTANGLE, 
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

void fill_area(const uint16_t x_s, const uint16_t y_s, const uint16_t color)
{
    send_char(0XAA);
    send_char(0x73);
    uint16_t xs= x_s;
    send_char(xs/256);
    send_char(xs%256);
    uint16_t ys= y_s;
    send_char(ys/256);
    send_char(ys/256);;
    send_char(color/256);
    send_char(color/256);
    print_end(); 
}