// file    : dac.h
// author  : rb
// purpose : header file for dac.c
// date    : 160312
// last    : 221122
//

#ifndef _DAC_H_
#define _DAC_H_

extern float Vset;

// -- defines
#define DAC_MIN                            0x0000  // 12-bit DAC
#define DAC_MAX                            0x0fff  // 12-bit DAC

// -- prototypes
void init_dac (void);

void dac1_write (uint16_t dat);
void dac2_write (uint16_t dat);

#endif
