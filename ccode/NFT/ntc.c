// file    : ntc.c
// author  : rb
// purpose : NTC linearization routines (temperature sensor in EC probes)
// date    : 080622
// last    : 221125
//
// old     : NCP18XH103F03RB, Murata  10K/1% NTC used, RS# 725-9050
//           B-value = 3380 ±1% (25-50oC)
//
// note    : TDS-NTC-38 sensor used from Ali Express
//           10K NTC, B-value = 3950 (??-??oC, assume 25-50oC)

// use Cypress "AN2017_S_H_Constant_Calc.xls" for Steinhart-Hart coeff. calculation
//

#include "includes.h"

#define NTC_DEBUG

// globals
float ntc_avg;                                   // NTC ADC data average 
float ntc_temp;                                  // NTC temperature, fixed point notation [doC]
float ntc_err;                                   // temperature offset errors retrieved from user FLASH

uint16_t ntc_ftab[NTC_FILTERDEPTH];              // storage for moving average filter

// init NTC measurement
void init_ntc (void)
{
  // read NTC offset error parameters from EEPROM
  ntc_get_error ();
}

// read NTC & filter 
void ntc_update (void)
{
  uint32_t ntc_dat;
  
  // read ADC 
  ntc_dat = adc_read_ntc ();

  // apply moving average filter
  ntc_avg = ntc_filter (ntc_dat);

  // convert to temperature
  ntc_temp = ntc_convert (ntc_avg);
}

// get actual EC sensor temperature
float ntc_get (void)
{
  return (ntc_temp);
}

// naive moving average filter
float ntc_filter (uint16_t dat)
{
  static int first = 1;  // speed up filter after start-up
  uint32_t sum = 0;

  // speed up filter 
  if (first)
  {
    for (int i = 0; i < NTC_FILTERDEPTH; i++)
      ntc_ftab[i] = dat;

    first = 0;
  }
  
  // shift historical data in array & add new data point
  for (int i = NTC_FILTERDEPTH-1; i > 0; i--)
    ntc_ftab[i] = ntc_ftab[i-1];

  ntc_ftab [0] = dat;

  // calculate average of all data points
  for (int i = 0; i < NTC_FILTERDEPTH; i++)
    sum += ntc_ftab[i];

  return ((float)sum/NTC_FILTERDEPTH);
}

// calculate NTC restistance from ADC data & convert to temperature
float ntc_convert (float adc_val)
{
  // calculate NTC resistance from voltage divider output 
  float R = (R_PULL * adc_val) / (MAX_ADC - adc_val);

  // calculate temperature from NTC resistance using Steinhart-Hart & add correction
  return (((1.0/(NTC_PARAM_A + log(R)*(NTC_PARAM_B + NTC_PARAM_C*log(R)*log(R)))) - 273.15)) + ntc_err;
}

// dump NTC data
void ntc_dump (void)
{
#ifdef NTC_DEBUG
  printf2 ("NTC_RAW: %2.2f ", ntc_avg);
#endif 
  printf2 ("NTC_TEMP: %2.2f\n", ntc_temp);
}

// get NTC error parameter from EEPROM
void ntc_get_error (void)
{
  // read data from EEPROM
//ntc_err = ee_get_ntccal (0);
  ntc_err = -3.7;
}

// dump NTC error parameters, working set stored in SRAM 
void ntc_dump_error (void)
{
  ntc_get_error ();

  printf2 ("E: %02d ", ntc_err);

  printf2 ("\n");
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
  return (exp (3380/T) * 1.4838E-03); // wrong fakte, what'ev
}
#endif







