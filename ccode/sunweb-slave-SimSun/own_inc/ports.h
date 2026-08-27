// file    : ports.h
// author  : rb
// purpose : header file for ports.c
// date    : 180505
// last    : 190109
//

#ifndef _PORTS_H_
#define _PORTS_H_

// -- defines, taken from STTM32L151 for backward compatibility 
#define  GPIO_MODE_IN        (0x00000000u)       // GPIO set to input
#define  GPIO_MODE_OUT       (0x00000001u)       // GPIO set to output
#define  GPIO_MODE_AF        (0x00000002u)       // GPIO set to alternate function
#define  GPIO_MODE_AN        (0x00000003u)       // GPIO set to analog mode

#define GPIO_OTYPE_PP        (0x00000000u)       // output push-pull
#define GPIO_OTYPE_OD        (0x00000001u)       // output open-drain

// -- prototypes
void init_ports (void);

void led_red_on     (void);
void led_red_off    (void);
void led_red_toggle (void);
void led_red_flash  (uint16_t delay);

void led_grn_on     (void);
void led_grn_off    (void);
void led_grn_toggle (void);
void led_grn_flash  (uint16_t delay);

void rs485_dir (uint16_t dir);

void led_fred_on  (void);
void led_fred_off (void);

#endif