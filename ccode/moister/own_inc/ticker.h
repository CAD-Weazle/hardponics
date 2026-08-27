// file    : ticker.h
// author  : rb
// purpose : header file ticker.h
// date    : 170906
// last    : 220530

#ifndef __TICKER_H__
#define __TICKER_H__

//#include "types.h"

// -- globals
volatile extern uint8_t csec_elapsed;
volatile extern uint8_t dsec_elapsed;
volatile extern uint8_t sec_elapsed;

// -- defines 
#define TIMER_1HZ    (CCLK/1    - 1)
#define TIMER_1KHZ   (CCLK/1000 - 1)
#define TIMER_1MHZ   (CCLK/1000000ul - 1)

// -- prototypes
//void TIMER0_IRQHandler (void);
//void RIT_IRQHandler    (void);

void init_ticker (void);

void usleep (int16_t usecs);
void msleep (uint16_t msecs);

#endif
