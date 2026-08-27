// file    : comp.c
// author  : rb
// purpose : comparator routines - STM32L1xx
// date    : 210408
// last    : 220328
//
// note    : external IO PB4 is set to Analog Mode and COMP2_INP non-inverting input

#include "includes.h"

uint32_t cap_cnt;
float filt;
int cap_new = 0;

// COMP2 ISR
void COMP_IRQHandler (void)
{
  // clear pending interrupt
  EXTI->PR = EXTI_PR_PR22;                    
  
  // stop timer & capture count
  TIM5->CR1 &= ~TIM_CR1_CEN;
  cap_cnt = TIM5->CNT;

  // apply IIR filters
  filt = iir_update ((uint32_t)cap_cnt);

  led_red_off ();
}

// init comparators
void init_comp (void)
{
  // enable COMP clock
  RCC->APB1ENR |= RCC_APB1ENR_COMPEN; 

  // setup EXTI interrupt/event controller - COMP2
  EXTI->PR    =  EXTI_PR_PR22;         // clear interrupt pending bit
  EXTI->IMR  |=  EXTI_IMR_MR22;        // enable interrupt on EXTI22 line
  EXTI->EMR  &= ~EXTI_EMR_MR22;        // disable event on EXTI22 line
  EXTI->RTSR |=  EXTI_RTSR_TR22;       // enable trigger rising edge
  EXTI->FTSR &= ~EXTI_FTSR_TR22;       // disable trigger falling edge

  // set up COMP2
  COMP->CSR = COMP_CSR_INSEL_0;        // 0b001: select external IO PB3 as COMP2_INM inverting input
              
  // enable EXTI22 interrupt 
  NVIC_EnableIRQ (COMP_IRQn);
}

void cap_time (void)
{
//printf2 ("cap: %ld\n", cap_cnt);
  printf2 ("cap: %ld %4.2f\n", cap_cnt, filt);
}
