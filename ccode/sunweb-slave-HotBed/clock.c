// file    : clock.c
// author  : rb
// purpose : STM32F042xx RCC module routines 
// date    : 180609
// last    : 180609
//

#include "includes.h"

// init all clocks
void init_clock (void)
{
  rcc_stop_pll  ();          // make sure PLL is stopped before calling 'rcc_set_systemclock ()'
  rcc_start_hsi ();          // start internal RC oscillator (8 Mc)
  rcc_start_lsi ();          // start internal RC oscillator (40kc)
  rcc_set_systemclock ();
}

// set system clock
void rcc_set_systemclock (void)
{
  RCC->CFGR = 0; // <> dev only

  // set MCO, system clocks and bus dividers 
  RCC->CFGR = RCC_CFGR_MCOPRE_DIV1     |         // MCO divided by 1
//            RCC_CFGR_MCOSEL_SYSCLK   |         // SYSCLK on MCO         
              RCC_CFGR_MCOSEL_HSI      |         // HSI on MCO (8 Mc)
//			      RCC_CFGR_MCOSEL_MSI      |         // MSI on MCO
//            RCC_CFGR_MCOSEL_HSE      |         // HSE on MCO
//            RCC_CFGR_MCOSEL_PLL_DIV2 |         // PLL on MCO
              RCC_CFGR_PPRE_DIV1       |         // PCLK clock divided by 1 (APB)
              RCC_CFGR_HPRE_DIV1       |         // HCLK clock divided by 1 (AHB)
              RCC_CFGR_SW_HSI;                   // HSI used for SYSCLK 
              
//printf ("RCC->CR  : 0x%08x\n", RCC->CR);
//printf ("RCC->CFGR: 0x%08x\n", RCC->CFGR); 
}

// start lo-power 40kc LSI clock
void rcc_start_lsi (void)
{
  // turn LSI oscillator on
  RCC->CSR |= RCC_CSR_LSION;

  // wait for LSI stable
  while ((RCC->CSR & RCC_CSR_LSIRDY) == 0)
    ;                   
}

// stop lo-power 40kc LSI clock
void rcc_stop_lsi (void)
{
  // turn LSI oscillator off
  RCC->CSR &= ~RCC_CSR_LSION;
}

// start 8Mc HSI clock
void rcc_start_hsi (void)
{
  // turn HSI oscillator on
  RCC->CR |= RCC_CR_HSION;

  // wait for HSI stable
  while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    ;                   
}

// stop 8Mc HSI clock
void rcc_stop_hsi (void)
{
  // turn HSI oscillator off
  RCC->CR &= ~RCC_CR_HSION;
}

// start HSE clock
void rcc_start_hse (void)
{
  // turn HSE oscillator on
  RCC->CR |= RCC_CR_HSEON;

  // wait for HSE stable
  while ((RCC->CR & RCC_CR_HSERDY) == 0)
    ;                   
}

// start PLL 
void rcc_start_pll (void)
{
  // turn PLL on
  RCC->CR |= RCC_CR_PLLON;

  // wait for PLL locked
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    ;                   

//printf ("RCC->CR  : 0x%08x, PLL started\n", RCC->CR);
}

// stop PLL 
void rcc_stop_pll (void)
{
  // turn PLL off
  RCC->CR &= ~RCC_CR_PLLON;

  // wait for PLL stopped
  while ((RCC->CR & RCC_CR_PLLRDY) == 1)
    ;                   

//printf ("RCC->CR  : 0x%08x, PLL stopped\n", RCC->CR);
}

