// file    : leds.h
// author  : rb
// purpose : header file leds.h
// date    : 180805
// last    : 190109

#ifndef __LEDS_H__
#define __LEDS_H__

// -- defines
#define LED_OFF        0
#define LED_FULL     100

// -- prototypes
void init_leds (void);

void leds_dred (uint8_t intensity);
void leds_rblu (uint8_t intensity);
void leds_fred (uint8_t intensity);

#endif
