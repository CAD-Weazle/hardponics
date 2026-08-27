// file    : ec.h
// author  : rb
// purpose : header file for ec.c
// date    : 190502
// last    : 230114
//

#ifndef _EC_H_
#define _EC_H_

// -- defines
#define EC_CELL_DEFAULT               (1.4107276)  // default cell constant for uncalibrated EC sensor

#define EC_NUM_DEVICES                          1  // number of EC sensors
#define EC_FILTERDEPTH                         64  // moving average filter depth
#define EC_WAVE_DELAY                           2  // excitation wave inter-sample delay
//#define EC_CAL_WAIT                        1200  // waiting time during calibration [s/10]
#define EC_CAL_WAIT                           120  // waiting time during calibration [s/10]
#define EC_NUM_CYCLES                           3  // number of full exitation sine waves (must be uneven)

#define EC_NUM_DOTS                       (128/1)  // number of points single full sine wave
#define EC_SINE_AMP                           200  // amplitude excitation signal (ADC out = 350 mVpp)
#define EC_DAC_TWEAK                          185  // adjustment for 3V3 STM32 & 3V0 OPAMP supply mismatch

#define PI                            3.141592654  // good old pi...

#define EC_Rf                                 510  // I/U converter feedback resistor (not used)
#define EC_ALFA                            (0.02)  // temperature correction coeff [%/oC]

// calibration state machine
#define EC_CAL_IDLE                             0
#define EC_CAL_START                            1
#define EC_CAL_INSERT                           2
#define EC_CAL_BUSY                             3
#define EC_CAL_CLEAN                            4
#define EC_CAL_READY                           99

// -- prototypes
void init_ec (void);

void ec_update (void);

void ec_get (void);

void     ec_sjw (void);
uint16_t ec_peak_detect (void);
float    ec_filter (uint16_t dat);

void ec_cal_sensor (void);
void ec_calibrate  (void);

void ec_dump (void);

float ec_get_t (void);
float ec_get_e (void);

void ec_log_start (void);
void ec_log_stop  (void);

void ec_dump_tab (void);

#endif




