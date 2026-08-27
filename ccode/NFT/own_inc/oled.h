// file    : oled.h
// author  : rb
// purpose : header file for oled.c
// date    : 210904
// last    : 230126
//

#ifndef _OLED_H_
#define _OLED_H_

// -- defines
#define OLED_WIDTH                            128  //
#define OLED_HEIGHT                           128  //

#define OLED_CMD_SETCOLUMN                   0x15
#define OLED_CMD_SETROW                      0x75
#define OLED_CMD_WRITERAM                    0x5c
#define OLED_CMD_READRAM                     0x5d
#define OLED_CMD_SETREMAP                    0xa0
#define OLED_CMD_STARTLINE                   0xa1
#define OLED_CMD_DISPLAYOFFSET 	             0xa2
#define OLED_CMD_DISPLAYALLOFF 	             0xa4
#define OLED_CMD_DISPLAYALLON  	             0xa5
#define OLED_CMD_NORMALDISPLAY 	             0xa6
#define OLED_CMD_INVERTDISPLAY 	             0xa7
#define OLED_CMD_FUNCTIONSELECT              0xab
#define OLED_CMD_DISPLAYOFF                  0xae
#define OLED_CMD_DISPLAYON                   0xaf
#define OLED_CMD_PRECHARGE                   0xb1
#define OLED_CMD_DISPLAYENHANCE              0xb2
#define OLED_CMD_CLOCKDIV                    0xb3
#define OLED_CMD_SETVSL                      0xb4
#define OLED_CMD_SETGPIO 		             0xb5
#define OLED_CMD_PRECHARGE2                  0xb6
#define OLED_CMD_SETGRAY                     0xb8
#define OLED_CMD_USELUT                      0xb9
#define OLED_CMD_PRECHARGELEVEL              0xbb
#define OLED_CMD_VCOMH                       0xbe
#define OLED_CMD_CONTRASTABC                 0xc1
#define OLED_CMD_CONTRASTMASTER              0xc7
#define OLED_CMD_MUXRATIO                    0xca
#define OLED_CMD_COMMANDLOCK                 0xfd
#define OLED_CMD_HORIZSCROLL                 0x96
#define OLED_CMD_STOPSCROLL                  0x9e
#define OLED_CMD_STARTSCROLL                 0x9f

#define OLED_COL_BLACK                     0x0000
#define OLED_COL_RED                       0xf800  // 5-bit
#define OLED_COL_GREEN                     0x07e0  // 6-bit
#define OLED_COL_BLUE                      0x001f  // 5-bit
#define OLED_COL_CYAN                      0x07ff
#define OLED_COL_MAGENTA                   0xf81f
#define OLED_COL_YELLOW                    0xffe0
#define OLED_COL_WHITE                     0xffff
#define OLED_COL_OLDLACE                   0xe6d7
#define OLED_COL_GRAY                      0x4208
#define OLED_COL_GOLD                      0xbcc4

// -- prototypes
void oled_select   (void);
void oled_deselect (void);

void oled_cmd (void);
void oled_dat (void);

void init_oled (void); 

void oled_wr_cmd (uint8_t dat);
void oled_wr_dat (uint8_t dat);

void oled_wr_col (uint16_t col);

void oled_update (void);

void oled_power_down (void);
void oled_power_up   (void);

void oled_brightness (uint8_t dat) ;

void oled_clrscr (uint16_t col);  

void oled_rect (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t col);  

void oled_putc (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t fg, uint16_t bg, uint8_t *font);  
void oled_puts (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t fg, uint16_t bg, char *str);

void oled_dump_font (void);

void oled_display_background (void);

void oled_display_data (float v_bat, float i_bat, float v_sol, float par, float temp, float rh);

void oled_align (char *str, char *res);

#endif

