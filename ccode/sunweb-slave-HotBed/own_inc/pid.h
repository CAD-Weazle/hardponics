// file    : pid.h
// author  : rb
// purpose : header file for pid.h
// date    : 050522
// last    : 181220
//

#ifndef __PID_H__
#define __PID_H__

// -- defines
#define I_MAX             10.0
#define I_MIN            -10.0

// -- prototypes
void init_pid (void);

void pid_start (void);
void pid_stop  (void);

void pid_start_log (void);
void pid_stop_log  (void);

void pid_update (void);

void  pid_set_target (uint16_t target);
float pid_get_target (void);

void pid_dump_temp (void);

void pid_set_kp (float dat);
void pid_set_ki (float dat);

uint8_t pid_get_active (void);

#endif
