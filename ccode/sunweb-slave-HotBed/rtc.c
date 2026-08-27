// file    : rtc.c
// author  : rb
// purpose : STM32F042xx RTC routines
// date    : 171120
// last    : 180827
//
// Note    : The LSEON, LSEBYP, RTCSEL and RTCEN bits of the RTC domain control 
//           register (RCC_BDCR) are in the RTC domain. As a result, after RESET, 
//           these bits are write-protected and the DBP bit in the Power control 
//           register (PWR_CR) has to be set before these can be modified. Refer 
//           to Section 5.1.3: Battery backup domain for further information. 
//           These bits are only reset after a RTC domain reset (see Section 6.1.3: 
//           RTC domain reset). Any internal or external RESET will not have any 
//           effect on these bits.

#include "includes.h"

//#define RTC_DEBUG 

uint8_t do_rtc = 0;          // wallclock flag

// struct holding time 
struct
{
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t hrs;
  uint32_t min;
  uint32_t sec;
  uint32_t num_sec;
} rtc_time;

// init RTC
void init_rtc (void)
{
  // enable PWR peripheral, so PWR_CR register can be accessed
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  // enable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR |= PWR_CR_DBP;
  (void) PWR->CR;                                // dummy read, make sure write action completed

  // select RTC clock & enable RTC 
  RCC->BDCR |= RCC_BDCR_RTCEN |                  // enable RTC
               (0b10 << RCC_BDCR_RTCSEL_Pos);    // select LSI as RTC clock

  // disable RTC registers write protection
  RTC->WPR = 0xca;
  RTC->WPR = 0x53;

  // select 1Hz calibration output
//RTC->CR |= RTC_CR_COSEL;

  // enable calibration output
//RTC->CR |= RTC_CR_COE;

  // enter init state, allow initializing RTC_TR, RTC_DR & RTC_PRER
  rtc_enter_initmode ();

  // update prescaler
//RTC->PRER = 0x007f0137;                        // 40kHz/128 = 312 Hz, 312Hz/312 = 1Hz
//RTC->PRER = 0x007f0129;                        // manually tweaked to pas
//RTC->PRER = 0x007f0000 | 291;                  // manually tweaked to pas 4:00/3:53 (ARM/Tab)
//RTC->PRER = 0x007f0000 | 300;                  // manually tweaked to pas 4:00/4:01 (ARM/Tab)

  // ok for eval board
//RTC->PRER = 0x007f0000 | 299;                  // manually tweaked to pas

  // SimSun CONTROL
  RTC->PRER = 0x007f0000 | 321;                  // manually tweaked to pas

  // update time 
//RTC->TR = RTC_TR_PM | 0;                       // start at zero, AM/PM-hour format
  RTC->TR = 0;                                   // start at zero, 24-hour format
  RTC->DR = 0;                                   // start at zero

  // leave init phase
  rtc_leave_initmode ();

  // wait for calibration register write allowed
  while((RTC->ISR & RTC_ISR_RECALPF) == RTC_ISR_RECALPF) 
  {
  }

  // set calibration to around +20ppm, which is a standard value @25°C 
//RTC->CALR = RTC_CALR_CALP | 482;
  RTC->CALR = 113;        // -108ppm for SimSun CONTROL (113 * 0.954ppm)

  // enable RTC registers write protection
  RTC->WPR = 0xff;                     // 'wrong' code enables write protect

  // disable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR &= ~PWR_CR_DBP;

#ifdef RTC_DEBUG
  // dump current state
  rtc_dump_registers ();
#endif
}

// set RTC time registers
void rtc_set_time (uint8_t hrs, uint8_t min, uint8_t sec)
{
  // input data sanity check, bail if not ok
  if ((hrs >= 24) || (min >= 60) || (sec >= 60))
    return;

  // enable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR |= PWR_CR_DBP;
  (void) PWR->CR;                 

  // disable RTC registers write protection
  RTC->WPR = 0xca;
  RTC->WPR = 0x53;

  // enter init state, allow initializing RTC_TR, RTC_DR & RTC_PRER
  rtc_enter_initmode ();
 
  // set time registers, use 24-hour format
  RTC->TR = ((hrs/10) << 20) | ((hrs%10) << 16) |
            ((min/10) << 12) | ((min%10) <<  8) |
            ((sec/10) <<  4) | ((sec%10) <<  0); 

  // leave init phase
  rtc_leave_initmode ();

  // enable RTC registers write protection
  RTC->WPR = 0xff;  

  // disable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR &= ~PWR_CR_DBP;
}

// set RTC time registers - hex mode, for SunWeb data
void rtc_set_time_bcd (uint8_t hrs, uint8_t min, uint8_t sec)
{
  // input data sanity check, bail if not ok
  if ((hrs >= 0x24) || (min >= 0x60) || (sec >= 0x60))
    return;

  // enable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR |= PWR_CR_DBP;
  (void) PWR->CR;                 

  // disable RTC registers write protection
  RTC->WPR = 0xca;
  RTC->WPR = 0x53;

  // enter init state, allow initializing RTC_TR, RTC_DR & RTC_PRER
  rtc_enter_initmode ();
 
  // set time registers, use 24-hour format
  RTC->TR = ((hrs << 16) | (min << 8) | (sec << 0));

  // leave init phase
  rtc_leave_initmode ();

  // enable RTC registers write protection
  RTC->WPR = 0xff;  

  // disable access to RTC, RTC Backup & RCC CSR registers
  PWR->CR &= ~PWR_CR_DBP;
}

// get current time
void rtc_update_time (void)
{
  // dummy read registers
  (void)RTC->TR;
  (void)RTC->DR;

  // extract time
  rtc_time.hrs = ((RTC->TR & 0x00300000) >> 20)*10 + ((RTC->TR & 0x000f0000) >> 16);
  rtc_time.min = ((RTC->TR & 0x00007000) >> 12)*10 + ((RTC->TR & 0x00000f00) >>  8);
  rtc_time.sec = ((RTC->TR & 0x00000070) >>  4)*10 +  (RTC->TR & 0x0000000f);

  // extract date
  // <> todo

  // calculated elapsed seconds today
  rtc_convert_time ();
}

// convert time to seconds
uint32_t rtc_convert_time (void)
{
  return (rtc_time.num_sec = (rtc_time.hrs*3600) + (rtc_time.min*60) + rtc_time.sec);
}

// dump current time to UART
void rtc_dump_time (void)
{
  // bail when inactive
  if (!do_rtc)
    return;

  // get current time
  rtc_update_time ();

  printf2 ("%02d:%02d:%02d - %d\n", rtc_time.hrs, rtc_time.min, rtc_time.sec, rtc_time.num_sec);
//printf2 ("%02d:%02d:%02d - ", rtc_time.hrs, rtc_time.min, rtc_time.sec);
}

// enter RTC init mode: counters stopped & writes to RTC_TR/RTC_DR/RTC_PRER allowad
void rtc_enter_initmode (void)
{
  uint32_t wait = RTC_INIT_TIMEOUT;

  // set RTC to init mode
  RTC->ISR |= RTC_ISR_INIT;

  // wait for RTC init mode set or timeout
  while (!(RTC->ISR & RTC_ISR_INITF) && --wait)
    usleep (10);

#ifdef RTC_DEBUG
  // flag timeout
  if (!wait)
    printf2 ("rtc_enter_initmode: timeout occurred\n");
#endif
}

// leave RTC init mode: counters running & register access not allowed
void rtc_leave_initmode (void)
{
  uint32_t wait = RTC_INIT_TIMEOUT;

  // clear RTC init mode
  RTC->ISR &= ~RTC_ISR_INIT;

  // wait for RTC init mode set or timeout
  while ((RTC->ISR & RTC_ISR_INITF) && --wait)
    usleep (10);
  
#ifdef RTC_DEBUG
  // flag timeout
  if (!wait)
    printf2 ("rtc_leave_initmode: timeout occurred\n");
#endif
}

// start wallclock, dump to serialport
void rtc_wallclock_start (void)
{
  do_rtc = 1;
}

// stop wallclock
void rtc_wallclock_stop (void)
{
  do_rtc = 0;
}

// dump some RTC registers <> dev only
void rtc_dump_registers (void)
{
  printf2 ("RTC registers:\n");
  printf2 ("RTC_TR       : %08lx\n", RTC->TR);
  printf2 ("RTC_DR       : %08lx\n", RTC->DR);
  printf2 ("RTC_CR       : %08lx\n", RTC->CR);
  printf2 ("RTC_ISR      : %08lx\n", RTC->ISR);
  printf2 ("RTC_PRER     : %08lx\n", RTC->PRER);
  printf2 ("RTC_ALRMAR   : %08lx\n", RTC->ALRMAR);
  printf2 ("RTC_WPR      : %08lx\n", RTC->WPR);
  printf2 ("RTC_SSR      : %08lx\n", RTC->SSR);
  printf2 ("RTC_SHIFTR   : %08lx\n", RTC->SHIFTR);
  printf2 ("RTC_TSTR     : %08lx\n", RTC->TSTR);
  printf2 ("RTC_TSDR     : %08lx\n", RTC->TSDR);
  printf2 ("RTC_TSSSR    : %08lx\n", RTC->TSSSR);
  printf2 ("RTC_CALR     : %08lx\n", RTC->CALR);
  printf2 ("RTC_TAFCR    : %08lx\n", RTC->TAFCR);
  printf2 ("RTC_ALRMASSR : %08lx\n", RTC->ALRMASSR);
  printf2 ("RTC_BKP0R    : %08lx\n", RTC->BKP0R);
  printf2 ("RTC_BKP1R    : %08lx\n", RTC->BKP1R);
  printf2 ("RTC_BKP2R    : %08lx\n", RTC->BKP2R);
  printf2 ("RTC_BKP3R    : %08lx\n", RTC->BKP3R);
  printf2 ("RTC_BKP4R    : %08lx\n", RTC->BKP4R);
  printf2 ("\n");
  printf2 ("PWR_CR       : %08lx\n", PWR->CR);
}



///////////////////////////////////
/* obselete ??/??
// set RTC prescaler
void rtc_set_prescaler (uint32_t val)
{
  // enter initialization mode
  rtc_enter_initmode ();

  // set prescaler, use two write
  RTC->PRER  = val & 0x00007fff;       // 1st write: syn  = 256
  RTC->PRER |= val & 0x007f0000;       // 2nd write: asyn = 128

  // leave initialization mode
  rtc_leave_initmode ();
}
*/
