// file    : ports.h
// author  : rb
// purpose : header file for ports.c
// date    : 170810
// last    : 230203
//

#ifndef _PORTS_H_
#define _PORTS_H_

// GPIO output set/clear macros
#define gpio_set(port, pin)       do {port->BSRR = (0x00000001 << pin);} while(0) // set outputs: lo-word BSRR
#define gpio_clr(port, pin)       do {port->BSRR = (0x00010000 << pin);} while(0) // clr outputs: hi-word BSRR

// GPIO port mode defines
#define GPIO_MODE_IN                (0x00000000u)  // GPIO set to input
#define GPIO_MODE_OUT               (0x00000001u)  // GPIO set to output
#define GPIO_MODE_AF                (0x00000002u)  // GPIO set to alternate function
#define GPIO_MODE_AN                (0x00000003u)  // GPIO set to analog mode

#define GPIO_OTYPE_PP               (0x00000000u)  // output push-pull
#define GPIO_OTYPE_OD               (0x00000001u)  // output open-drain

// port A
#define SHT_DAT                                 8  // SHT15 data (open drain)
#define SHT_CLK                                 9  // SHT15 clock
#define SHT_PWR_L                              10  // SHT15 power enable
#define OLED_CS_L                              11  // OLED CS line
#define OLED_CD                                12  // OLED command/data line
#define REF_EC                                 15  // EC reference enable

// port B
#define VGND                                    2  // VBAT & VSOL input dividers GND connection
#define PUMP1                                   3  // water pump 1
#define PUMP0                                   8  // water pump 0

// port C
#define IBAT_SIGN                              13  // sign bit IBAT
#define WIFI_EN                                15  // WIFI modeule enable

// port H
#define LED_RED                                 0
#define LED_GRN                                 1

// -- prototypes
void init_ports (void);

void ports_stop (void);

void led_red_on     (void);
void led_red_off    (void);
void led_red_toggle (void);
void led_red_flash  (uint16_t delay);

void led_grn_on     (void);
void led_grn_off    (void);
void led_grn_toggle (void);
void led_grn_flash  (uint16_t delay);

void sht15_power_on  (void);
void sht15_power_off (void);

void ec_power_on  (void);
void ec_power_off (void);

void pump0_on  (void);
void pump0_off (void);
void pump1_on  (void);
void pump1_off (void);

void gnd_connect (void);
void gnd_disconnect (void);

#endif



