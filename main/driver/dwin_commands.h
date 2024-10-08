#pragma once

#define INDEX_VARIABLE_VALUE    2
#define TOUCH_CODE              0x78

/*fragments dwin commands*/
#define FRAME_HEADER            0xAA
#define FRAME_END               0xCC,0x33,0xC3,0x3C
#define COMMAND_HISTOGRAM       0x75
#define COMMAND_SET_BOX         0x45
#define COMMAND_PRINT           0x98
#define MAX_BUZZER_TIME         0xef
#define COMMAND_BUZZER          0x79
#define COMMAND_SET_SETTING     0x55,0xAA,0x5A,0xA5
#define COMMAND_SET_BRIGHT      0x5f
#define COMMAND_SET_PIC         0x70
#define COMMAND_BOX_RECTANGLE   0x59
#define COMMAND_FILL_RECTANGLE  0x5B
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
#define COMMAND_FILL                          0x73
#define HEADER_SET_CLOCK                      0xAA,0xE7,0x55,0xAA,0x5A,0xA5
#define COMMAND_GET_TIME                      0x9B,0x5A
#define CLOCK_ON_COMMAND                      0x9B,0xFF,0x00
#define CLOCK_OFF_COMMAND                     0x9B,0x00
