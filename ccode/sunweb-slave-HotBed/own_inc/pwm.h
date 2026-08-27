// file    : pwm.h
// author  : rb
// purpose : header file pwm.h
// date    : 180805
// last    : 181220

#ifndef __PWM_H__
#define __PWM_H__

// -- defines
#define PWM_RESET_LEDS      0     // default bottom LEDs intensity after RESET
#define PWM_RESET_HEATER    0     // default heater drive after RESET
#define PWM_RESET_FAN       0     // default fan speed after RESET

// -- prototypes
void init_pwm (void);

void pwm_set_heater  (uint8_t intensity);
void pwm_dump_heater (uint8_t cr_nl);

void    pwm_set_leds  (uint8_t intensity);
uint8_t pwm_get_leds  (void);
void    pwm_dump_leds (uint8_t cr_nl);

void    pwm_set_fan   (uint8_t intensity);
uint8_t pwm_get_fan   (void);
void    pwm_dump_fan  (uint8_t cr_nl);

void pwm_state (void);

void pwm_leds_tick  (void);
void pwm_dump_table (void);

void pwm_show_start (void);
void pwm_show_stop  (void);

#endif
