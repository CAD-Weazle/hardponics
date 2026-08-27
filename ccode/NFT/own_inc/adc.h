// file    : adc.h
// author  : rb
// purpose : header file for adc.c
// date    : 171130
// last    : 230315
//

#ifndef _ADC_H_
#define _ADC_H_

// -- defines 
#define ADC_NUM_REFSAMPLES                     16  // number of samples to average Vrefint
#define ADC_EXP_DEPTH                          64  // average exponential filter depth
#define ADC_EXP_DEPTH_PAR                     256  // average exponential filter depth

#define ADC_CHAN_PAR                            0  // PAR light sensor
#define ADC_CHAN_EC                             1  // EC conductivity sensor
#define ADC_CHAN_NTC                            6  // NTC temperature sensor of EC sensor
#define ADC_CHAN_VBAT                           8  // battery voltage
#define ADC_CHAN_IBAT                           9  // battery current
#define ADC_CHAN_VREF                          17  // ADC channel Vrefint
#define ADC_CHAN_VSOL                          20  // solar panel voltage

#define VREFINT_CAL_ADR ((uint16_t *) 0x1ff800f8U) // absolute address VREFINT calibration data - STM151L151C 
//#define VREFINT_CAL_ADR ((uint16_t *) 0x1ff80078U) // absolute address VREFINT calibration data - STM151L151B
#define VREFINT_CAL_DAT        *(VREFINT_CAL_ADR)  // VREFINT calibration data
#define VREFINT_VDDA                        (3.0)  // VDDA used during VREFINT calibration 
#define ADC_FULLRANGE                    (4095.0)  // 12-bit ADC full range

// PAR: 4.57uV/umol/m^2.s / amplifier x400 -> 1.828mV/umol/m^2.s (real OPAMP gain ~405)
#define PAR_FACTOR                         (4.57)  // Kipp & Zonen PAR sensor calibration factor [uV/umol/m^2.s]
#define PAR_LSB                          (0.4343)  // after calibration with 5mVDC input voltage
#define PAR_AMP_GAIN                      (405.0)  // two-stage chopper OPAMP gain (not used)

// -- protoypes
void init_adc (void);

uint32_t adc_read_channel (uint8_t chan);

float adc_read_vref (void);

void adc_update (void);

float adc_read_vbat (void);
float adc_read_ibat (void);
float adc_read_vsol (void);

uint16_t adc_read_par (void);
uint16_t adc_read_ntc (void);

void adc_dump_registers (void);

#endif

