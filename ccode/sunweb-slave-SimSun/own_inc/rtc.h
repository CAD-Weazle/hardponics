// file    : rtc.h
// author  : rb
// purpose : header file for rtc.c
// date    : 171120
// last    : 190109
//

#ifndef _RTC_H_
#define _RTC_H_

// -- defines
#define RTC_INIT_TIMEOUT       1000
#define RTC_WKUP_EXTI     (1 << 20)

#define RTC_WAKEUP_INTERVAL      10

// -- globals
extern uint8_t do_sample;
extern uint8_t do_rtc;

// -- prototypes
void init_rtc (void);

void rtc_set_time     (uint8_t hrs, uint8_t min, uint8_t sec);
void rtc_set_time_bcd (uint8_t hrs, uint8_t min, uint8_t sec);

void     rtc_update_time  (void);
uint32_t rtc_convert_time (void);

void rtc_log_start    (void);
void rtc_log_stop     (void);
void rtc_log          (void);
void rtc_dump_time    (void);

void rtc_set_prescaler   (uint32_t val);
void rtc_set_wakeuptimer (uint32_t val);
void rtc_set_wakeupint   (void);

void rtc_enter_initmode (void);
void rtc_leave_initmode (void);

void rtc_dump_registers (void);


#endif


