// file    : ws2815.h
// author  : rb
// purpose : header file for ws2815.c
// date    : 211123
// last    : 221122

#ifndef __WS2815_H__
#define __WS2815_H__

#define WS2815_NUMLEDS                            4   // number of LEDs in string (small string)

#define WS2815_COL_BLK                     0x000000   // color black
#define WS2815_COL_WHT                     0xffffff   // color white
#define WS2815_COL_RED                     0xff0000   // color red
#define WS2815_COL_GRN                     0x00ff00   // color green
#define WS2815_COL_BLU                     0x0000ff   // color blue
#define WS2815_COL_OLDLACE                 0xfdf5e6   // color Old Lace
#define WS2815_COL_PEACH                   0x996f2c   // color Peach
#define WS2815_COL_TEAL                    0x184436   // color Teal

#define WS2815_ALPHA_25                    0x3f3f3f   // 25% brightness
#define WS2815_ALPHA_50                    0x7f7f7f   // 50% brightness

// -- prototypes
void init_ws2815 (void);

void ws2815_update (void);
void ws2815_do     (void);

void ws2815_pixel (uint32_t col);
void ws2815_reset (void);

void ws2815_touchstate (int button[4]); // 4 == NUM_BUTTONS from 'touch.h'

void ws2815_barberpole   (uint32_t alfa);

#endif

