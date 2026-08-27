// file    : pll.c
// author  : rb
// purpose : STM32L1xx RCC module routines 
// date    : 171125
// last    : 220203
//
// note    : max. SYSCLK  = 32Mc
//           max. APB1CLK = 32Mc
//           max. APB2CLK = 32Mc

#include "includes.h"

// init PLL and set bus dividers
void init_pll (void)
{
  // set MCO and bus dividers
  RCC->CFGR = RCC_CFGR_MCOPRE_DIV4    |          // MCO divided by 4
            //RCC_CFGR_MCO_HSI        |          // HSI on MCO
            //RCC_CFGR_MCO_PLL        |          // PLL on MCO
              RCC_CFGR_MCOSEL_SYSCLK  |          // SYSCLK on MCO
              RCC_CFGR_PPRE2_DIV1     |          // APB2 clock divided by 1
              RCC_CFGR_PPRE1_DIV1;               // APB1 clock divided by 1

  // enable HSI, used as clock source of PLL
  RCC->CR |= RCC_CR_HSION;

  // wait for HSI stable
  while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    ;                   

/* disable PLL to save power, run on HSI clock
  // configure PLL
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSI |              // clock PLL with HSI
               RCC_CFGR_PLLMUL4    |              // PLL_MUL - x4
             //RCC_CFGR_PLLDIV2;                  // PLL_DIV - /2
               RCC_CFGR_PLLDIV4;                  // PLL_DIV - /4 test

  // enable PLL
  RCC->CR |= RCC_CR_PLLON;

  // & wait for PLL stable
  while( !(RCC->CR & RCC_CR_PLLRDY) )
    ;
*/
  // set flash wait states
  FLASH->ACR  = FLASH_ACR_ACC64;                // ACC64 must be set first
  FLASH->ACR |= FLASH_ACR_LATENCY;              // set FLASH latency to 1 wait state
//FLASH->ACR &= ~FLASH_ACR_ACC64;               // reset ACC64 <> needed?

  // select PLL as system clock - org
//RCC->CFGR |= RCC_CFGR_SW_PLL;

  // select HSI as system clock - test
  RCC->CFGR |= RCC_CFGR_SW_HSI;
}





