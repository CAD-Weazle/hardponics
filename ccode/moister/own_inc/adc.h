// file    : adc.h
// author  : rb
// purpose : header file for adc.c
// date    : 171130
// last    : 220707
//

#ifndef _ADC_H_
#define _ADC_H_

extern float ntc_fil[4];                 // median filtered NTC data

// -- defines 
#define ADC_FILTER_DEPTH                        8  // average filter depth
#define ADC_MEDIAN_DEPTH                        7  // median filter depth
#define ADC_NUM_NTC                             4  // number of NTC sensors

#define ADC_NUM_REFSAMPLES                     16  // number of samples to average Vrefint     <> todo
#define ADC_NUM_RCHANNELS (ADC_MEDIAN_DEPTH*ADC_NUM_NTC) // ADC scan chain length
#define ADC_NUM_JCHANNELS                       3  // number of ADC regular channels in use    <> todo

#define ADC_CHAN_NTC0                           4  // ADC channel NTC0
#define ADC_CHAN_NTC1                           5  // ADC channel NTC1
#define ADC_CHAN_NTC2                           6  // ADC channel NTC2
#define ADC_CHAN_NTC3                           7  // ADC channel NTC3
#define ADC_CHAN_VREF                          17  // ADC channel Vref_int
#define ADC_CHAN_VBAT                          19  // ADC channel Vbat
#define ADC_CHAN_VSOL                          20  // ADC channel Vsolar

#define VREFINT_CAL_ADR ((uint16_t *) 0x1ff800f8L) // absolute address VREFINT calibration data
#define VREFINT_CAL_DAT        *(VREFINT_CAL_ADR)  // VREFINT calibration data
#define VREFINT_VDDA                        (3.0)  // VDDA used during VREFINT calibration 
#define ADC_FULLRANGE                    (4095.0)  // 12-bit ADC full range

// -- protoypes
void init_adc (void);

void adc_update (void);

void adc_read_supply (void);
void adc_read_ntc    (void);
void adc_read_ntc_new (void);

uint32_t adc_read_channel (uint8_t chan);

void vrefint_enable  (void);
void vrefint_disable (void);

void adc_dump (void);
void adc_dump_ble (void);

void adc_dump_registers (void);

float adc_median (uint32_t *buf);

#endif














