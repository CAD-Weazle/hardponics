// file    : ticker.h
// author  : rb
// purpose : header file ticker.h
// date    : 170906
// last    : 230111

#ifndef __TICKER_H__
#define __TICKER_H__

#include "types.h"

// -- globals
volatile extern uchar csec_elapsed;
volatile extern uchar dsec_elapsed;
volatile extern uchar sec_elapsed;

// -- defines 
#define TIMER_1HZ       (SYSCLK/1    - 1)   // Timer clock = 1Hz
#define TIMER_1KHZ      (SYSCLK/1000 - 1)   // Timer clock = 1kHz
#define TIMER_10KHZ    (SYSCLK/10000 - 1)   // Timer clock = 10kHz
#define TIMER_100KHZ  (SYSCLK/100000 - 1)   // Timer clock = 100kHz
#define TIMER_1MHZ   (SYSCLK/1000000 - 1)   // Timer clock = 1MHz
#define TIMER_SYSCLK                  (0)   // Timer clock = SYSCLK

// -- prototypes
//void TIMER0_IRQHandler (void);
//void RIT_IRQHandler    (void);

void init_ticker (void);

void usleep (int16_t usecs);
void msleep (uint16_t msecs);

void timer_dump_registers (void);

void pump_pwm (int pump, int pwm);

void pump_update (void);
#endif
