// file    : par.h
// author  : rb
// purpose : header file for par.c
// date    : 221119
// last    : 221214
//

#ifndef _PAR_H_
#define _PAR_H_

// -- defines 
#define PAR_FACTOR                         (4.57)  // Kipp & Zonen PAR sensor calibration factor [uV/umol/m^2.s]
#define PAR_LSB                          (0.4343)  // after calibration with 5mVDC input voltage
#define PAR_AMP_GAIN                      (405.0)  // two-stage chopper OPAMP gain (not used)

// -- protoypes
float par_read (void);

void par_dump (void);

void par_log_start (void);
void par_log_stop  (void);

#endif
