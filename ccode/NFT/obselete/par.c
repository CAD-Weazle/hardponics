// file    : par.c
// author  : rb
// purpose : Kipp & Zonen PAR sensor interface
// date    : 221119
// last    : 221129
//
// note    : 4.57uV/umol/m^2.s / amplifier x400 -> 1.828mV/umol/m^2.s (real OPAMP gain ~405)

#include "includes.h"

// globals 
uint32_t par_raw;            // raw ADC PAR data
float    par_mol;            // calculated photon flow [umol/s.m^2]
int      par_log = 1;        // log data flag

// update PAR data
float par_read (void)
{
  par_raw = adc_read_par ();
  return (par_raw * PAR_LSB);

//par_uV   = 1000000 * ((VREFINT_VDDA * VREFINT_CAL_DAT * par_raw) / (Vrefint * ADC_FULLRANGE));
//par_mol2 = par_uV / (PAR_FACTOR * PAR_AMP_GAIN);
}

// dump PAR data
void par_dump (void)
{
  if (!par_log)
    return;

  printf2 ("PAR: %4.1f ", par_mol);
//printf2 ("\n");
}

// start PAR logging
void par_log_start (void)
{
  par_log = 1;
}

// stop PAR logging
void par_log_stop (void)
{
  par_log = 0;
}
