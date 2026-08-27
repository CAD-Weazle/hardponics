// file    : esp32.h
// author  : rb
// purpose : header file for esp32.c
// date    : 211124
// last    : 230225
//

#ifndef _ESP32_H_
#define _ESP32_H_

// -- defines 
#define ESP_INBUF_LEN                         128  // UART buffer size
                                              
#define ESP_WATCHDOG                          120  // watchdog time-out

// -- protoypes
void init_esp32 (void);

void esp32_update (void);

void esp32_reset (void);

void esp32_wd_reset (void);
void esp32_wd_test  (void);

void esp32_stop (int time);
void esp32_run  (void);

#endif


