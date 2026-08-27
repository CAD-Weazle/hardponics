// file    : ticker.h
// author  : rb
// purpose : header file ticker.h
// date    : 170906
// last    : 180805

#ifndef __TICKER_H__
#define __TICKER_H__

//#include "types.h"

// -- globals
volatile extern uint8_t csec_elapsed;
volatile extern uint8_t dsec_elapsed;
volatile extern uint8_t sec_elapsed;

// -- defines 
#define TIMER_1HZ    (SYSCLK/1UL       - 1)
#define TIMER_1KHZ   (SYSCLK/1000UL    - 1)
#define TIMER_1MHZ   (SYSCLK/1000000UL - 1)

#define PWM_PERIOD   99      // PWM period (== 100 us / 10 kHz)
#define PWM_OFF      0       // V_CTRL = 0V0, LEDs off
#define PWM_MIN      10      // V_CTRL = 0V3, LED intensity minimal
#define PWM_MAX      76      // V_CTRL = 2V5, LED intensity maximal

#define PWM_DRED0    0
#define PWM_DRED1    1
#define PWM_DRED2    2
#define PWM_FRED     3  // note: has no PWM timer, bit-bang this
#define PWM_RBLU     4

// -- prototypes
void TIM1_BRK_UP_TRG_COM_IRQHandler (void);

void init_ticker (void);

void usleep (int16_t usecs);
void msleep (uint16_t msecs);

void pwm_set (uint16_t channel, uint8_t intensity);

#endif
