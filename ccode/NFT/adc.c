// file    : adc.c
// author  : rb
// purpose : STM32L1xx ARM ADC routines
// date    : 171130
// last    : 230315
//
// note    : VREFINT_CAL Raw data acquired at temperature of 30 °C ±5 °C VDDA= 3V ±10 mV x1FF800F8 - 0x1FF800F9
//           is used for ADC-to-voltage calculation

#include "includes.h"

// globals
float Vrefint = 1511.0;      // default Vrefint [ADC counts]

// data ADC channels
int ntc_raw;
int vbat_raw;
int vsol_raw;

int par_raw;
int par_old = 0;             // filter state
int par_avg;                 // ADC averaged data

int ibat_raw;
int ibat_old = 0;            // filter state
int ibat_avg;                // ADC averaged data

// init ADC1
void init_adc (void) 
{ 
  // enable clock for ADC1
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  // set HSI divider 
  ADC->CCR = (0b00 << ADC_CCR_ADCPRE_Pos);       // ADC clock = HSI/1 (16/1 = 16Mc)

  // first get Vrefint before setting up scan sequence & stuff (used for ADC-to-voltage calculations)
  Vrefint = adc_read_vref ();

  // set ADC sample time
  // * channel sample times:
  // 0b000 =   3 cycles
  // 0b001 =  15 cycles
  // 0b010 =  28 cycles
  // 0b011 =  56 cycles
  // 0b100 =  84 cycles
  // 0b101 = 112 cycles
  // 0b110 = 144 cycles
  // 0b111 = 480 cycles
  ADC1->SMPR1 = (0b110 <<  0) |  // channel 20 -- VSOL (PB14)
                (0b110 <<  3) |  // channel 21
                (0b010 <<  6) |  // channel 22
                (0b010 <<  9) |  // channel 23
                (0b010 << 12) |  // channel 24
                (0b010 << 15) |  // channel 25
                (0b010 << 18) |  // channel 26
                (0b010 << 21) |  // channel 27
                (0b010 << 24) |  // channel 28
                (0b010 << 27);   // channel 29

  ADC1->SMPR2 = (0b010 <<  0) |  // channel 10
                (0b010 <<  3) |  // channel 11
                (0b010 <<  6) |  // channel 12
                (0b010 <<  9) |  // channel 13
                (0b010 << 12) |  // channel 14
                (0b010 << 15) |  // channel 15
                (0b010 << 18) |  // channel 16
                (0b110 << 21) |  // channel 17 -- Vrefint 
                (0b010 << 24) |  // channel 18 
                (0b010 << 27);   // channel 19 

  ADC1->SMPR3 = (0b110 <<  0) |  // channel 0  -- PAR  (PA0)
                (0b110 <<  3) |  // channel 1  -- EC   (PA1)
                (0b010 <<  6) |  // channel 2
                (0b010 <<  9) |  // channel 3
                (0b010 << 12) |  // channel 4
                (0b010 << 15) |  // channel 5
                (0b010 << 18) |  // channel 6  -- NTC  (PA6)
                (0b010 << 21) |  // channel 7
                (0b010 << 24) |  // channel 8  -- VBAT (PB0)
                (0b010 << 27);   // channel 9  -- IBAT (PB1) - noisy AF

  // setup scan sequence length & scan order regular channels 
  ADC1->SQR1 = (0 << ADC_SQR1_L_Pos);            // sequence length
  ADC1->SQR2 = 0;                                          
  ADC1->SQR3 = 0;                                          
  ADC1->SQR4 = 0;                                          
  ADC1->SQR5 = ADC_CHAN_VREF;
  
  // set EOC flag behavior
  ADC1->CR2 |= ADC_CR2_EOCS;                     // set EOC flag after every conversion

  // get Vrefint
  Vrefint = adc_read_vref ();
}

// read ADC channel
uint32_t adc_read_channel (uint8_t chan)
{
  // sequence length 
  ADC1->SQR1 = (0 << 20);                        // read single channel

  // set single ADC channel sequence
  ADC1->SQR5 = (chan << 0);   

  // enable ADC
  ADC1->CR2 |= ADC_CR2_ADON;

  // wait for ADC ready & Regular Channel ready  <> add this to older 'adc.c' source
  while (((ADC1->SR & ADC_SR_ADONS) == 0) || ((ADC1->SR & ADC_SR_RCNR) != 0))
    ;  

  // start conversion Regular Channel
  ADC1->CR2 |= ADC_CR2_SWSTART;

  // wait for conversion ready, check EOC flag (called EOCS for some reason, WTF?)
  // note: hangs with leaving stop mode (fix this)
  while ((ADC1->SR & ADC_SR_EOCS) == 0)
    ;  

  // disable ADC to save power
  ADC1->CR2 &= ~ADC_CR2_ADON;

  // read result
  return (ADC1->DR);
}

// read internal reference voltage VREFINT
float adc_read_vref (void)
{
  uint32_t dat = 0;

  // sample VREFINT a few times
  for (int i = 0; i < ADC_NUM_REFSAMPLES; i++)
  {
    dat += adc_read_channel (ADC_CHAN_VREF);
  }

  // return average
  return ((float)dat / ADC_NUM_REFSAMPLES);
}

// read all ADC channels & filter if needed
void adc_update (void)
{
  static int cnt = 0;
  static int first = 1;

  // no noise filter needed, read once per second
  if ((cnt++ >= 100) || (first == 1))
  {
    // connect voltage dividers to GND (VSOL/VACCU)
    gnd_connect ();
    msleep (20);  // relax a vit, let analog signals settle

//  par_raw  = adc_read_channel (ADC_CHAN_PAR);   // PAR light sensor
    ntc_raw  = adc_read_channel (ADC_CHAN_NTC);   // NTC sensor for EC sensor
    vbat_raw = adc_read_channel (ADC_CHAN_VBAT);  // battery voltage
    vsol_raw = adc_read_channel (ADC_CHAN_VSOL);  // solar panel voltage

    // disconnect voltage dividers from GND (VSOL/VACCU)
    gnd_disconnect ();

    cnt = 0;
  }

  // read battery current sensor sign bit first
  int sign;

  if (GPIOC->IDR & (1 << IBAT_SIGN))
    sign = 1; // postive: battery is charging
  else
    sign = -1;// negative: battery is discharging

  // IBAT noisy AF, filter needed
  ibat_raw = sign * adc_read_channel (ADC_CHAN_IBAT);

  // PAR noisy, needs filter?
  par_raw  = adc_read_channel (ADC_CHAN_PAR);   // PAR light sensor

  // speed up filter 
  if (first)
  {
    par_old = par_raw;
    ibat_old = ibat_raw;
    first = 0;
  }

  // exponential filter: Xn = (1-a).Xn-1 + a.New (a = 1/ADC_EXP_DEPTH) 
  par_avg = ((ADC_EXP_DEPTH_PAR-1)*par_old + par_raw)/ADC_EXP_DEPTH_PAR;

  // exponential filter: Xn = (1-a).Xn-1 + a.New (a = 1/ADC_EXP_DEPTH) 
  ibat_avg = ((ADC_EXP_DEPTH-1)*ibat_old + ibat_raw)/ADC_EXP_DEPTH;

  // update state
  par_old = par_avg;   
  ibat_old = ibat_avg;   
}

// read battery voltage 
float adc_read_vbat (void)
{
  // voltage divider, multiply result by factor
  return (4.0 * (VREFINT_VDDA*VREFINT_CAL_DAT * vbat_raw) / (Vrefint*ADC_FULLRANGE));
}

// read battery current
float adc_read_ibat (void)
{
  return (1000 * (VREFINT_VDDA*VREFINT_CAL_DAT*ibat_avg) / (Vrefint*ADC_FULLRANGE));
}

// read solar panel voltage
float adc_read_vsol (void)
{
  // voltage divider, multiply result by factor
  return (10.0908 * (VREFINT_VDDA*VREFINT_CAL_DAT * vsol_raw) / (Vrefint*ADC_FULLRANGE));
}

// read ADC channel PAR sensor
uint16_t adc_read_par (void)
{
//return (par_raw * PAR_LSB);
  return (par_avg * PAR_LSB);
}

// read ADC channel NTC of EC sensor
uint16_t adc_read_ntc (void)
{
  return (ntc_raw);
}

// dump ADC registers -- dev only
void adc_dump_registers (void)
{
  printf2 ("-- ADC registers --\n");
  printf2 ("ADC1_SR    : %08lx\n", ADC1->SR);
  printf2 ("ADC1_CR1   : %08lx\n", ADC1->CR1);
  printf2 ("ADC1_CR2   : %08lx\n", ADC1->CR2);
  printf2 ("ADC1_SMPR1 : %08lx\n", ADC1->SMPR1);
  printf2 ("ADC1_SMPR2 : %08lx\n", ADC1->SMPR2);
  printf2 ("ADC1_SMPR3 : %08lx\n", ADC1->SMPR3);
  printf2 ("ADC1_SQR1  : %08lx\n", ADC1->SQR1);
  printf2 ("ADC1_SQR2  : %08lx\n", ADC1->SQR2);
  printf2 ("ADC1_SQR3  : %08lx\n", ADC1->SQR3);
  printf2 ("ADC1_SQR4  : %08lx\n", ADC1->SQR4);
  printf2 ("ADC1_SQR5  : %08lx\n", ADC1->SQR5);
  printf2 ("ADC1_DR    : %08lx\n", ADC1->DR);
}
