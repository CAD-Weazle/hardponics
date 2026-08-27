// file    : control.c
// author  : rb
// purpose : collect log data & send to serial port and wifi channel
// date    : 221214
// last    : 230326
//
// VBAT: 11.47 IBAT: -14.46 VSOL: 15.45 T: 17.00 RH: 58.01 PAR: 1777 T_EC: 18.10 EC: 2.95
//

#include "includes.h"

float vbat = 0.0;            // [V]
float ibat = 0.0;            // [mA]
float vsol = 0.0;            // [V]
float temp = 0.0;            // [oC]
float humi = 0.0;            // [%RH]
float ec_t = 0.0;            // [oC]
float ec_e = 0.0;            // [mS]
uint32_t par = 0;            // [umol/s.m^2]

int log_on = 0;              // dump log output to serial port & wifi (with LOG_INTERVAL frequency)

int pump_run = PUMPS_DAY;    // pump regime

uint32_t pump_ontime;        // time pump is running [s]
uint32_t pump_period;        // time pump is idle [s]

// main control loop
void control (void)
{
  // check ESP32 still active
  esp32_wd_test ();

  // control ESP32 power consumption
  esp32_update ();

  // control OLED on timing
  oled_update ();

  // control water pumps
  pump_update ();

  // read sensors
  log_update ();

  // log data to UART/wifi
  log_dump ();

  // log data to UART only, every second <> debug only
//log_debug ();
}

// get timing parameters from EEPROM
void init_pump (void)
{
  pump_ontime = eeprom_read_long (EEPROM_BASE_PUMP);
  pump_period = eeprom_read_long (EEPROM_BASE_PUMP+4);

  // check EEPROM parameter, write default if reset
  if (pump_ontime == 0)
  {
    pump_ontime = PUMP_DAYTIME_ON;

    pump_set_ontime (PUMP_DAYTIME_ON);
  }

  // check EEPROM parameter, write default if reset
  if (pump_period == 0)
  {
    pump_period = PUMP_DAYPERIOD;

    pump_set_period (PUMP_DAYPERIOD);
  }
}

// set water pumping regime
void pump_regime (int dat)
{
  // sanity check
  if (dat > PUMPS_NIGHT)
    return;

  // set water pumping regime
  pump_run = dat;
}

// control water pumps
void pump_update (void)
{
//static int pump0_state = 0;
//static int pump1_state = 0;
  static int pump_tcnt = 0;      // pump on timer [s]
  static int pump_tpar = 0;      // counter for pump regime

//// handle PUMP0 on/off control touch button
//if (touch_pump0_pressed ())
//{
//   if (pump0_state == 0)
//   {
//     pump0_on ();
//     pump0_state = 1;
//   }
//   else
//   {
//     pump0_off ();
//     pump0_state = 0;
//   }
//}  
//
//// handle PUMP1 on/off control touch button
//if (touch_pump1_pressed ())
//{
//   if (pump1_state == 0)
//   {
//     pump1_on ();
//     pump1_state = 1;
//   }
//   else
//   {
//     pump1_off ();
//     pump1_state = 0;
//   }
//}  
//
//// time out touch button control water pumps - pumps can not be running too long
//if ((pump0_state == 1) || (pump1_state == 1))
//{
//  if (pump_tcnt++ >= PUMP_FLUSH_MAX)
//  {
//    // pumps off
//    pump0_off ();
//    pump1_off ();
//
//    // reset flags & interval counter
//    pump0_state = 0;
//    pump1_state = 0;
//
//    pump_tcnt = 0;
//  }
//
//  // bail
//  return;
//}

  // check pumping program is running
  if (pump_run == PUMPS_OFF)
    return;

  // water pumping during daytime
  if (pump_run == PUMPS_DAY)
  {
    // normal water pump control loop - runs only when touch button pump control inactive
    if (pump_tcnt < PUMP_DAYTIME_ON)
    {
      // water pumps on 
      pump0_on ();
      pump1_on ();
    } 
    else
    {
      // water pumps on 
      pump0_off ();
      pump1_off ();
    }
    
    // adjust counter
    if (pump_tcnt < PUMP_DAYPERIOD)
      pump_tcnt++;
    else
      pump_tcnt = 0;

    // switch night regime when dark
    if (par < PAR_DAYNIGHT_LEVEL)
    {
      if (pump_tpar++ > PAR_DAYNIGHT_DELAY)
      {
        pump_run = PUMPS_NIGHT;
        pump_tpar = 0;
        pump_tcnt = 0;

        printf2 ("#switch to night pump regime\n");
        return;
      }
    }
    else
    {
      // reset PAR counter
      pump_tpar = 0;
    }
  }

  // water pumping during nighttime 
  if (pump_run == PUMPS_NIGHT)
  {
    // normal water pump control loop - runs only when touch button pump control inactive
  //if (pump_tcnt < PUMP_NIGHTTIME_ON)
    if ((pump_tcnt < PUMP_NIGHTTIME_ON) && (par != 0))  // stop pumps at night (against noise polution)
    {
      // water pumps on 
      pump0_on ();
      pump1_on ();
    } 
    else
    {
      // water pumps on 
      pump0_off ();
      pump1_off ();
    }

    // adjust counter
    if (pump_tcnt++ >= PUMP_NIGHTPERIOD)
      pump_tcnt = 0;
  
    // currently pumps are running under night regime, check for sunlight
    if (par >= PAR_DAYNIGHT_LEVEL)
    {
      // switch to day regime when bright
      if (pump_tpar++ > PAR_DAYNIGHT_DELAY)
      {
        pump_run = PUMPS_DAY;
        pump_tpar = 0;
        pump_tcnt = 0;

        printf2 ("#switch to day pump regime\n");
        return;
      }
    }
    else
    {
      // reset PAR counter
      pump_tpar = 0;
    }
  }

//printf2 ("PAR: %d tpar: %d pump_tcnt: %d day/night: %d\n", par, pump_tpar, pump_tcnt, pump_run);
}

// update all sensors & OLED display
void log_update (void)
{
  static int cnt = 0;
  static int first = 1;

  // read battery current
  ibat = adc_read_ibat ();

  // read battery voltage
  vbat = adc_read_vbat ();

  // read solar panel voltage
  vsol = adc_read_vsol ();

  // read PAR
  par = adc_read_par ();

  // read EC sensor
  ec_t = ec_get_t ();
  ec_e = ec_get_e ();

  // read slow mofo once per 10 seconds
  if ((cnt++ >= 10) || (first == 1))
  {
    // update SHT15 T/RH sensor
    temp = sht15_get_t (); // read first, temperature needed for humidity calculation
    humi = sht15_get_h ();

    cnt = 0;
    first = 0;
  }

  // update OLED display
  oled_display_data (vbat, ibat, vsol, par, temp, humi);
}

// log data to local serial port only
void log_debug (void)
{
  static uint32_t pcnt = 0;  // packet counter

  // dump to serial port
  printf2 ("CNT: %ld ", pcnt);
  printf2 ("VBAT: %2.2f ", vbat);
  printf2 ("IBAT: %2.2f ", ibat);
  printf2 ("VSOL: %2.2f ", vsol);
  printf2 ("T: %2.2f "   , temp);
  printf2 ("RH: %2.2f "  , humi);
  printf2 ("PAR: %ld "   , par);
  printf2 ("T_EC: %2.2f ", ec_t);
  printf2 ("EC: %2.2f",    ec_e);

  if (pump_run == PUMPS_OFF)
    printf2 ("pumps off");

  if (pump_run == PUMPS_DAY)
    printf2 ("daytime");

  if (pump_run == PUMPS_NIGHT)
    printf2 ("nighttime");

  printf2 ("\n");

  // update packet counter
  pcnt++;
}

// log data to local serial port & to AP/Putty
void log_dump (void)
{
  static uint32_t pcnt = 0;  // packet counter
  static uint32_t tcnt = 0;  // time counter
  static uint32_t first = 1; // set flag

  // bail when not logging
  if (!log_on)
    return;

  // bail when log interval not elapsed 
  if (tcnt++ < LOG_INTERVAL)
  {
    if (first)   // don't wait for log interval first time
      first = 0; // reset flag
    else
     return;
  }

  // reset counter
  if (tcnt >= LOG_INTERVAL)
    tcnt = 0;

  // dump to serial port
  printf2 ("CNT: %ld ", pcnt);
  printf2 ("VBAT: %2.2f ", vbat);
  printf2 ("IBAT: %2.2f ", ibat);
  printf2 ("VSOL: %2.2f ", vsol);
  printf2 ("T: %2.2f "   , temp);
  printf2 ("RH: %2.2f "  , humi);
  printf2 ("PAR: %ld "   , par);
  printf2 ("T_EC: %2.2f ", ec_t);
  printf2 ("EC: %2.2f ",   ec_e);

  if (pump_run == PUMPS_OFF)
    printf2 ("pumps off");

  if (pump_run == PUMPS_DAY)
    printf2 ("daytime");

  if (pump_run == PUMPS_NIGHT)
    printf2 ("nighttime");

  printf2 ("\n");

  // dump to AP/PuTTY (always start with '%')
  printf3 ("%CNT: %ld ", pcnt);
  printf3 ("VBAT: %2.2f ", vbat);
  printf3 ("IBAT: %2.2f ", ibat);
  printf3 ("VSOL: %2.2f ", vsol);
  printf3 ("T: %2.2f "   , temp);
  printf3 ("RH: %2.2f "  , humi);
  printf3 ("PAR: %ld "   , par);
  printf3 ("T_EC: %2.2f ", ec_t);
  printf3 ("EC: %2.2f ",   ec_e);

  if (pump_run == PUMPS_OFF)
    printf3 ("pumps off");

  if (pump_run == PUMPS_DAY)
    printf3 ("daytime");

  if (pump_run == PUMPS_NIGHT)
    printf3 ("nighttime");

  printf3 ("\n");

  // update packet counter
  pcnt++;
}

// response on ToyBox client 'stat' command
void stat_dump (void)
{
  static uint32_t pcnt = 0;  // packet counter

  // dump to serial port
  printf2 ("CNT: %ld ", pcnt);
  printf2 ("VBAT: %2.2f ", vbat);
  printf2 ("IBAT: %2.2f ", ibat);
  printf2 ("VSOL: %2.2f ", vsol);
  printf2 ("T: %2.2f "   , temp);
  printf2 ("RH: %2.2f "  , humi);
  printf2 ("PAR: %ld "   , par);
  printf2 ("T_EC: %2.2f ", ec_t);
  printf2 ("EC: %2.2f ",   ec_e);

  if (pump_run == PUMPS_OFF)
    printf2 ("pumps off");

  if (pump_run == PUMPS_DAY)
    printf2 ("daytime");

  if (pump_run == PUMPS_NIGHT)
    printf2 ("nighttime");

  printf2 ("\n");

  // dump to AP (PuTTY/'client.py', always start with '#')
  printf3 ("#CNT: %ld ", pcnt);
  printf3 ("VBAT: %2.2f ", vbat);
  printf3 ("IBAT: %2.2f ", ibat);
  printf3 ("VSOL: %2.2f ", vsol);
  printf3 ("T: %2.2f "   , temp);
  printf3 ("RH: %2.2f "  , humi);
  printf3 ("PAR: %ld "   , par);
  printf3 ("T_EC: %2.2f ", ec_t);
  printf3 ("EC: %2.2f ",   ec_e);

  if (pump_run == PUMPS_OFF)
    printf3 ("pumps off");

  if (pump_run == PUMPS_DAY)
    printf3 ("daytime");

  if (pump_run == PUMPS_NIGHT)
    printf3 ("nighttime");

  printf3 ("\n");

  // update packet counter
  pcnt++;

  // let ESP32 transmit data
  msleep (2000);
//esp32_reset ();
}

// check battery empty
int bat_empty (void)
{
  return ((vbat < (CELLS_IN_SERIES * VOLTAGE_CELL_EMPTY)) ? 1 : 0);  // 9V0
}

// check battery full
int bat_full (void)
{
  return ((vbat > (CELLS_IN_SERIES * VOLTAGE_CELL_FULL)) ? 1 : 0); // 12V75
}

// start logging
void log_start (void)
{
  log_on = 1;
}

// stop logging
void log_stop (void)
{
  log_on = 0;
}

// write water pump on time to EEPROM
void pump_set_ontime (uint32_t dat)
{
  eeprom_write_long (EEPROM_BASE_PUMP, dat);

  // update state
  pump_ontime = dat;
}

// write water pump period to EEPROM
void pump_set_period (uint32_t dat)
{
  eeprom_write_long (EEPROM_BASE_PUMP+4, dat);

  // update state
  pump_period = dat;
}

// dump water pump parametsrs
void pump_dump (void)
{
  printf2 ("#pump ontime: %d / period: %d [s]\n", pump_ontime, pump_period);
  printf3 (">pump ontime: %d / period: %d [s]\n", pump_ontime, pump_period);
}






