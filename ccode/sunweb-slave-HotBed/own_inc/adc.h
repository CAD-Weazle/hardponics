// file    : adc.h
// author  : rb
// purpose : header file for adc.c
// date    : 171130
// last    : 180218
//

#ifndef _ADC_H_
#define _ADC_H_

// -- defines 
#define ADC_CHAN_VREF                            17   // ADC channel Vref_int
#define ADC_CHAN_VBAT                            18   // ADC channel Vbat

#define VREFINT_CAL_ADR  ((uint16_t *) 0x1ff800f8L)   // absolute address VREFINT calibration data
#define VREFINT_CAL_DAT          *(VREFINT_CAL_ADR)   // VREFINT calibration data
#define VREFINT_VDDA                          (3.0)   // VDDA used during VREFINT calibration 
#define ADC_FULLRANGE                      (4096.0)   // 12-bit ADC full range

// -- protoypes
void init_adc (void);

uint32_t adc_read_channel (uint8_t chan);

uint32_t adc_read_vref (void);
float    adc_read_vbat (void);

void adc_dump_registers (void);

#endif
