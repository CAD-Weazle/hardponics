// file    : rtd.h
// author  : rb
// purpose : header file for rtd.h
// date    : 161204
// last    : 181030
//

#ifndef __RTD_H__
#define __RTD_H__

// -- defines
#define SENSOR_BED           0         // hot bed Pt100 sensors, averaged
#define SENSOR_PT1           1         // hot bed Pt100 sensors, Pt100 sensor 1
#define SENSOR_PT2           2         // hot bed Pt100 sensors, Pt100 sensor 2
#define SENSOR_AMB           3         // external Pt100 sensor, ambient temperature

#define PT_NUM               3         // number of Pt100 sensors

#define PT_SENSOR1           0
#define PT_SENSOR2           1
#define PT_SENSOR3           2

#define FILTER               3         // filter cut-off            
#define PT_FILTER (1 << FILTER)        // ADC filter coefficient (power of 2)

#define MIN_TEMP           0.0         // minimum allowed valid temperature, signal error when lower
#define MAX_TEMP          50.0         // maximum allowed valid temperature, signal error when higher


// -- prototypes
void init_rtd (void);

float pt_get_temp (uint8_t sensor);

void  pt_update (void);
float pt_get    (uint8_t num);

float adc_to_oC (uint16_t adc, uint8_t num);
float rtd_to_oC (float R);
float oC_to_rtd (float oC);

float polynomial (float *p, uint8_t n, float x);

void pt_dump     (void);
void pt_dump_pid (void);
void pt_dump_raw (void);
void pt_dump_avg (void);

void pt_get_error  (void);
void pt_set_error  (uint8_t num, int16_t err);
void pt_dump_error (void);

void rtd_start_log (void);
void rtd_stop_log  (void);


#endif
