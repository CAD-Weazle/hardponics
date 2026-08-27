// file    : rtd.c
// author  : ao/rb
// purpose : Pt100 conversion
// date    : 030324
// last    : 181208
//
// 'rtd_err' PCB HEATER 0x21:
// E0: 2.08
// E1: 2.04
// E2: broken
//
// 'rtd_err' PCB HEATER 0x22:
// E0: 2.45
// E1: 2.41
// E2: 2.22
//
// 'rtd_err' PCB HEATER 0x23:
// E0: 1.92
// E1: 2.10
// E2: 2.22
//
// calibration RTD circuit with fixed resistors
// 
//   R      pt0     pt1     pt2      avg
//  80R5    5026    broken  4994                     --  -49.6 oC - not used
//  91R4    5681    broken  5645                     --  -21.9 oC - not used
// 100R3    6223    broken  6177                     --    0.8 oC 
// 109R7    6777    broken  6732                     --   24.9 oC
// 120R9    7440    broken  7386                     --   53.9 oC                 
// 129R3    7932    broken  7876                     --   75.8 oC 
//
// f1(x) = -5.23707537865403 + 0.01695856131096208*x
// f2(x) = -5.20552173655432 + 0.01707508029343002*x
// f1(x) = -5.2370 + 0.01695*x
// f2(x) = -5.2055 + 0.01707*x

#include "includes.h"

#define RTD_DEBUG            0

uint8_t rtd_log = 0;                             // log temperature

// Pt100 sensor's I2C address
uint8_t rtd_adr[PT_NUM] = {MCP3421A0, MCP3421A1, MCP3421A3};

// ADC data filter
uint32_t adc_raw[PT_NUM];                        // raw data from ADC
uint32_t adc_old[PT_NUM];                        // filter state
uint32_t adc_avg[PT_NUM];                        // averaged data ADC, used for temperature calculation

// current temperature 
float rtd_cur[PT_NUM] = {99.99, 99.99, 99.99};   // 99.99 oC is 'error' temperature

// temperature offset errors retrieved from user FLASH
int32_t rtd_err[PT_NUM] = {0, 0, 0};       

// ADC curve, measured with fixed resistors
float adc_curve[2] = 
{
 -5.2370,
  0.01695
};

// ITS-90 linearisation coefficients
float pt100[5] = 
{
  1.0000E+02,  
  3.9083E-01,  
 -5.7750E-05,  
  4.1830E-08,  // extra coeff.
 -4.1830E-10   // extra coeff.
};

// derivatitve of 3-term polynome
float pt100_d[2] = 
{
  3.9083E-1,
 -5.7750E-5 * 2
};

// init Pt100 measurement
void init_rtd (void)
{
  // read Pt100 error parameters from FLASH
  pt_get_error ();
}

// get current temperature 
float pt_get_temp (uint8_t sensor)
{
  // return hot bed or air temperature
  if (sensor == SENSOR_BED)
    return (rtd_cur[PT_SENSOR1] + rtd_cur[PT_SENSOR2]) / 2;  
  else if (sensor == SENSOR_PT1)
    return rtd_cur[PT_SENSOR1];
  else if (sensor == SENSOR_PT2)
    return rtd_cur[PT_SENSOR2];
  else
    return rtd_cur[PT_SENSOR3];
}

// read ADCs & average 
void pt_update (void)
{
  rtd_cur[PT_SENSOR1] = pt_get (PT_SENSOR1);
  rtd_cur[PT_SENSOR2] = pt_get (PT_SENSOR2);
  rtd_cur[PT_SENSOR3] = pt_get (PT_SENSOR3);

  debug2 (RTD_DEBUG, "\n");
}

// calculate current temperature
float pt_get (uint8_t num)
{
  static uint8_t first[PT_NUM] = {1, 1, 1};

  // read selected sensor
  adc_raw[num] = (uint32_t)mcp3421_read (rtd_adr[num]);

  // exponential filter: Xn = (1-a).Xn-1 + a.New
  if (first[num])
  {
    // speed up filter after RESET
    adc_avg[num] = adc_raw[num];
    first[num] = 0;
  }
  else
  {
    adc_avg[num] = ((PT_FILTER-1)*adc_old[num] + adc_raw[num]) / PT_FILTER;
  }

  // store current sample
  adc_old[num] = adc_avg[num];   

  // & convert average to temperature
  return (adc_to_oC ((uint16_t)adc_avg[num], num));
}

// convert ADC counts to Pt100 temperature
float adc_to_oC (uint16_t adc, uint8_t num)
{
  float R, oC_raw, oC;
 
  // correct for non-linear ADC / Pt100 excitation source
  R = polynomial (adc_curve, 2, adc);

  // correct for Pt100 non-linearity
  oC_raw = rtd_to_oC (R);

  // remove cable error
  oC = oC_raw + (float)rtd_err[num] / 100; 

  // signal defective/disconnected sensor
  if ((oC < 0.0) || (oC > MAX_TEMP))
    oC = 99.99;

  debug2 (RTD_DEBUG, "ADC: %d ", adc);
  debug2 (RTD_DEBUG, "R: %d.%02d ",      ((uint16_t)(R*100))/100, ((uint16_t)(R*100))%100);
  debug2 (RTD_DEBUG, "oC_raw: %d.%02d ", ((uint16_t)(oC_raw*100))/100, ((uint16_t)(oC_raw*100))%100);
  debug2 (RTD_DEBUG, "oC: %d.%02d ",     ((uint16_t)(oC*100))/100, ((uint16_t)(oC*100))%100);

  return (oC);
}

// convert RTD resistance to temperature
// note: when temperature < 0C, use higher order polynome
float rtd_to_oC (float R)
{
  char i;
  float oC = 0.0;

  for (i = 0; i < 3; i++)
  {
    oC -= (polynomial (pt100, oC < 0 ? 5 : 3, oC) - R)
         / polynomial (pt100_d, 2, oC); 
  }

  return (oC);
}

// convert temperature to RTD resistance
float oC_to_rtd (float oC)
{
  return (polynomial (pt100, oC < 0 ? 5 : 3, oC));
}

// evaluate polynomial function
float polynomial (float *p, uint8_t n, float x)
{
  float y = p[--n];

  while (n > 0)
    y = y * x + p[--n];

  return (y);
}

// dump current temperature
void pt_dump (void)
{
  if (!rtd_log) 
    return;

  // integer version
  printf2 ("T1: %02d.%02d T2: %02d.%02d T3: %02d.%02d\n", (uint16_t)(rtd_cur[0] * 100) / 100, (uint16_t)(rtd_cur[0] * 100) % 100,
                                                          (uint16_t)(rtd_cur[1] * 100) / 100, (uint16_t)(rtd_cur[1] * 100) % 100,
                                                          (uint16_t)(rtd_cur[2] * 100) / 100, (uint16_t)(rtd_cur[2] * 100) % 100);
}

// dump current temperature for PID routines
void pt_dump_pid (void)
{
  printf2 ("T1: %02d.%02d T2: %02d.%02d T3: %02d.%02d\n", (uint16_t)(rtd_cur[0] * 100) / 100, (uint16_t)(rtd_cur[0] * 100) % 100,
                                                          (uint16_t)(rtd_cur[1] * 100) / 100, (uint16_t)(rtd_cur[1] * 100) % 100,
                                                          (uint16_t)(rtd_cur[2] * 100) / 100, (uint16_t)(rtd_cur[2] * 100) % 100);
}


// dump current temperature - raw ADC data
void pt_dump_raw (void)
{
  printf2 ("raw1: %05d raw2: %05d raw3: %05d\n", adc_raw[0], adc_raw[1], adc_raw[2]);
}

// dump current temperature - average ADC data
void pt_dump_avg (void)
{
  printf2 ("avg1: %05d avg2: %05d avg3: %05d\n", adc_avg[0], adc_avg[1], adc_avg[2]);
}

// get Pt100 error parameters from FLASH
void pt_get_error (void)
{
  flash_read_rtderr (rtd_err, PT_NUM);
}

// write Pt100 error parameters to FLASH
void pt_set_error (uint8_t num, int16_t err)
{
  rtd_err[num] = err;
  flash_write_rtderr (rtd_err, PT_NUM);
}

// dump Pt100 error parameters, working set stored in SRAM
void pt_dump_error (void)
{
  int32_t sign;

  for (int i = 0; i < PT_NUM; i++)
  {
    sign = (rtd_err[i] < 0) ? -1 : 1;
    printf2 ("E%d: %d.%02d\n", i, rtd_err[i]/100, sign*rtd_err[i]%100);
  }
}

// start temperature logging to terminal
void rtd_start_log (void)
{
  rtd_log = 1;
}

// stop temperature logging to terminal
void rtd_stop_log  (void)
{
  rtd_log = 0;
}
