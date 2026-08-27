// file    : ticker.c
// author  : rb
// purpose : STM32L1xx timer routines
// date    : 170906
// last    : 230124
// note    : timer 6 usleep timer & signal handler for timer 7 wall clock overflow interrupt

#include "includes.h"

// globals
volatile uchar csec_elapsed = 0;
volatile uchar dsec_elapsed = 0;
volatile uchar  sec_elapsed = 0;

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

// setup Timers
void init_ticker (void)
{
  // enable clock for Timers
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN 
               |  RCC_APB1ENR_TIM3EN 
               |  RCC_APB1ENR_TIM6EN 
               |  RCC_APB1ENR_TIM7EN;
  
  RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
  
//  // init 16-bit Timer 2 for PWM output - PUMP1
//  TIM2->PSC    = TIMER_100KHZ;              // set timer_clock 
//  TIM2->ARR    = PUMP_PWM_PERIOD-1;         // set period (== 10 ms / 100 Hz)
//  TIM2->CCR2   = PWM_OFF;                   // pump off
//  TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 |         // select PWM mode 1 on OC2 (OC2M = 0b110)
//                 TIM_CCMR1_OC2M_1 |
//                 TIM_CCMR1_OC2PE;           // enable preload register on OC1, channel is output
//  TIM2->CCER  |= TIM_CCER_CC2E;             // select active high polarity on OC2 & enable output OC2
////TIM2->BDTR  |= TIM_BDTR_MOE;              // enable output (this timer has no BDTR register)
//  TIM2->CR1   |= TIM_CR1_CEN;               // enable counter, edge allign mode & direction upcounting
//  TIM2->EGR   |= TIM_EGR_UG;                // force update

//  // init 16-bit Timer 3 for PWM output - PUMP0
//  TIM3->PSC    = TIMER_100KHZ;              // set timer_clock 
//  TIM3->ARR    = PUMP_PWM_PERIOD-1;         // set period (== 10 ms / 100 Hz)
//  TIM3->CCR3   = PWM_OFF;                   // pump off
//  TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 |         // select PWM mode 1 on OC3 (OC3M = 0b110)
//                 TIM_CCMR2_OC3M_1 |
//                 TIM_CCMR2_OC3PE;           // enable preload register on OC3, channel is output
//  TIM3->CCER  |= TIM_CCER_CC3E;             // select active high polarity on OC3 & enable output OC3
////TIM3->BDTR  |= TIM_BDTR_MOE;              // enable output (this timer has no BDTR register)
//  TIM3->CR1   |= TIM_CR1_CEN;               // enable counter, edge allign mode & direction upcounting
//  TIM3->EGR   |= TIM_EGR_UG;                // force update

  // init 16-bit Timer 6, freerunning at 1 MHz - 'usleep ()' 
  TIM6->PSC    = TIMER_1MHZ;                // timer_clock = SYSCLK / 32 (== 1 Mc)
  TIM6->EGR    = TIM_EGR_UG;                // force update
  TIM6->CR1    = TIM_CR1_CEN;               // enable timer

  // init 16-bit Timer 7, generate interrupt every ms - wall clock
  TIM7->PSC    = TIMER_1MHZ;                // timer_clock = SYSCLK / 32 (== 1 Mc)
  TIM7->ARR    = 1000;                      // set period
  TIM7->EGR    = TIM_EGR_UG;                // force update
  TIM7->DIER   = TIM_DIER_UIE;              // enable interrupts, disable DMA
  TIM7->CR1    = TIM_CR1_CEN;               // enable timer
  NVIC_EnableIRQ (TIM7_IRQn);               // enable interrupt Timer 7

  // init 32-bit Timer 9 - ADC trigger via TRGO
  TIM9->PSC    = TIMER_1MHZ;                // timer_clock = SYSCLK / 32 (== 1 Mc)
  TIM9->ARR    = (1000 - 1);                // sample rate (== 1000 samples/s)
//TIM9->ARR    = (500 - 1);                 // sample rate (== 2000 samples/s)
  TIM9->CR2    = TIM_CR2_MMS_1;             // update event as TRGO trigger ouput
//TIM9->CR1    = TIM_CR1_CEN;               // enable timer
}

// kill time with 1 us resolution
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

// dump Timer registers -- dev only
void timer_dump_registers (void)
{
  printf ("-- Timer4 registers --\n");
  printf ("TIM4->PSC  : %08lx\n", TIM4->PSC);
  printf ("TIM4->ARR  : %08lx\n", TIM4->ARR);
  printf ("TIM4->EGR  : %08lx\n", TIM4->EGR);
  printf ("TIM4->CR1  : %08lx\n", TIM4->CR1);
  printf ("TIM4->CR2  : %08lx\n", TIM4->CR2);  
  printf ("TIM4->SMCR : %08lx\n", TIM4->SMCR);  
  printf ("TIM4->CCMR1: %08lx\n", TIM4->CCMR1);  
  printf ("TIM4->CCMR2: %08lx\n", TIM4->CCMR2);
  printf ("TIM4->CCER : %08lx\n", TIM4->CCER);
		
/*      
  printf ("-- Timer9 registers --\n");
  printf ("TIM9->PSC  : %08lx\n", TIM9->PSC);
  printf ("TIM9->ARR  : %08lx\n", TIM9->ARR);
  printf ("TIM9->EGR  : %08lx\n", TIM9->EGR);
  printf ("TIM9->CR1  : %08lx\n", TIM9->CR1);
  printf ("TIM9->CR2  : %08lx\n", TIM9->CR2);  */
}
