// file    : ntc.c
// author  : rb
// purpose : NTC linearization routines
// date    : 080622
// last    : 220622
//          
// note    : NCP18XH103F03RB, Murata  10K/1% NTC used, RS# 725-9050 / DK# 490-4800-2-ND
//           B-value = 3380 ±1% (25-50oC)
//
// note   : TDS-NTC-38 sensor used from Ali Express
//          10K NTC, B-value = 3950 (??-??oC, assume 25-50oC)

// use Cypress "AN2017_S_H_Constant_Calc.xls" for Steinhart-Hart coeff. calculation
//

#include "includes.h"

// globals
float ntc_temp[NTC_NUM_SENSORS];                 // NTC temperature, fixed point notation [oC]
float ntc_err[NTC_NUM_SENSORS];                  // temperature offset errors retrieved from user FLASH

// init NTC measurement
void init_ntc (void)
{
  // read NTC offset error parameters from EEPROM
  ntc_get_error ();
}

// handle NTC sensors
void ntc_update (void)
{
  // convert NTC ADC data
  for (int i = 0; i < NTC_NUM_SENSORS; i++)
  {
    // convert to temperature
    ntc_temp[i] = ntc_convert (ntc_fil[i], i);
  }
}

// calculate NTC restistance from ADC data & convert to temperature
float ntc_convert (uint32_t adc_val, uint8_t channel)
{
  float R;

  // sanity check
  if (channel >= NTC_NUM_SENSORS)
    return (0);

  // calculate NTC resistance from voltage divider output 
  R = (float)((R_PULL*adc_val) / (MAX_ADC - adc_val));

  // calculate temperature from NTC resistance using Steinhart-Hart & add correction
  return (((1.0/(NTC_PARAM_A + log(R)*(NTC_PARAM_B + NTC_PARAM_C*log(R)*log(R)))) - 273.15)) + ntc_err[channel];
}

// dump NTC data
void ntc_dump (void)
{
#ifdef NTC_DEBUG
  printf2 ("NTC_RAW: %d %d %d ", ntc_avg[0], ntc_avg[1], ntc_avg[2]);
#endif 
  printf2 ("NTC: %2.1f %2.1f %2.1f %2.1f  ", ntc_temp[0], ntc_temp[1], ntc_temp[2], ntc_temp[3]);
}

// dump NTC data to BLE module
void ntc_dump_ble (void)
{
  printf3 ("NTC: %2.1f %2.1f %2.1f %2.1f  ", ntc_temp[0], ntc_temp[1], ntc_temp[2], ntc_temp[3]);
}

// get NTC error parameter from EEPROM
void ntc_get_error (void)
{
  // read data from EEPROM
  for (int i = 0; i < NTC_NUM_SENSORS; i++)
    ntc_err[i] = ee_get_ntccal (i);
}

// dump NTC error parameters, working set stored in SRAM 
void ntc_dump_error (void)
{
  ntc_get_error ();

  for (int i = 0; i < NTC_NUM_SENSORS; i++)
    printf2 ("#E%d: %02d ", i, ntc_err[i]);

  printf2 ("\n");
}

// set NTC offset error [oC*10]
void ee_set_ntccal (int num, int err)
{
  // sanity check
  if (num >= NTC_NUM_SENSORS)
    return;

  eeprom_write_long (EEPROM_BASE_NTC + num*4, err);  
}

// get NTC offset error [oC*10]
float ee_get_ntccal (int num)
{
  // sanity check
  if (num >= NTC_NUM_SENSORS)
    return 0;

  float tmp = eeprom_read_long (EEPROM_BASE_NTC + num*4);

  // convert to [oC]
  return (tmp/10);  
}

// dump NTC error parameters as stored in EEPROM
void ee_dump_ntccal (void)
{
  printf2 ("#NTC offset errors:\n");

  for (int i = 0; i < 4; i++)
    printf2 ("#NTC%d: %1.1f \n", i, ee_get_ntccal (i));
}




// -- only needed during development with new NTC --

#if (NTC_TABLE_DEBUG)
// compare calculated temperature against data from NTC R/T table
void ntc_test_steinhart (void)
{
  float Ttable, Tstein, Rtable;

  for (int i = 0; i < NUM_TABLE_POINTS; i++)
  {
    Rtable = ntc_points[i][1];         // read resistance from table
    Ttable = ntc_points[i][0];         // read temperature from table
    Tstein = steinhart (Rtable);       // calculated T

    printf2 ("Ttable: %4d ", (long)Ttable);  
    printf2 ("Rtable: %7d ", (long)Rtable);  
    printf2 ("Tstein: %d\n", (long)Tstein*10);  
  }
}

// simulate NTC using beta parameter, dev only
// note: not very accurate
float ntc_simulate (float T)
{

  // NTC: Rt = Ro * exp (B * (1/T - 1/To)).
  //         = Ro * exp (-B/To) * exp (B/T)
  //         =  C * exp (B/T)
  // Ro = 10K
  // B  = 3380
  // To = 25 oC (298 K)

  T += 273.15;
  return (exp (3380/T) * 1.4838E-03); // wrong faktor, what'ev
}
#endif







