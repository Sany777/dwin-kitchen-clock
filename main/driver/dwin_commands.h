#pragma once

#include "dwin_config.h"
/*for commands : buzzer, set pic, set brightnes*/
#define INDEX_VARIABLE_VALUE    2
#define TOUCH_CODE              0x78
#define LEN_FRAME               2

/*fragments dwin commands*/
#define FRAME_HEADER            0xAA
#define FRAME_END               0xCC,0x33,0xC3,0x3C
#define COMMAND_SET_BOX         0x45
#define COMMAND_PRINT           0x98
#define COMMAND_GET_TIME        0x9B,0x5A
#define MAX_BUZZER_TIME         0xef
#define COMMAND_BUZZER          0x79
#define COMMAND_SET_SETTING     0x55,0xAA,0x5A,0xA5
#define COMMAND_SET_CLOCK       0xE7,COMMAND_SET_SETTING
#define COMMAND_SET_BRIGHT      0x5f
#define COMMAND_SET_PIC         0x70
#define COMMAND_CLOCK_ON        0x9b,0xFF,0x00
#define COMMAND_CLOCK_OFF       0x9b,0x00
#define COMMAND_ON_RECTANGLE    0x59
#define COMMAND_OFF_RECTANGLE   0x69
#define COMMAND_FILL_AREA       0x64
#define COMMAND_PIC_SAVE        0xE2
#define COMMAND_ANIMATION       0x9A
#define COMMAND_SET_COLOR       0x40
#define COMMAND_LINE_DISPLAY_FOREGROUND       0x56
#define COMMAND_LINE_DISPLAY_BACKGROUND       0x5D
#define COMMAND_CIRCULAR                      0x57
#define COMMAND_CIRCULAR_FOREGROUND           0x01
#define COMMAND_CIRCULAR_FILL_FOREGROUND      0x03
#define COMMAND_BROKEN_LINE                   0x76
#define COMMAND_CLEAR_SCREEN                  0x52
#define COMMAND_INIT_UPDATE                   0xf2,0xf2,0xf2,0x5A,0xA5,0x1E
#define COMMAND_WORK_IN_VIDEOBUF              0x72

/****************************** FULL DWIN COMMANDS **********************************/

static const char CLEAR_SCREEN[] = {
    FRAME_HEADER,
    COMMAND_CLEAR_SCREEN,
    FRAME_END
};

static char SEND_IMG[] = {
    FRAME_HEADER,
    COMMAND_WORK_IN_VIDEOBUF,
    VARIABLE_VALUE, //H Addr
    VARIABLE_VALUE,
    VARIABLE_VALUE, //M Addr
    VARIABLE_VALUE,
    VARIABLE_VALUE, //L Addr
    VARIABLE_VALUE,
};

static char INIT_UPDATE[] = {
    FRAME_HEADER,
    COMMAND_INIT_UPDATE,
    FRAME_END,
};


static char CIRCULAR[] = {
    FRAME_HEADER,
    VARIABLE_VALUE, // mode
    VARIABLE_VALUE, // x centre
    VARIABLE_VALUE,
    VARIABLE_VALUE, // y centre
    VARIABLE_VALUE,
    VARIABLE_VALUE, // radius 
    VARIABLE_VALUE,
    FRAME_END
};

static char SET_COLOR[] = {
    FRAME_HEADER,
    COMMAND_SET_COLOR,
    VARIABLE_VALUE, //foreground
    VARIABLE_VALUE,
    VARIABLE_VALUE, //background
    VARIABLE_VALUE,
    FRAME_END
}; 

static const char HELLO_COMMAND[] = {
    FRAME_HEADER,
    0x00,
    FRAME_END
};         

static char ANIMATION[] = {
    FRAME_HEADER, 
    COMMAND_ANIMATION, 
    VARIABLE_VALUE, //pic id
    FRAME_END,
};

static const char ANIMATION_OFF[] = {
    FRAME_HEADER, 
    COMMAND_ANIMATION, 
    0xff,
    FRAME_END,
};

static char SAVE_PIC[] = {
    FRAME_HEADER, 
    COMMAND_PIC_SAVE, 
    VARIABLE_VALUE, //pic id
    FRAME_END,
};

static const char RECTANGLE_OFF[] = {
    FRAME_HEADER, 
    COMMAND_OFF_RECTANGLE, 
    FRAME_END,
};

static char RECTANGLE_ON[] = {
    FRAME_HEADER, 
    COMMAND_ON_RECTANGLE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    FRAME_END,
};

static char PRINT[] = {
    FRAME_HEADER, 
    COMMAND_PRINT, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    VARIABLE_VALUE, 
    0x00,
    0x80,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    0xFF,
    0XFF,
    FRAME_END,
};

static char CLOCK_ON[] = {
    FRAME_HEADER, 
    COMMAND_CLOCK_ON, 
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    VARIABLE_VALUE,
    FRAME_END
};

static char SET_PIC[] = {
    FRAME_HEADER, 
    COMMAND_SET_PIC, 
    VARIABLE_VALUE,
    FRAME_END
};

static const char HEADER_CLOCK_SET[]= {
    FRAME_HEADER, 
    COMMAND_SET_CLOCK, 
};

static char SET_BRIGHT[] = {
    FRAME_HEADER, 
    COMMAND_SET_BRIGHT, 
    VARIABLE_VALUE,
    FRAME_END
};

static const char CLOCK_OFF[] = {
    FRAME_HEADER, 
    COMMAND_CLOCK_OFF, 
    FRAME_END
};

static char FULL_COMMAND_BUZZER[] = {
    FRAME_HEADER, 
    COMMAND_BUZZER, 
    VARIABLE_VALUE, 
    FRAME_END
};

static const char GET_TIME[] = {
    FRAME_HEADER, 
    COMMAND_GET_TIME, 
    FRAME_END
};

static char SET_TEXT_BOX[] = {
    FRAME_HEADER, 
    COMMAND_SET_BOX,
    VARIABLE_VALUE, //x start
    VARIABLE_VALUE,
    VARIABLE_VALUE, //y start
    VARIABLE_VALUE,
    VARIABLE_VALUE, //x end
    VARIABLE_VALUE,
    VARIABLE_VALUE, //y end
    VARIABLE_VALUE,
    FRAME_END
};

static const char CANCEL_TEXT_BOX[] = {
    FRAME_HEADER, 
    0x00,
    FRAME_END
};

static const char END[] = { FRAME_END };

enum index_byte_clock_on {
    CLOCK_FONT = 4,
    CLOCK_COLOUR_1,
    CLOCK_COLOUR_2,
    CLOCK_COL_1,
    CLOCK_COL_2,
    CLOCK_ROW_1,
    CLOCK_ROW_2
};

enum index_byte_print {
    PRINT_COL_1=2,
    PRINT_COL_2,
    PRINT_ROW_1,
    PRINT_ROW_2,
    PRINT_FONT=8,
    PRINT_COLOUR_1,
    PRINT_COLOUR_2,
};
