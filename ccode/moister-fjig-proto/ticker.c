// file    : ticker.c
// author  : rb
// purpose : STM32L1xx timer routines
// date    : 170906
// last    : 220329
// note    : timer 6 usleep timer & signal handler for timer 7 wall clock overflow interrupt

#include "includes.h"

// globals
volatile uchar csec_elapsed = 0;
volatile uchar dsec_elapsed = 0;
volatile uchar  sec_elapsed = 0;

// Timer 4 overflow interrupt - sample rate = 500 [samples/s]
void TIM4_IRQHandler (void)
{
  static int charge = 1;

  // clear interrupt (note: first thing to do in the ISR!!!!!)
  TIM4->SR = 0;

  if (charge)
  {
  //led_red_on ();  // note: LED off in COMP2 ISR <> debug only
    cap_charge ();
    charge = 0;
  }
  else
  {
    cap_discharge ();
    charge = 1;
  }
}

// Timer 7 overflow interrupt
void TIM7_IRQHandler (void)
{
  static uchar msec_cnt = 0;
  static uchar csec_cnt = 0;
  static uchar dsec_cnt = 0;

  // clear interrupt (note: first thing to do in the ISR!!!!!)
  TIM7->SR = 0;

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

// setup timer 6 & 7
void init_ticker (void)
{
  // enable all Timer clocks
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN |
                  RCC_APB1ENR_TIM5EN |
                  RCC_APB1ENR_TIM6EN |
                  RCC_APB1ENR_TIM7EN ;

  // init 16-bit Timer 4, freerunning at 16MHz - for cap-meter ISR
  TIM4->PSC    = 0;                    // timer_clock = SYSCLK / 1 
  TIM4->ARR    = TIMER_1KHZ;           // set period (1kHz)
  TIM4->EGR    = TIM_EGR_UG;           // force update
  TIM4->DIER   = TIM_DIER_UIE;         // enable interrupts, disable DMA
  TIM4->CR1    = TIM_CR1_CEN;          // enable timer

  // init 32-bit Timer 5, freerunning at 16MHz - for cap-meter
  TIM5->PSC    = 0;                    // timer_clock = SYSCLK / 1
  TIM5->EGR    = TIM_EGR_UG;           // force update
//TIM5->CR1    = TIM_CR1_CEN;          // enable timer

  // init 16-bit Timer 6, freerunning at 16MHz
  TIM6->PSC    = 0;                    // timer_clock = SYSCLK / 1
  TIM6->EGR    = TIM_EGR_UG;           // force update
  TIM6->CR1    = TIM_CR1_CEN;          // enable timer

  // init 16-bit Timer 7, generate interrupt every ms - wall clock
  TIM7->PSC    = 0;                    // timer_clock = SYSCLK / 1 
  TIM7->ARR    = TIMER_1KHZ;           // set period (1kHz)
  TIM7->EGR    = TIM_EGR_UG;           // force update
  TIM7->DIER   = TIM_DIER_UIE;         // enable interrupts, disable DMA
  TIM7->CR1    = TIM_CR1_CEN;          // enable timer

  // enable interrupts
//NVIC_EnableIRQ (TIM4_IRQn);          // enable interrupt Timer 4 - cap-meter
  NVIC_EnableIRQ (TIM7_IRQn);          // enable interrupt Timer 7 - wall clock
}

// kill time with us resolution
void usleep (int16_t usecs)
{
  uint16_t start = TIM6->CNT + usecs;

  while ((int16_t)(TIM6->CNT - start) < 0)
    continue;
}

// kill time with ms resolution
void msleep (uint16_t msecs)
{
  while (msecs--)
    usleep (1000); 
}






