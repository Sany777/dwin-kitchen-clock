#include "show_data.h"


int send_with_eof(const char *data_to_send)
{
    for(size_t i = 0; i<MAX_DWIN_DATA; i++){
        if(i && data_to_send[i] == myEOF){
    		return uart_write_bytes(UART_DWIN, data_to_send, i);
		}
	}
	return 0;
}

void send_number(int number) 
{
    char send_number[SIZE_BUF_INT];
    size_t it=0;
    if(number < 0) {
        send_number[it++]='-';
        number *=- 1;
    }    
    int denominator = 10;
    for(int number_temp=number; number_temp; number_temp/=10, denominator*=10);
    for(; denominator; ) {
        send_number[it++] = (char)number/denominator + '0';
        number %= denominator;
        denominator/=10;
    }
    uart_write_bytes(UART_DWIN, send_number, it);
}

void send_without_zero(const uint8_t number) 
{
    char message[SIZE_BUF_UINT8];
	size_t it = 0;
    if((number%100)/10) {
        message[it++] = (number%100)/10;
    }
	message[it++] = number%10;
	uart_write_bytes(UART_DWIN, message, it);
}

#define SIZE_CLOCK 7

void send_clock(const uint8_t hour, const uint8_t min) 
{
    char message[SIZE_CLOCK] = { VARIABLE_VALUE,VARIABLE_VALUE,' ',':',' ',VARIABLE_VALUE,VARIABLE_VALUE};
    message[0] = hour%100 > 9 ? hour/10+'0' : ' ';
    message[1] = hour%10+'0';
    message[5] = min/10+'0';
    message[6] = min%10+'0';
    uart_write_bytes(UART_DWIN, message, SIZE_CLOCK);
}

void send_with_zero(const uint8_t number) 
{
    char message[SIZE_BUF_UINT8];
    message[0] = (number%100)/10 + '0';
    message[1] = number%10 + '0';
    uart_write_bytes(UART_DWIN, message, SIZE_BUF_UINT8);
}

#define SIZE_BUF_FLOAT 6

void send_temperature(temperature_t int_as_float) 
{
    if(int_as_float < 1000 && int_as_float > -1000) {
        char str_float[SIZE_BUF_FLOAT]; 
        size_t it=0;
        if(int_as_float < 0) {
            str_float[it++] = '-';
            int_as_float *= -1;
        }
        if(int_as_float > 100) {
            str_float[it++] = int_as_float/100+'0';
            int_as_float %= 100;
        }
        str_float[it++] = int_as_float/10 + '0';
        if(int_as_float%10 ) {
            str_float[it++] = '.';
            str_float[it++] = int_as_float%10 + '0';
        }
        uart_write_bytes(UART_DWIN, str_float, it);
    }
}


/* SPECIAL SYMBOLS DWIN SCREEN*/ 
#define CORNER_TOP_LEFT     	1
#define CORNER_TOP_RIGHT    	2
#define CORNER_BOTTOM_LEFT    	3
#define CORNER_BOTTOM_RIGHT   	4
#define LINE_VERTICAL       	5
#define LINE_HORISONTAL     	6

#define MAX_NUMBER_DIGIT     	6
#define MAX_LINES_IN_FRAME   	5

void send_in_frame(uint8_t row, 
							const uint8_t column, 
							const uint8_t font, 
							const uint16_t color, 
							const char *message)
{
	uint8_t length_max_piece = 0, lines = 0, end_print, start_print;
	uint8_t length_pieces[MAX_LINES_IN_FRAME];
	char tmp;
	for (uint8_t length_cur = 0, i = 0;; length_cur++, i++)	{
		tmp = message[i];
		if (tmp == ' ' || tmp== '\t' || tmp == '\r' || tmp == '\n') {
			length_pieces[lines++] = length_cur;
			if (lines >= MAX_LINES_IN_FRAME || tmp == myEOF)
				break;
			if (length_max_piece < length_cur)
				length_max_piece = length_cur;
			length_cur = 0;
		}
	}
	/*frame - 2 sides*/
	lines += 2;
	for (uint8_t pos_char = 0, line_str = 0, line = 0; line < lines; line++) {
		dwin_print(row++, column, color, font);
		if (line == 0 || line == lines - 1)	{
			if (line == 0)
				send_char(CORNER_TOP_LEFT);
			else
				send_char(CORNER_BOTTOM_LEFT);
			for (uint8_t i = 0; i < length_max_piece; i++) {
				send_char(LINE_HORISONTAL);
			}
			if (line == 0)
				send_char(CORNER_TOP_RIGHT);
			else
				send_char(CORNER_BOTTOM_RIGHT);
		}
		else {
			send_char(LINE_VERTICAL);
			end_print = length_max_piece/2+length_pieces[line_str]/2;
			start_print = length_max_piece/2-length_pieces[line_str]/2;
			for (uint8_t i=0; i<length_max_piece; i++) {
				if (i >= start_print && i <= end_print \
					&& message[pos_char] != '\n' && message[pos_char] != myEOF) {
						send_char(message[pos_char++]);
				} else {
					send_char(' ');
				}	
			}
			pos_char++;
			line_str++;
			send_char(LINE_VERTICAL);
		}
		print_end();
	}
}

#define SIZE_MAC_STR 17

void print_mac(const uint8_t *mac) 
{
	uint8_t tmp;
	int str_count=0; 
	char str_out[SIZE_MAC_STR];
    for(int mac_count=0; mac_count<SIZE_MAC; ) {
        tmp = mac[mac_count++]&0xf0;
		if(tmp) {
			str_out[str_count++] = 'A' + tmp;
		} else {
			str_out[str_count++] = tmp+'0';
		}
		tmp = mac[mac_count]&0x0f;
		if(tmp) {
			str_out[str_count++] = 'A' + tmp;
		} else {
			str_out[str_count++] = tmp+'0';
		}
		str_out[str_count++] = '.';
    }
	uart_write_bytes(UART_DWIN, str_out, SIZE_MAC_STR);
}





