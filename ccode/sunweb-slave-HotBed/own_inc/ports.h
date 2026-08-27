// file    : ports.h
// author  : rb
// purpose : header file for ports.c
// date    : 180505
// last    : 180710
//

#ifndef _PORTS_H_
#define _PORTS_H_

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

#endif