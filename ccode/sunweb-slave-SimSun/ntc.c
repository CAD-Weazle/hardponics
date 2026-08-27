// file    : ntc.c
// author  : rb
// purpose : NTC linearization routines
// date    : 080622
// last    : 190906
// note    : NCP18XH103F03RB, Murata  10K/1% NTC used, RS# 725-9050
//           B-value = 3380 ±1% (25-50oC)
//
// use Cypress "AN2017_S_H_Constant_Calc.xls" for Steinhart-Hart coeff. calculation
//
// note    : ADC signal noisy with low LED light levels (< 10), 
//           don't use temperature data then
//
// NTC offsets:
// SimSun 0x11 = -2.5 oC 
// SimSun 0x12 = -2.6 oC
// SimSun 0x13 = -2.2 oC

#include "includes.h"

#define NTC_TABLE_DEBUG  0

// globals
uint32_t ntc_avg = 0;                  // NTC ADC data average 
uint16_t ntc_temp;                     // NTC temperature, fixed point notation

// temperature offset errors retrieved from user FLASH
int32_t ntc_err = 0;       

// init NTC measurement
void init_ntc (void)
{
  // read NTC error parameters from FLASH
  ntc_get_error ();
}

// read temperature
uint16_t ntc_gettemp (void)
{
  return ntc_temp;
}

// read NTC & filter 
void ntc_update (void)
{
         uint32_t ntc_dat;
  static uint32_t ntc_old = 0;

  // read ADC
  ntc_dat = adc_read_channel (0);

  // exponential filter: Xn = (1-a).Xn-1 + a.New (a = 1/8)
  ntc_avg = ((NTC_FILTER-1)*ntc_old + ntc_dat) / (NTC_FILTER);
  ntc_old = ntc_dat;

  // convert to temperature
  ntc_temp = ntc_convert ();
}

// scale NTC ADC data & convert to temperature
uint16_t ntc_convert (void)
{
  float R;

  // scale NTC ADC data to resistance
  R = (float)ntc_avg * 6.3219;

  // calculate temperature from NTC resistance using Steinhart-Hart & add correction
  // (*10 for conversion to single digit fixed point notation)
  return (uint16_t)(((1.0/(NTC_PARAM_A + \
                   log(R)*(NTC_PARAM_B + NTC_PARAM_C*log(R)*log(R)))) - 273.15) * 10) \
                   + ntc_err;
}

// dump NTC data
void ntc_dump (void)
{
#ifdef NTC_DEBUG
  printf2 ("NTC_RAW: %d ", ntc_avg);
#endif 
  printf2 ("NTC_TEMP: %d.%d\n", ntc_temp / 10, ntc_temp % 10);
}

// get NTC error parameter from FLASH
void ntc_get_error (void)
{
  flash_read_ntcerr (&ntc_err, 1);
  printf2 ("read from FLASH: %d\n", ntc_err);
}

// write NTC error parameters to FLASH
void ntc_set_error (int16_t err)
{
  ntc_err = err;
  flash_write_ntcerr (&ntc_err, 1);
}

// dump NTC error parameters, working set stored in SRAM
void ntc_dump_error (void)
{
  int32_t sign;

  sign = (ntc_err < 0) ? -1 : 1;
  printf2 ("NTC error: %d.%d\n", ntc_err/10, sign*ntc_err%10);
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







