// file    : ticker.h
// author  : rb
// purpose : header file ticker.h
// date    : 170906
// last    : 181018

#ifndef __TICKER_H__
#define __TICKER_H__

// -- globals
volatile extern uint8_t csec_elapsed;
volatile extern uint8_t dsec_elapsed;
volatile extern uint8_t sec_elapsed;

// -- defines 
#define TIMER_1HZ    (SYSCLK/1UL       - 1)
#define TIMER_1KHZ   (SYSCLK/1000UL    - 1)
#define TIMER_1MHZ   (SYSCLK/1000000UL - 1)

#define PWM_PERIOD   99      // PWM period (== 100 us -> 10 kHz)
#define PWM_OFF       0      // PWM off
#define PWM_MIN       1      // minimum PWM
#define PWM_MAX     100      // maximum PWM

#define PWM_HEATER    0
#define PWM_FAN       1
#define PWM_LEDS      2

// -- prototypes
void TIM1_BRK_UP_TRG_COM_IRQHandler (void);

void init_ticker (void);

void usleep (int16_t usecs);
void msleep (uint16_t msecs);

void pwm_set (uint16_t channel, uint8_t intensity);

#endif
