// file    : caps.h
// author  : rb
// purpose : header file for caps.c
// date    : 220304
// last    : 220717
//

#ifndef _CAPS_H_
#define _CAPS_H_

// -- defines 
#define NUM_SOIL_SENSORS                        3  // number of soil sensors

#define GPIO_CAP_SAMPLE                         5  // PB5: sampling capacitor
#define GPIO_CAP_SOIL0                          6  // PB6: soil sensor 0
#define GPIO_CAP_SOIL1                          7  // PB7: soil sensor 1
#define GPIO_CAP_SOIL2                          4  // PB4: soil sensor 2

#define TCS_START                               0
#define TCS_SAMPLE                              1

#define TCS_MAX_SAMPLE                       3000  // sampling time-out
#define TCS_ERR_SAMPLE                       9999  // error code

// -- protoypes
void init_tcs (void);

void tsc_cs_discharge (void);
void tsc_cs_float     (void);
void tsc_ct_discharge (int num);
void tsc_ct_charge    (int num);
void tsc_ct_float     (int num);

void tcs_switches (int num);

int soil_read (int sensor);

void soil_update (void);

void soil_dump (void);
void soil_dump_raw (void);
void soil_dump_ble (void);

void tsc_get_bounds (void);
void tsc_set_bounds (int num, int err);
void tsc_dump_bounds (void);

void tcs_set_min (void);
void tcs_set_max (void);

#endif


