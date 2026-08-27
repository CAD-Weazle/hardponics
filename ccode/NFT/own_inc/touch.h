// file    : touch.h
// author  : rb
// purpose : header file for touch.c
// date    : 220304
// last    : 230212
//

#ifndef _TOUCH_H_
#define _TOUCH_H_

// -- defines 
#define TOUCH_NUM_BUTTONS                       4  // number of touch buttons
#define TOUCH_DELTA                            20  // drop in touch value to signal button press

#define TOUCH_BUTTON_OLED                       3  // touch button for OLED on/off control
#define TOUCH_BUTTON_STATUS                     2  // touch button (wifi?) status dump <> button flaky, has saw tooth (WTF?)
#define TOUCH_BUTTON_PUMP0                      1  // touch button water pump 0
#define TOUCH_BUTTON_PUMP1                      0  // touch button water pump 1

#define GPIO_CAP_SAMPLE1                        4  // PB4 : sampling capacitor
#define GPIO_CAP_BUTTON0                        5  // PB5 : touch button 0
#define GPIO_CAP_BUTTON1                        6  // PB6 : touch button 1
#define GPIO_CAP_BUTTON2                        7  // PB7 : touch button 2

#define GPIO_CAP_SAMPLE2                       12  // PB12: sampling capacitor
#define GPIO_CAP_BUTTON3                       13  // PB13: touch button 3

#define TCS_START                               0  // touch buttons read-out FSM
#define TCS_SAMPLE                              1  // touch buttons read-out FSM

#define TCS_MAX_SAMPLE                      12000  // sampling time-out
#define TCS_ERR_SAMPLE                       9999  // error code

// -- protoypes
void init_touch (void);

void touch_update (void);

int touch_read (int button);

void touch_dump (void);

void tsc_cs_discharge (int button);
void tsc_cs_float     (int button);
void tsc_ct_discharge (int button);
void tsc_ct_charge    (int button);
void tsc_ct_float     (int button);

void tcs_switches (int button);

void touch_scan (void);

int touch_oled_pressed  (void);
int touch_pump0_pressed (void);
int touch_pump1_pressed (void);

void touch_enable  (void);
void touch_disable (void);

#endif







