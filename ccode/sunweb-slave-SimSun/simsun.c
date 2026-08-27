// file    : simsun.c
// author  : rb
// purpose : SimSun LED driving routines
// date    : 180823
// last    : 191215

#include "includes.h"

//#define SIMSUN_DEBUG

// SimSun light cycle flag
uint8_t do_simsun = 0;        // light program flag

// day light period parameters
uint32_t time_dawn;           // start of day, LEDs on [s]
uint32_t time_dusk;           // end of day, LEDs off  [
uint32_t time_now;            // current time [s]

// current LED light intensity
uint8_t dred_val = LED_OFF;   // current light intensity Deep Red
uint8_t rblu_val = LED_OFF;   // current light intensity Royal Blue
uint8_t fred_val = LED_OFF;   // current light intensity Far Red

// target light intensity
uint8_t dred_max = 10;        // maximum light intensity Deep Red during ramp up
uint8_t rblu_max = 10;        // maxumum light intensity Royal Blue during ramp up

// SimSun statemachine
int already_on  = 0;
int already_off = 1;

// init LED day light program
void init_simsun (void)
{
  // start with default daylight pattern
  simsun_set_dawn ( 9,  0,  1); // sun-up time
  simsun_set_dusk (21,  0,  1); // sun-set time

  // LEDs off
  dred_val = LED_OFF;  
  rblu_val = LED_OFF;  

  // reset statemachine 
  already_on  = 0;
  already_off = 1;
}

// track LED day light program (called in 1 sec ticker)
void simsun_update (void)
{
  // bail when not running LED light program
  if (!do_simsun)
    return;

  // get current time & convert to seconds
  rtc_update_time ();
  time_now = rtc_convert_time ();

  // LEDs on between dusk & dawn
  if ((time_now > time_dawn) && (time_now < time_dusk))
  {
    dred_val = dred_max;
    rblu_val = rblu_max;
  }
  else
  {
    dred_val = LED_OFF;
    rblu_val = LED_OFF;
  }

/*
  // sanity check data <> needed?
  if (dred_val > dred_max)
    dred_val = dred_max;

  if (rblu_val > rblu_max)
    rblu_val = rblu_max;

  // control LED light intensity - Deep Red & Royal Blue only
  if ((time_now > time_dawn) && (time_now < time_dusk))
  {
    // ramp up light intensity
    if (!already_on)
    {
      if (dred_val < dred_max)
        dred_val++;

      if (rblu_val < rblu_max)
        rblu_val++;

      if ((dred_val >= dred_max) && (rblu_val >= rblu_max))
      {
        already_on  = 1;
        already_off = 0;
      }
    }
  }
  else
  {
    // ramp down light intensity
    if (!already_off)
    {
      if (dred_val > 0)
        dred_val--;

      if (rblu_val > 0)
        rblu_val--;
      
      if ((dred_val <= 0) && (rblu_val <= 0))
      {
        already_on  = 0;
        already_off = 1;
      }
    }
  }

*/
  // set LED light intensity - Deep Red & Royal Blue only, Far Red is off
  leds_dred (dred_val);
  leds_rblu (rblu_val);

#ifdef SIMSUN_DEBUG
  // dump state
  simsun_dump_state ();
#endif
}

// start SimSun ligth program
void simsun_start (void)
{
  // set flag
  do_simsun = 1;
}

// stop SimSun ligth program
void simsun_stop (void)
{
  // clear flag
  do_simsun = 0;

  // LEDs off
  dred_val = LED_OFF;  
  rblu_val = LED_OFF;  
}

// set time of dawn, converted to seconds
void simsun_set_dawn (uint8_t hrs, uint8_t min, uint8_t sec)
{
  // input data sanity check, bail on error
  if ((hrs >= 24) || (min >= 60) || (sec >= 60))
    return;

  time_dawn = hrs*3600 + min*60 + sec;
}

// set time of dusk, converted to seconds
void simsun_set_dusk (uint8_t hrs, uint8_t min, uint8_t sec)
{
  // input data sanity check, bail on error
  if ((hrs >= 24) || (min >= 60) || (sec >= 60))
    return;

  time_dusk = hrs*3600 + min*60 + sec;
}

// set target light intensity Deep Red
void simsun_dred_max (uint8_t val)
{
  dred_max = val;        
}

// set target light intensity Royal Blue
void simsun_rblu_max (uint8_t val)
{
  rblu_max = val;        
}

// get current light intensity Deep Red
uint8_t simsun_get_dred (void)
{
  return dred_val;        
}

// get current light intensity Royal Blue
uint8_t simsun_get_rblu (void)
{
  return rblu_val;        
}

// get current light intensity Far Red
uint8_t simsun_get_fred (void)
{
  return fred_val;        
}

// dump SimSun state
void simsun_dump_state (void)
{
//rtc_dump_time ();

  printf2 ("t_now: %d "     , time_now);
  printf2 ("t_dawn: %d "    , time_dawn);
  printf2 ("t_dusk: %d    " , time_dusk);

  printf2 ("dred: %d / %d "   , dred_val, dred_max);
  printf2 ("rblu: %d / %d    ", rblu_val, rblu_max);

  ntc_dump ();
}

