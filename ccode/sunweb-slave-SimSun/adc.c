// file    : adc.c
// author  : rb
// purpose : STM32F042xx ARM ADC routines
// date    : 171130
// last    : 190109
//

#include "includes.h"

//#define ADC_DEBUG

// init ADC1
void init_adc (void) 
{
  // enable clock for ADC
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

  // select ADC clock mode
  ADC1->CFGR2 = ADC_CFGR2_CKMODE_0;              // ADC clock = PCLK/2

  // enable Vrefint (needed?)
//ADC->CCR |= ADC_CCR_VREFEN;   

  // set sample time
  ADC1->SMPR = (0b011 << ADC_SMPR_SMP_Pos);      // ADC sample time =  28.5 clock cycles
//ADC1->SMPR = (0b111 << ADC_SMPR_SMP_Pos);      // ADC sample time = 239.5 clock cycles

  // set up scan sequence length & scan order
  ADC1->CR = 0;

  // set EOC flag behavior
//ADC1->CR2 |= ADC_CR2_EOCS;                     // set EOC flag after every conversion

#ifdef ADC_DEBUG
  adc_dump_registers ();
#endif
}

// read ADC channel
uint32_t adc_read_channel (uint8_t chan)
{
  // enable ADC
  ADC1->CR |= ADC_CR_ADEN;

  // set single ADC channel sequence
  ADC1->CHSELR = (1 << chan);   

  // relax a bit (skip checking flags)
//msleep (50);

  // start conversion regular channel
  ADC1->CR |= ADC_CR_ADSTART;

  // wait for conversion ready, check EOC flag (called EOCS for some reason, WTF?)
  // note: hangs with leaving stop mode (fix this)
  while ((ADC1->ISR & ADC_ISR_EOC) == 0)
    ;  

  // disable ADC
  ADC1->CR &= ~ADC_CR_ADEN;

  // read result & clear EOC flag
  return ADC1->DR;
}

// dump ADC registers -- dev only
void adc_dump_registers (void)
{
  printf ("-- ADC registers --\n");
  printf ("ADC1_SR    : %08lx\n", ADC1->ISR);
  printf ("ADC1_CR1   : %08lx\n", ADC1->IER);
  printf ("ADC1_CR2   : %08lx\n", ADC1->CR);
  printf ("ADC1_SMPR1 : %08lx\n", ADC1->CFGR1);
  printf ("ADC1_SMPR2 : %08lx\n", ADC1->CFGR2);
  printf ("ADC1_SMPR3 : %08lx\n", ADC1->SMPR);
  printf ("ADC1_SQR1  : %08lx\n", ADC1->TR);
  printf ("ADC1_SQR2  : %08lx\n", ADC1->CHSELR);
  printf ("ADC1_DR    : %08lx\n", ADC1->DR);
}

