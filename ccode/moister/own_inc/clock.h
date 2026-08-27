// file    : clock.h
// author  : rb
// purpose : header file for clock.c
// date    : 171120
// last    : 220329
//

#ifndef _CLOCK_H_
#define _CLOCK_H_

// -- defines
#define Mc         (1000000UL)              // 1 Mc

#define HSI_CLK    (16*Mc)                  // HSI Clock = 16 Mc
//#define MSI_CLK  (1024000ul)              // MSI Clock = 1.024 M
//#define MSI_CLK  (2048000ul)              // MSI Clock = 2.097 Mc  <> debug
#define MSI_CLK    (4194000ul)              // MSI Clock = 4.194 Mc  <> debug
#define SYSCLK     (HSI_CLK)                // System Clock is HSI Clock

#define APB1DIV    (1UL)                    // APB1 clock divider
#define APB1CLK    (SYSCLK/APB1DIV)         // APB1 clock

#define APB2DIV    (1UL)                    // APB2 clock divider
#define APB2CLK    (SYSCLK/APB2DIV)         // APB2 clock

#define CCLK       SYSCLK                   // normally defined in 'pll.h'

// -- prototypes
void init_clock (void);

void rcc_set_systemclock (void);
void rcc_set_rtcclock    (void);

void rcc_start_hsi (void);
void rcc_stop_hsi  (void);

#endif
