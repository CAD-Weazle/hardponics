// file    : ws2815.c
// author  : rb/ao
// purpose : WS2815 'Neopixel' LED driver
// date    : 201130
// last    : 221122

#include "includes.h"

// bit patterns for WS2812/WS2815 serial LEDs (4 data bits per symbol)
static const uint16_t ws[16] = 
{ 
  0x8888, 0x888e, 0x88e8, 0x88ee,
  0x8e88, 0x8e8e, 0x8ee8, 0x8eee,
  0xe888, 0xe88e, 0xe8e8, 0xe8ee,
  0xee88, 0xee8e, 0xeee8, 0xeeee,
};

// globals
uint32_t image_buf[WS2815_NUMLEDS];   // image buffer

// init WS2815 
void init_ws2815 (void)
{
  // all LEDs off
  for (int i = 0; i < WS2815_NUMLEDS; i++)
    image_buf[i] = WS2815_COL_BLK;
}

// update image buffer 
void ws2815_update (void)
{
  // calculate new light pattern
  ws2815_barberpole (WS2815_ALPHA_25);

  // update LED string
  ws2815_do ();
}

// update WS2815 LED string
void ws2815_do (void)
{
  // update all LEDs
  for (int i = 0; i < WS2815_NUMLEDS; i++)
    ws2815_pixel (image_buf[i]);

  // latch in data
//ws2815_reset ();
}

// apply WS2815 RESET signal
void ws2815_reset (void)
{
  // not needed
}

// write WS2815 pixel - MOSI SPI2 
void ws2815_pixel (uint32_t col)
{
  // re-order bit pattern from RGB to GRB (for WS2815)
//uint32_t dat = ((col<<8) & 0xff0000) |
//               ((col>>8) & 0x00ff00) |
//               ((col<<0) & 0x0000ff);

  // re-ordering not needed (????)
  uint32_t dat = col;

  // write 24-bit color data to WS2815 DI pin via SPI MOSI 
  for (int i = 20; i >= 0; i -= 4)
    spi2_write_word (ws[(dat >> i) & 0xf]);
}

// display touch button state
void ws2815_touchstate (int *but)
{
  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
  {
    if (but[i])
      image_buf[i] = WS2815_COL_RED & WS2815_ALPHA_25;
    else
      image_buf[i] = WS2815_COL_BLK;
  }

  // update LEDs
  ws2815_do ();
}

// display 'barber pole' pattern <> test only
void ws2815_barberpole (uint32_t alfa)
{
  static uint16_t cnt = 0;

  for (int i = 0; i < WS2815_NUMLEDS; i++)
  {
    if (((i + cnt) & 0b11111) < 12)
      image_buf[i] = WS2815_COL_RED & alfa;
    else
      image_buf[i] = WS2815_COL_GRN & alfa;
  }

  cnt++;
}
