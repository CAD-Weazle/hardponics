// file    : pll.h
// author  : rb
// purpose : header file for pll.c
// date    : 171120
// last    : 210318
//

#ifndef _PLL_H_
#define _PLL_H_

// -- defines
#define Mc         (1000000UL)              // 1 Mc

#define HSI_CLK    (16*Mc)                  // HSI clock = 16 Mc
#define MSI_CLK    (1024000UL)              // MSI clock = 1.024 Mc

#define PLL_MUL    4                        // PLL multiplier 
//#define PLL_DIV    2                      // PLL divider - org 32Mc clock
#define PLL_DIV    4                        // PLL divider - test 16 Mc clock
//#define PLL_CLK    (HSI_CLK * (4/2))      // PLL clock = 32 Mc - org
#define PLL_CLK    (HSI_CLK * (PLL_MUL/PLL_DIV)) // PLL clock = 16 Mc - test

#define SYSCLK     (PLL_CLK)                // system clock

#define APB1DIV    (1UL)                    // APB1 clock divider
#define APB1CLK    (SYSCLK/APB1DIV)         // APB1 clock

#define APB2DIV    (1UL)                    // APB2 clock divider
#define APB2CLK    (SYSCLK/APB2DIV)         // APB2 clock

// -- prototypes
void init_pll (void);

#endif
