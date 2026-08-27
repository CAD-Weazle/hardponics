// file    : adc.c
// author  : rb
// purpose : ST32L1xx ARM ADC routines
// date    : 171130
// last    : 220801
//
// note    : VREFINT_CAL Raw data acquired at temperature of 30 °C ±5 °C VDDA= 3 V ±10 mV x1FF800F8 - 0x1FF800F9
//           is used for ADC-to-voltage calculation
// 
// note    : reading Vbat/Vsol/Vrefint using Injected Channels gives more noisy data (???)

#include "includes.h"

//#define ADC_DEBUG

// globals
uint32_t ntc_dat[ADC_NUM_RCHANNELS];             // storage for DMA NTC data
float    ntc_fil[ADC_NUM_NTC];                   // median filtered NTC data

uint32_t vrefint;                                // internal reference voltage

float vbat = 0.0;                                // battery voltage
float vsol = 0.0;                                // solar cell voltage
float vdda = 0.0;                                // VDDA

// init ADC1
void init_adc (void) 
{
  // enable clock for ADC1
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  // set HSI divider 
//ADC->CCR = (0b10 << ADC_CCR_ADCPRE_Pos);       // ADC clock = HSI/4 (4 Mc)  - total conversion time = 275us
  ADC->CCR = (0b00 << ADC_CCR_ADCPRE_Pos);       // ADC clock = HSI/1 (16 Mc) - total conversion time = 75us

  // disable ADC before writing SMPRx registers (clear ADON)
  ADC1->CR2 = 0;                                 

  // set sample time (ADON must be 0)
  // * channel sample times:
  // 0b000 =   4 cycles
  // 0b001 =   9 cycles
  // 0b010 =  16 cycles
  // 0b011 =  24 cycles
  // 0b100 =  48 cycles
  // 0b101 =  96 cycles
  // 0b110 = 192 cycles
  // 0b111 = 384 cycles
  ADC1->SMPR1 = (0b111 <<  0) |  // channel 20 -- Vsolar (PB14)
                (0b010 <<  3) |  // channel 21
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
                (0b111 << 21) |  // channel 17 -- Vrefint 
                (0b010 << 24) |  // channel 18 
                (0b111 << 27);   // channel 19 -- Vbat (PB13)

  ADC1->SMPR3 = (0b010 << 27)    // channel 9
              | (0b010 << 24)    // channel 8
              | (0b011 << 21)    // channel 7  -- NTC3 (PA7)
              | (0b011 << 18)    // channel 6  -- NTC2 (PA6)
              | (0b011 << 15)    // channel 5  -- NTC1 (PA5)
              | (0b011 << 12)    // channel 4  -- NTC0 (PA4)
              | (0b010 <<  9)    // channel 3
              | (0b010 <<  6)    // channel 2
              | (0b010 <<  3)    // channel 1
              | (0b010 <<  0);   // channel 0

  // setup scan sequence length & scan order Regular Channels
  ADC1->SQR1 = ((ADC_NUM_RCHANNELS-1) << 20)     // sequence length
             | (ADC_CHAN_NTC3 << 15)             // NTC3
             | (ADC_CHAN_NTC3 << 10)
             | (ADC_CHAN_NTC3 <<  5)
             | (ADC_CHAN_NTC3 <<  0);

  ADC1->SQR2 = (ADC_CHAN_NTC3 << 25)
             | (ADC_CHAN_NTC3 << 20)
             | (ADC_CHAN_NTC3 << 15)
             | (ADC_CHAN_NTC2 << 10)             // NTC2
             | (ADC_CHAN_NTC2 <<  5)
             | (ADC_CHAN_NTC2 <<  0);

  ADC1->SQR3 = (ADC_CHAN_NTC2 << 25)
             | (ADC_CHAN_NTC2 << 20)
             | (ADC_CHAN_NTC2 << 15)
             | (ADC_CHAN_NTC2 << 10)
             | (ADC_CHAN_NTC1 <<  5)             // NTC1
             | (ADC_CHAN_NTC1 <<  0);

  ADC1->SQR4 = (ADC_CHAN_NTC1 << 25)
             | (ADC_CHAN_NTC1 << 20)
             | (ADC_CHAN_NTC1 << 15)
             | (ADC_CHAN_NTC1 << 10)                                
             | (ADC_CHAN_NTC1 <<  5)
             | (ADC_CHAN_NTC0 <<  0);            // NTC0

  ADC1->SQR5 = (ADC_CHAN_NTC0 << 25)
             | (ADC_CHAN_NTC0 << 20)
             | (ADC_CHAN_NTC0 << 15)
             | (ADC_CHAN_NTC0 << 10)
             | (ADC_CHAN_NTC0 <<  5)
             | (ADC_CHAN_NTC0 <<  0);

  // setup DMA channel
  init_dma_adc (ntc_dat, ADC_NUM_RCHANNELS);

#ifdef ADC_DEBUG
  adc_dump_registers ();
#endif
}

// update ADC channels
void adc_update (void)
{ 
  // read board voltages
  adc_read_supply ();
  
  // delay reduces noise
  msleep (100);
  
  // read NTC sensors
  adc_read_ntc_new ();
}

// read battery, solar cell & analog supply voltage
void adc_read_supply (void)
{
  uint32_t vref_accu = 0;
  uint32_t vbat_accu = 0;
  uint32_t vsol_accu = 0;

  // enable Vrefint (seems not needed?)
  vrefint_enable ();
  
  // read Vrefint (used for ADC-to-voltage calculations)
  for (int i = 0; i < ADC_FILTER_DEPTH; i++)
  {
    vref_accu += adc_read_channel (ADC_CHAN_VREF);
  
    // relax a bit to reduce noise
    usleep (5);
  }
  
  // disable Vrefint <> needed?
  vrefint_disable ();

  // enable GND connection voltage divider - measure Vbat & Vsol
  vgnd_enable (); 
  
  // let voltage settle
  msleep (5);
  
  // take samples
  for (int i = 0; i < ADC_FILTER_DEPTH; i++)
    vbat_accu += adc_read_channel (ADC_CHAN_VBAT);
  
  for (int i = 0; i < ADC_FILTER_DEPTH; i++)
    vsol_accu += adc_read_channel (ADC_CHAN_VSOL);
  
  // disable GND connection voltage divider - leave floating
  vgnd_disable (); 
  
  // calculate Vrefint
  vrefint = vref_accu/ADC_FILTER_DEPTH;
  
  // calculate average Vbat & Vsol
  float vbat_avg = (float)vbat_accu/ADC_FILTER_DEPTH;
  float vsol_avg = (float)vsol_accu/ADC_FILTER_DEPTH;
  
  // convert ADC value to Vbat & Vsol (use VREFINT calibration data + actual ADC read-out data)
  // note: 12-bit ADC data, voltage divider = 5M6/5M6 -> factor 2.0
  vbat = (2.0 * (VREFINT_VDDA*VREFINT_CAL_DAT*vbat_avg) / ((float)vrefint*ADC_FULLRANGE));
  vsol = (2.0 * (VREFINT_VDDA*VREFINT_CAL_DAT*vsol_avg) / ((float)vrefint*ADC_FULLRANGE));
  
  // calculate VDDA (as per Users Manual)
  vdda = ((3.0 * VREFINT_CAL_DAT) / vrefint);
}

// read NTC sensors using DMA - stalls!!!
void adc_read_ntc (void)
{
  // setup scan sequence length & scan order Regular Channels
  ADC1->SQR1 = ((ADC_NUM_RCHANNELS-1) << 20)     // sequence length
             | (ADC_CHAN_NTC3 << 15)             // NTC3
             | (ADC_CHAN_NTC3 << 10)
             | (ADC_CHAN_NTC3 <<  5)
             | (ADC_CHAN_NTC3 <<  0);

  ADC1->SQR5 = (ADC_CHAN_NTC0 << 25)             // NTC0
             | (ADC_CHAN_NTC0 << 20)
             | (ADC_CHAN_NTC0 << 15)
             | (ADC_CHAN_NTC0 << 10)
             | (ADC_CHAN_NTC0 <<  5)
             | (ADC_CHAN_NTC0 <<  0);

  // set ADC scan mode & resolution
  ADC1->CR1 = (0b00 << ADC_CR1_RES_Pos)          // 12-bit mode
            | ADC_CR1_SCAN;                      // enable channel scanning
  
  // setup DMA & enable DMA                                    
  ADC1->CR2 = (0b001 << ADC_CR2_DELS_Pos)        // delay after conversion <> test
            | ADC_CR2_DMA;                       // enable DMA
          //| ADC_CR2_DDS;                       // keep DMA going on new data <> test

  // 0b000: stalls after long time
  // 0b001: stalls after long time
  // 0b010: not running
  // 0b011: not running
  // 0b100: not running

  // enable ADC
  ADC1->CR2 |= ADC_CR2_ADON;

  // wait for ADC ready & Regular Channel ready
  while (((ADC1->SR & ADC_SR_ADONS) == 0) || ((ADC1->SR & ADC_SR_RCNR) != 0))
    ;  

  // start conversion Regular Channel
  ADC1->CR2 |= ADC_CR2_SWSTART;

  // wait for conversion ready, check EOC flag (called EOCS for some reason, WTF?)
  // note: hangs with leaving stop mode (fix this)
  while ((ADC1->SR & ADC_SR_EOCS) == 0)
    ;  
  
  // wait for DMA transfer complete
  while ((DMA1->ISR & DMA_ISR_TCIF1) == 0)   
    usleep (10);
  
  // clear DMA1 Chanel1 Transfer Complete flag 
  DMA1->IFCR |= DMA_ISR_TCIF1;                   // write 1 to reset flag

  // disable ADC to save power & disable DMA                                    
  ADC1->CR2 = 0;

  // filter NTC sensors data
  ntc_fil[0] = adc_median (&ntc_dat[0*ADC_MEDIAN_DEPTH]);
  ntc_fil[1] = adc_median (&ntc_dat[1*ADC_MEDIAN_DEPTH]);
  ntc_fil[2] = adc_median (&ntc_dat[2*ADC_MEDIAN_DEPTH]);
  ntc_fil[3] = adc_median (&ntc_dat[3*ADC_MEDIAN_DEPTH]);
}

// read NTC sensors wo using DMA - under test now
void adc_read_ntc_new (void)
{
  // apply power to NTC sensors
  ntc_enable ();

  // relax a bit
  msleep (5);

  for (int i = 0; i < ADC_MEDIAN_DEPTH; i++)
  {
    ntc_dat[i + 0*ADC_MEDIAN_DEPTH] = adc_read_channel (ADC_CHAN_NTC0);
    ntc_dat[i + 1*ADC_MEDIAN_DEPTH] = adc_read_channel (ADC_CHAN_NTC1);
    ntc_dat[i + 2*ADC_MEDIAN_DEPTH] = adc_read_channel (ADC_CHAN_NTC2);
    ntc_dat[i + 3*ADC_MEDIAN_DEPTH] = adc_read_channel (ADC_CHAN_NTC3);
  }

  // remove power from NTC sensors
  ntc_disable ();

  // filter NTC sensors data
  ntc_fil[0] = adc_median (&ntc_dat[0*ADC_MEDIAN_DEPTH]);
  ntc_fil[1] = adc_median (&ntc_dat[1*ADC_MEDIAN_DEPTH]);
  ntc_fil[2] = adc_median (&ntc_dat[2*ADC_MEDIAN_DEPTH]);
  ntc_fil[3] = adc_median (&ntc_dat[3*ADC_MEDIAN_DEPTH]);
}

// read ADC channel
uint32_t adc_read_channel (uint8_t chan)
{
  // sequence length 
  ADC1->SQR1 = (1 << 20);                        // read single channel

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

// enable Vrefint
void vrefint_enable (void)
{
  ADC->CCR |= ADC_CCR_TSVREFE;

  // relax a bit, let Vrefint settle
  msleep (5);
}

// disable Vrefint, save power during Stop mode
void vrefint_disable (void)
{
  ADC->CCR &= ~ADC_CCR_TSVREFE;
}

// dump ADC channels converted
void adc_dump (void)
{
  printf2 ("VDDA: %1.2f Vsol: %1.2f Vbat: %1.2f  ", vdda, vsol, vbat); 
}

// dump ADC data to BLE module
void adc_dump_ble (void)
{
//printf3 ("\E[35mVDDA: %1.2f Vsol: %1.2f Vbat: %1.2f Vamb: %1.2f \E[30m\n", vdda, vsol, vbat, vamb); 
  printf3 ("VDDA: %1.2f Vsol: %1.2f Vbat: %1.2f  ", vdda, vsol, vbat); 
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
  printf2 ("ADC_CSR    : %08lx\n", ADC->CSR);
  printf2 ("ADC_CCR    : %08lx\n", ADC->CCR);
}

// -- median filter routines

// swap two variables for median filter
inline void swap (uint32_t *a, uint32_t *b)
{
  uint32_t temp = *a;
  *a = *b;
  *b = temp;
}

// median filter (odd number of array elements only please)
float adc_median (uint32_t *buf)
{
  uint32_t tmp[ADC_MEDIAN_DEPTH];

  // copy buffer to tmp
  for (int i = 0; i < ADC_MEDIAN_DEPTH; i++)
    tmp[i] = buf[i];    

  // sort tmp buffer
  for (int i = 0; i < ADC_MEDIAN_DEPTH-1; i++) 
  {
    for (int j = 0; j < ADC_MEDIAN_DEPTH-i-1; j++) 
    {
      if (tmp[j] > tmp[j+1])
      {
        swap (&tmp[j], &tmp[j+1]);
      }
    }
  }

  // return median
  return ((float)tmp[(ADC_MEDIAN_DEPTH-1)/2]);
}
