// file    : pid.c
// author  : rb
// purpose : PID temperature control
// date    : 050522
// last    : 181227

#include "includes.h"

#define PID_DEBUG            1

// PID settings
float pid_Kp = 1.25;         // P-gain
float pid_Ki = 0.00125;       // I-gain
float pid_Kd = 0.0;          // D-gain

// PID variables
float t_actual;              // HotBed actual temperature
float t_target;              // HotBed target temperature
float t_error;               // error between target and actual temperature

float pid_p = 0.0;           // P-term
float pid_i = 0.0;           // I-term
float pid_d = 0.0;           // D-term

float pid_pwm = 0.0;         // heater PWM signal

uint8_t pid_active;          // PID active/inactive flag
uint8_t pid_log;             // PID data logging flag
uint8_t pid_tune;            // PID state logging flag

// set up PID controller
void init_pid (void)
{
  t_target   = 25.0;         // set default target temperature

  pid_active = 0;            // PID flag
  pid_log    = 0;            // logging flag
  pid_tune   = 0;            // tuning flag
}

// enable HotBed PID temperature control 
void pid_start (void)
{
  pid_i = 0.0;               // reset I-term
  pid_active = 1;            // set flag

  debug2 (PID_DEBUG, "enable HotBed PID temperature control\n");
}

// disable HotBed PID temperature control 
void pid_stop (void)
{
  pwm_set_heater (0);        // heater off
  pid_active = 0;            // clear flag

  debug2 (PID_DEBUG, "disable HotBed PID temperature control\n");
}

// start PID state logging to terminal
void pid_start_log (void)
{
  pid_log = 1;
}

// stop PID state logging to terminal
void pid_stop_log  (void)
{
  pid_log = 0;
}

// control temperature HotBed
void pid_update (void)
{
  int16_t pwm_out;

  // update HotBed temperature 
  t_actual = pt_get_temp (SENSOR_BED);

  // bail if PID not active
  if (!pid_active)
    return;

  // calculate error between target and actual temperature
  t_error = t_target - t_actual;
	
  // update P & I
  pid_p = pid_Kp * t_error;

  // update I (only for small errors to prevent windup)
  if ((t_error < 2.5) || (t_error > -2.5))
    pid_i += pid_Ki * t_error;

  // clip I
  if (pid_i > I_MAX)
    pid_i = I_MAX;
  else if (pid_i < I_MIN)
    pid_i = I_MIN;

  // calculate PWM output
  pid_pwm = pid_p + pid_i + pid_d;

  pwm_out = (int16_t)(pid_pwm * 100);

  // clip PWM signal
  if (pwm_out > 100)
    pwm_out = 100;
  else if (pwm_out < 0)
    pwm_out = 0;

  // adjust heater
  pwm_set_heater ((uint8_t)pwm_out);

  // dump PID data - for normal PID logging
  if (pid_log)
    pid_dump_temp ();

  // dump PID state - for PID tuning
  if (pid_tune)
  {
    printf2 ("Tact: %d.%02d ", (uint16_t)(t_actual * 100) / 100, 
                               (uint16_t)(t_actual * 100) % 100);

    printf2 ("Ttrg: %d.%02d ", (uint16_t)(t_target * 100) / 100, 
                               (uint16_t)(t_target * 100) % 100);

    printf2 ("e: %d ", (int16_t)(t_error*100));
    printf2 ("P: %d ", (int16_t)(pid_p*100));
    printf2 ("I: %d ", (int16_t)(pid_i*1000));

    printf2 ("pwm_out: %d ", pwm_out);
    printf2 ("pid_Kp: %d ",  (uint16_t)(pid_Kp*1000));
    printf2 ("pid_Ki: %d\n", (uint16_t)(pid_Ki*1000));
  }
}

// control temperature HotBed
void pid_set_target (uint16_t target)
{
  // set new target & reset I term
  t_target = (float)target / 100;
  pid_i = 0.0;
}

// get HotBed target temperature
float pid_get_target (void)
{
  return t_target;
}

// dump PID state
void pid_dump_temp (void)
{
  printf2 ("Tact: %d.%02d ", (uint16_t)(t_actual * 100) / 100, 
                             (uint16_t)(t_actual * 100) % 100);

  printf2 ("Ttrg: %d.%02d ", (uint16_t)(t_target * 100) / 100, 
                             (uint16_t)(t_target * 100) % 100);

//printf2 ("PID %s ", pid_active ? ("active") : ("inactive"));

  pt_dump_pid ();
}

// set P-gain PID controller
void pid_set_kp (float dat)
{
  pid_Kp = dat;
}

// set I-gain PID controller
void pid_set_ki (float dat)
{
  pid_Ki = dat;
}

// get PID active state
uint8_t pid_get_active (void)
{
  return pid_active;
}


