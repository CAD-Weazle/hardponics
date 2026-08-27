// file    : ticker.c
// author  : rb
// purpose : ticker & PWM Timers
// date    : 170906
// last    : 180818

#include "includes.h"

// globals
volatile uint8_t csec_elapsed = 0;
volatile uint8_t dsec_elapsed = 0;
volatile uint8_t sec_elapsed  = 0;

// Timer 1 overflow interrupt
void TIM1_BRK_UP_TRG_COM_IRQHandler (void)
{
  static uint8_t msec_cnt = 0;
  static uint8_t csec_cnt = 0;
  static uint8_t dsec_cnt = 0;

  // clear interrupt (note: first thing to do in the ISR!!!!!)
  TIM1->SR = 0;

  msec_cnt++;

  if (msec_cnt >= 10)
  {
    csec_elapsed = 1;

    msec_cnt = 0;
    csec_cnt++;
  }

  if (csec_cnt == 10)
  {
    dsec_elapsed = 1;

    csec_cnt = 0;
    dsec_cnt++;
  }

  if (dsec_cnt == 10)
  {
    sec_elapsed = 1;
    dsec_cnt = 0;
  }
}        

// set up Timers
void init_ticker (void)
{
  // enable clock for Timer 1, Timer 16 & Timer 17
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN  |
                  RCC_APB2ENR_TIM16EN |
                  RCC_APB2ENR_TIM17EN;

  // enable clock for Timer 2, Timer 3 & Timer 14
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN  |
                  RCC_APB1ENR_TIM3EN  |
                  RCC_APB1ENR_TIM14EN;

  // init 16-bit Timer 1, generate interrupt every ms
  TIM1->PSC    = 0;                              // timer_clock = SYSCLK / 1 (== 8 Mc)
  TIM1->EGR    = TIM_EGR_UG;                     // force update
  TIM1->ARR    = TIMER_1KHZ;                     // time interrupts
  TIM1->DIER   = TIM_DIER_UIE;                   // enable update interrupt, disable DMA & other interrupts
  TIM1->CR1    = TIM_CR1_CEN;                    // enable timer
  NVIC_EnableIRQ (TIM1_BRK_UP_TRG_COM_IRQn);     // enable Break, Update, Trigger & Commutation Interrupt Timer 1

  // init 16-bit Timer 3, free running for usleep
  TIM3->PSC    = 7;                              // timer_clock = SYSCLK / 8 (== 1 Mc)
  TIM3->EGR    = TIM_EGR_UG;                     // force update
  TIM3->CR1    = TIM_CR1_CEN;                    // enable timer

  // init 32-bit Timer 2 for PWM output - heater
//TIM2->PSC    = 7;                              // timer_clock = SYSCLK / 8 (== 1 Mc)
  TIM2->PSC    = 799;                            // <> test timer_clock = SYSCLK / 800 (== 10 kc)
//TIM2->PSC    = 7999;                           // <> test timer_clock = SYSCLK / 8000 (== 1 kc)
  TIM2->ARR    = PWM_PERIOD;                     // set period (== 100 us / 10 kHz)
  TIM2->CCR2   = PWM_OFF;                        // set high period to zero
  TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 |              // select PWM mode 1 on OC2 (OC2M = 0b110)
                 TIM_CCMR1_OC2M_1 |
                 TIM_CCMR1_OC2PE;                // enable preload register on OC1, channel is output
  TIM2->CCER  |= TIM_CCER_CC2E;                  // select active high polarity on OC2 & enable output OC2
//TIM2->BDTR  |= TIM_BDTR_MOE;                   // enable output (this timer has no BDTR register)
  TIM2->CR1   |= TIM_CR1_CEN;                    // enable counter, edge allign mode & direction upcounting
  TIM2->EGR   |= TIM_EGR_UG;                     // force update

  // init 16-bit Timer 14 for PWM output - fan
  TIM14->PSC    = 7;                             // timer_clock = SYSCLK / 8 (== 1 Mc)
  TIM14->ARR    = PWM_PERIOD;                    // set period (== 100 us / 10 kHz)
  TIM14->CCR1   = PWM_OFF;                       // set high period to zero
  TIM14->CCMR1 |= TIM_CCMR1_OC1M_2 |             // select PWM mode 1 on OC1 (OC1M = 0b110)
                  TIM_CCMR1_OC1M_1 |
                  TIM_CCMR1_OC1PE;               // enable preload register on OC1, channel is output
  TIM14->CCER  |= TIM_CCER_CC1E;                 // select active high polarity on OC1 & enable output OC1
//TIM14->BDTR  |= TIM_BDTR_MOE;                  // enable output (this timer has no BDTR register)
  TIM14->CR1   |= TIM_CR1_CEN;                   // enable counter, edge allign mode & direction upcounting
  TIM14->EGR   |= TIM_EGR_UG;                    // force update

  // init 16-bit Timer 16 for PWM output - bottom LEDs
  TIM16->PSC    = 7;                             // timer_clock = SYSCLK / 8 (== 1 Mc)
  TIM16->ARR    = PWM_PERIOD;                    // set period (== 100 us / 10 kHz)
  TIM16->CCR1   = PWM_OFF;                       // set high period to zero
  TIM16->CCMR1 |= TIM_CCMR1_OC1M_2 |             // select PWM mode 1 on OC1 (OC1M = 0b110)
                  TIM_CCMR1_OC1M_1 |
                  TIM_CCMR1_OC1PE;               // enable preload register on OC1, channel is output
  TIM16->CCER  |= TIM_CCER_CC1E;                 // select active high polarity on OC1 & enable output OC1
  TIM16->BDTR  |= TIM_BDTR_MOE;                  // enable output
  TIM16->CR1   |= TIM_CR1_CEN;                   // enable counter, edge allign mode & direction upcounting
  TIM16->EGR   |= TIM_EGR_UG;                    // force update

  // init 16-bit Timer 17 for PWM output - not used
  TIM17->PSC    = 7;                             // timer_clock = SYSCLK / 8 (== 1 Mc)
  TIM17->ARR    = PWM_PERIOD;                    // set period (== 100 us / 10 kHz)
  TIM17->CCR1   = PWM_OFF;                       // set high period to zero
  TIM17->CCMR1 |= TIM_CCMR1_OC1M_2 |             // select PWM mode 1 on OC1 (OC1M = 0b110)
                  TIM_CCMR1_OC1M_1 |
                  TIM_CCMR1_OC1PE;               // enable preload register on OC1, channel is output
  TIM17->CCER  |= TIM_CCER_CC1E;                 // select active high polarity on OC1 & enable output OC1
  TIM17->BDTR  |= TIM_BDTR_MOE;                  // enable output
//TIM17->CR1   |= TIM_CR1_CEN;                   // enable counter, edge allign mode & direction upcounting
  TIM17->EGR   |= TIM_EGR_UG;                    // force update
}

// kill time with us resolution
void usleep (int16_t usecs)
{
  uint16_t start = TIM3->CNT + usecs;

  while ((int16_t) (TIM3->CNT - start) < 0)
    continue;
}

// kill time with ms resolution
void msleep (uint16_t msecs)
{
  while (msecs--)
    usleep (1000);              
}

// PWM routines for bottom LEDs, heater & fan (all same PWM frequency)
void pwm_set (uint16_t channel, uint8_t intensity)
{
  uint16_t hi_time;

  // calculate PWM hi-time from 0..100% intensity  
  if (intensity)
    hi_time = (((PWM_MAX - PWM_MIN) * intensity) / 100) + PWM_MIN;
  else
    hi_time = 0;

  switch (channel)
  {
    // heater
    case PWM_HEATER:
    {
      if (hi_time <= TIM2->ARR+1)
        TIM2->CCR2 = hi_time;
      break;
    }

    // fan
    case PWM_FAN:
    {
      if (hi_time <= TIM14->ARR+1)
        TIM14->CCR1 = hi_time;
      break;
    }

    // bottom green LEDs
    case PWM_LEDS:
    {
      if (hi_time <= TIM16->ARR+1)
        TIM16->CCR1 = hi_time;
      break;
    }

    default:
     break;
  }
}






