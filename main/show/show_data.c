#include "show_data.h"

char *buf_send_operation;

void send_str(const char * format, ... )
{
  va_list args;
  va_start (args, format);
  vsnprintf (buf_send_operation, MAX_DATA_LEN, format, args);
  va_end (args);
  uart_write_bytes(UART_DWIN, buf_send_operation, strlen(buf_send_operation));
}


void show_mac(uint8_t *mac, char *str)
{
    if(mac){
        ESP_LOGI(TAG, "Station "MACSTR" %s",
                 MAC2STR(mac), str);
    } else {
        ESP_LOGI(TAG, "%s", str);
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
	if(!message)return;
	uint8_t length_max_piece = 0, lines = 0, end_print, start_print;
	uint8_t length_pieces[MAX_LINES_IN_FRAME];
	char tmp;
	for (uint8_t length_cur = 0, i = 0; ; length_cur++, i++)	{
		tmp = message[i];
		if (tmp== '\t' || tmp == 0 || tmp == '\n') {
			length_pieces[lines++] = length_cur;
			if (length_max_piece < length_cur)
				length_max_piece = length_cur;
			
			if (lines >= MAX_LINES_IN_FRAME || tmp == 0)
				break;
			length_cur = 0;
		}

	}
	/*frame - 2 sides*/
	lines += 2;
	for (uint8_t pos_char = 0, line_str = 0, line = 0; line < lines; line++) {
		print_start(row++, column, color, font);
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
					&& message[pos_char] != '\n' && message[pos_char]) {
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





