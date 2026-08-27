// file    : control.h
// author  : rb
// purpose : header file for control.c
// date    : 221214
// last    : 230602
//
// note    : pump volume ~200mL/10sec

#ifndef _CONTROL_H_
#define _CONTROL_H_

// -- defines
//#define LOG_INTERVAL                     (10*60) // time delay log data [s]
#define LOG_INTERVAL                          (20) // time delay log data [s]
                                                     
#define PAR_DAYNIGHT_LEVEL                     50  // minimum light level for daytime [PAR]
#define PAR_DAYNIGHT_DELAY                     60  // switch day/night regime delay [s]

#define PUMPS_OFF                               0  // water pumping program not running
#define PUMPS_DAY                               1  // water pump control during day   - high flow/power consumption
#define PUMPS_NIGHT                             2  // water pump control during night - low flow/power consumption

// too little water:                                                 
//#define PUMP_DAYTIME_ON                      10  // during daytime interval water pump is on [s]
//#define PUMP_DAYTIME_OFF                    110  // during daytime interval water pump is off [s] 
//#define PUMP_DAYPERIOD (PUMP_DAYTIME_ON + PUMP_DAYTIME_OFF) // during daytime total pump on/off period

// 230523: test this - too little in bottom tray
//#define PUMP_DAYTIME_ON                      30  // during daytime interval water pump is on [s]
//#define PUMP_DAYTIME_OFF                    150  // during daytime interval water pump is off [s] 
//#define PUMP_DAYPERIOD (PUMP_DAYTIME_ON + PUMP_DAYTIME_OFF) // during daytime total pump on/off period

// 230602: test this
#define PUMP_DAYTIME_ON                        60  // during daytime interval water pump is on [s]
#define PUMP_DAYTIME_OFF                      120  // during daytime interval water pump is off [s] 
#define PUMP_DAYPERIOD (PUMP_DAYTIME_ON + PUMP_DAYTIME_OFF) // during daytime total pump on/off period

#define PUMP_NIGHTTIME_ON                      30  // during nighttime interval water pump is on [s]
#define PUMP_NIGHTTIME_OFF                    270  // during nighttime interval water pump is off [s] 
#define PUMP_NIGHTPERIOD (PUMP_NIGHTTIME_ON + PUMP_NIGHTTIME_OFF) // during nighttime total pump on/off period

#define PUMP_FLUSH_MAX                         15  // maximum time pumps are allowed to run when touch button controlled [s]
                                                   
#define CELLS_IN_SERIES                         3  // number op Li-ion cells in series (3 = 12V system)
#define VOLTAGE_CELL_EMPTY                  (3.00) // Li-ion cell clamp voltage empty
#define VOLTAGE_CELL_FULL                   (4.25) // Li-ion cell clamp voltage full

// -- prototypes
void control (void);

void init_pump (void);

void pump_regime (int dat);

void control_update (void);

void log_update (void);

void log_debug (void);
void log_dump  (void);
void log_start (void);
void log_stop  (void);

int bat_empty (void);
int bat_full  (void);

void log_dump  (void);
void stat_dump (void);

void pump_set_ontime (uint32_t dat);
void pump_set_period (uint32_t dat);
void pump_dump (void);

#endif
