// file    : ec.c
// author  : rb
// purpose : SJW Generator (Single-Jolt Wave Generator) EC meter routines
// date    : 190502
// last    : 230114
//
// note    : I/U converter OPAMP is working to get I_cell == I_Rf (Rf = feedback resistor of 500R)
//
//           * V_cell/R_cell = V_adc/Rf 
//            
//           * R_cell = V_cell * Rf / V_adc           ; Vcell = 2*EC_SINE_AMP / V_adc is output I/U OPAMP
//                                                    
//           * R_cell = 2*EC_SINE_AMP * Rf / V_adc    ; Rf = 500R
//            
//           * R_cell = 1000 * EC_SINE_AMP / V_adc
//            
//           * S_cell = V_adc / 1000 * EC_SINE_AMP [S]
//            
//           * S_cell = V_adc / EC_SINE_AMP [mS]
//
// * calibration with resistors:
//
// Rcell [R]       Scell [mS]
// (fixed R)  (calc'd) (measured)
//  992.5      1.008     1.028
//  468.1      2.136     2.175
//  327.6      3.052     3.060
//  153.7      6.506     6.635 
//   82.6     12.106    12.345   (3.135mS with 12.88mS calibration solution)

#include "includes.h"

// storage excitation wave DAC/ADC
int16_t dac_tab[EC_NUM_DOTS];                    // excitation sine wave for DAC
int16_t adc_tab[EC_NUM_DOTS];                    // response from EC sensor ADC

uint16_t ec_ftab[EC_FILTERDEPTH];                // storage for moving average filter
                                                 
// EC calculation                                
uint16_t ec_raw;                                 // raw data read from EC sensor
float    ec_avg;                                 // average value after filtering

float c_temp;                                    // EC NTC sensor temperature 
float c_raw;                                     // EC calculated [mS]
float c_adj;                                     // EC adjusted for cell constant [mS]
float c_cell;                                    // EC final result with added temperature compensation [uS]

// cell constant
float c_const;           

// flags
uint8_t ec_cal_busy = 0;                         // 0: normal operation / 1: calibration busy
uint8_t ec_log = 1;                              // log EC data to serial port
uint8_t first = 2;                               // speed up filter flag

// init EC
void init_ec (void)
{
  // apply power to EC driver & NTC
  ec_power_on ();

  // read cell constant from EEPROM
  c_const = eeprom_read_float (EEPROM_BASE_EC);

  // sanity check cell constant
  if (c_const == 0.0)
  {
    c_const = EC_CELL_DEFAULT;

    printf2 ("#apply default EC cell constant, calibration needed\n");
    printf3 (">apply default EC cell constant, calibration needed\n");
  }

  // fill sine table for excitation wave
  for (int i = 0; i < EC_NUM_DOTS; i++)
    dac_tab[i] = (int16_t)(EC_SINE_AMP * sin (2*PI*(float)i/EC_NUM_DOTS));   
}

// handle EC sensor
void ec_update (void)
{
  if (ec_cal_busy)
  {
    // calibrate EC sensor against standard solution
    ec_calibrate ();
  }
  else
  {
    // get actual EC sensor temperature
    ntc_update ();

    // read conductivity
    ec_get ();
  }
}

// excite & read back EC probe
void ec_get (void)
{
  // get actual EC sensor temperature
  c_temp = 25.0;

  // excite EC sensor
  ec_sjw ();

  // calculate response (Vtt [mV]) & filter
  ec_raw = ec_peak_detect ();
           
  ec_avg = ec_filter (ec_raw);

  // calculate EC cell conductivity [mS]
  c_raw = ec_avg / EC_SINE_AMP;  

  // adjust for cell constant 
  c_adj = c_raw * c_const;
  
  // apply temperature compensation [note: C_25 = C_t / (1 + alfa*(t-25))]
  c_cell = c_adj * (1 / (1 + EC_ALFA*(c_temp - 25.0)));
//c_cell = c_adj * / (1 + EC_ALFA*(c_temp - 25.0));   // <> try this
}

// excite cell with single sine wave & read back response
void ec_sjw (void)
{
  // get actual EC sensor temperature
  c_temp = ntc_get ();

  // generate sine wave & measure cell response
  for (int cycle = 0; cycle < EC_NUM_CYCLES; cycle++)
  {
    // inject oneven number of cycles, measure middle one only to cancel possible edge effects
    for (int i = 0; i < EC_NUM_DOTS; i++)
    {
      dac1_write (DAC_MAX/2 + dac_tab[i] - EC_DAC_TWEAK);  // inject signal in cell (tweak for 3V3 and 3V0 VDD rail mismatch)

      if (cycle == EC_NUM_CYCLES/2)
      {
      //adc_tab[i] = adc_read_ec ();                       // read response of middle cycle only
        adc_tab[i] = adc_read_channel (ADC_CHAN_EC);       // read response of middle cycle only
      }
      else
      //(void) adc_read_ec ();                             // dummy read for correct timing excitation signal
        (void) adc_read_channel (ADC_CHAN_EC);

      // delay a bit
      usleep (EC_WAVE_DELAY);
    }
  }
}

// determine min/max value EC response
uint16_t ec_peak_detect (void)
{
  uint16_t adc_max = 0;  
  uint16_t adc_min = ADC_FULLRANGE;  

  // find min/max
  for (int i = 0; i < EC_NUM_DOTS; i++)
  {
    if (adc_tab[i] >= adc_max)
      adc_max = adc_tab[i];

    if (adc_tab[i] <= adc_min)
      adc_min = adc_tab[i];
  }

  // calculate amplitude 
  return (adc_max - adc_min);
}

// naive moving average filter
float ec_filter (uint16_t dat)
{
  float sum = 0.0;

  // speed up filter 
  if (first)
  {
    for (int i = 0; i < EC_FILTERDEPTH; i++)
      ec_ftab[i] = dat;

    // delay a few samples to counter power-up transients
    first--;
  }
  else
  {
    for (int i = EC_FILTERDEPTH-1; i > 0; i--)
      ec_ftab[i] = ec_ftab[i-1];
  
    ec_ftab[0] = dat;
  }

  for (int i = 0; i < EC_FILTERDEPTH; i++)
    sum += ec_ftab[i];

  return (sum/EC_FILTERDEPTH);
}

// -- calibration routines
uint8_t ec_cur_sensor;                      // sensor being calibrated

// start calibration process
void ec_cal_sensor (void)
{
  ec_cal_busy = 1;
}

// calibrate EC sensor
void ec_calibrate (void)
{
  // flags
  static uint8_t  cal_state = EC_CAL_IDLE;  // FSM state
  static uint16_t cal_wait = 0;             // calibration waiting time counter
  
  // EC sensor calibration state machine
  switch (cal_state)
  {
    // -------- INITIAL PART ------------
    // check for calibration in progress, else bail
    case EC_CAL_IDLE:
    {
      // signal calibration busy
      led_red_on ();                    

      // stop log output 
      log_stop ();                      

      cal_state = EC_CAL_START;
      break;
    }

    // signal user calibration has started
    case EC_CAL_START:
    {
      printf2 ("#start EC sensor calibration\n");  // UART
      printf3 (">start EC sensor calibration\n");  // AP/PuTTY

      // set speed up filter flag
      first = 1;

      cal_state = EC_CAL_INSERT;
      break;
    }


    // -------- EC SENSOR CALIBRATION PART ------------

    // put EC sensor in calibration liquid with EC 3.00 [mS]
    case EC_CAL_INSERT:
    {
      printf2 ("#put EC sensor in 3.00 [mS] calibration solution & wait\n");
      printf3 (">put EC sensor in 3.00 [mS] calibration solution & wait\n");
     
      cal_state = EC_CAL_BUSY;
      break;
    }

    // & wait some time to reach equilibrium
    case EC_CAL_BUSY:
    {
      // signal calibration busy
      led_red_toggle ();

      // excite EC sensor
      ec_sjw ();

      // calculate response & filter
      ec_raw = ec_peak_detect ();  // calculate Vtt
      ec_avg = ec_filter (ec_raw); // apply moving average filter

      // calculate EC cell conductivity [mS]
      c_raw = (float)ec_avg / EC_SINE_AMP;  

      // assume unity cell constant 
      c_adj = c_raw * 1.0000;

      // apply temperature compensation [note: C_25 = C_t / (1 + alfa*(t-25))]
      c_cell = c_adj * (1 / (1 + EC_ALFA*(c_temp - 25.0)));

      // count down waiting time & display raw / average ADC value
      if (!(cal_wait % 10)) 
      {
        printf2 ("#EC sensor calibration busy, please wait %2d - T: %2.2f - measured: %2.2f target: %2.2f\n",  
                  (EC_CAL_WAIT - cal_wait)/10, c_temp, c_adj, 3.00);

        printf3 (">EC sensor calibration busy, please wait %2d - T: %2.2f - measured: %2.2f target: %2.2f\n",  
                  (EC_CAL_WAIT - cal_wait)/10, c_temp, c_adj, 3.00);
      }

      // wait some time for accurate calibration 
      if (cal_wait >= EC_CAL_WAIT)
      {
        float ec_cal_res = 3.000/c_cell;

        printf2 ("#EC sensor calibration factor: %1.8f\n", ec_cal_res);      
        printf3 (">EC sensor calibration factor: %1.8f\n", ec_cal_res);      

        // update EEPROM & adjust state
        eeprom_write_float (EEPROM_BASE_EC, ec_cal_res);

        c_const = ec_cal_res;

        cal_wait = 0;       
        cal_state = EC_CAL_READY;
      }
      else
        cal_wait++;

      break;
    }

    // -------- FINAL PART ------------

    // calibration done
    case EC_CAL_READY:
    {
      // signal calibration ready & dump calibration data
      printf2 ("#EC sensor calibration done\n");
      printf3 (">EC sensor calibration done\n");

      // adjust state
      ec_cal_busy = 0;            // clear busy flag, commence normal operation
      cal_wait = 0;               // reset calibration wait counter

      // signal end off calibration
      led_red_off  ();      

      // resume logging
    //log_start ();          

      cal_state = EC_CAL_IDLE; 
      break;
    }

    default:
      break;
  }
}

// -- dumping & logging

// log EC sensor data to serial port
void ec_dump (void)
{
  if (!ec_log)
    return;

  printf2 ("T_EC: %2.2f ", c_temp);
//printf2 ("Craw[mS]: %4.2f ", c_raw);
//printf2 ("Cadj[mS]: %4.2f ", c_adj);
  printf2 ("EC: %2.2f", c_cell);

  printf2 ("\n");
}

// get EC sensor temperature
float ec_get_t (void)
{
  return (c_temp);
}

// get EC sensor conductivity
float ec_get_e (void)
{
  return (c_cell);
}

// start EC logging
void ec_log_start (void)
{
  ec_log = 1;
}

// stop EC logging
void ec_log_stop (void)
{
  ec_log = 0;
}

// dump DAC & ADC data
void ec_dump_tab (void)
{
  for (int i = 0; i < EC_NUM_DOTS; i++)
    printf2 ("DAC: %d ADC: %d\n", dac_tab[i], adc_tab[i]);
}
