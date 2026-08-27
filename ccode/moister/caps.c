// file    : caps.c
// author  : rb
// purpose : STM32L1xx Touch IO port routines
// date    : 220304
// last    : 220815
//
// note    : sampling capacitor on PB5 - group 6
//           'SOIL0'  capacitor on PB6 - group 6
//           'SOIL1'  capacitor on PB7 - group 6
//           'SOIL2'  capacitor on PB4 - group 6 (patched from PB8)
//
// note    : charge transfer sequence:
// 
// State  Channel I/O configuration                 sampling capacitor I/O configuration
// 
//  #1    output push-pull low                      output push-pull low
//  #2    input floating                            input floating with hysteresis disabled 
//  #3    output push-pull high                     input floating with hysteresis disabled 
//  #4    input floating                            input floating with hysteresis disabled 
//  #5    input floating with analog switch closed  input floating with hysteresis disabled & analog switch closed
//  #6    input floating                            input floating with hysteresis disable

#include "includes.h"

// soil sensor GPIO mapping
int soil_gpio[NUM_SOIL_SENSORS] = {GPIO_CAP_SOIL0, GPIO_CAP_SOIL1, GPIO_CAP_SOIL2};

int soil_min[NUM_SOIL_SENSORS];
int soil_max[NUM_SOIL_SENSORS];

int moist[NUM_SOIL_SENSORS];

// setup GPIOs for Touch operation
void init_tcs (void)
{
  // enable Comparator interface clock to access RI registers
  RCC->APB1ENR |= RCC_APB1ENR_COMPEN;

  // no hysteresis on PB5 (& PB6, PB7, PB4???)
  RI->HYSCR1 = (1 << (GPIO_CAP_SAMPLE + 16))  // add 16 for hi-word bits
             | (1 << (GPIO_CAP_SOIL0  + 16))
             | (1 << (GPIO_CAP_SOIL1  + 16))
             | (1 << (GPIO_CAP_SOIL2  + 16));
 
  // clear all 
  gpio_clr (GPIOB, GPIO_CAP_SAMPLE);
  gpio_clr (GPIOB, GPIO_CAP_SOIL0);
  gpio_clr (GPIOB, GPIO_CAP_SOIL1);
  gpio_clr (GPIOB, GPIO_CAP_SOIL2);

  // ADC analog switches open!!! - st
//RI->ASCR1 &= (uint32_t)(~0x80000000); // ADC analog switches closed if the corresponding I/O switch is also closed (????)
  RI->ASCR1 = 0; // ADC analog switches closed if the corresponding I/O switch is also closed (????)
  RI->ASCR2 = 0; 

  // get min/max soil bounds
//tsc_get_bounds ();
//tsc_dump_bounds ();
}

// sampling capacitor discharge
void tsc_cs_discharge (void)
{
  // GPIO drive zero
  gpio_clr (GPIOB, GPIO_CAP_SAMPLE);

  // GPIO to output
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (GPIO_CAP_SAMPLE*2)); // clear bits
  tmp |=  (0b01 << (GPIO_CAP_SAMPLE*2)); // output

  GPIOB->MODER = tmp;
}

// sampling capacitor floating
void tsc_cs_float (void)
{
  // GPIO to input
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (GPIO_CAP_SAMPLE*2)); // input

  GPIOB->MODER = tmp;
}

// soil capacitor discharge
void tsc_ct_discharge (int num)
{
  // GPIO drive zero
  gpio_clr (GPIOB, soil_gpio[num]);

  // GPIO to output
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (soil_gpio[num]*2)); // clear bits
  tmp |=  (0b01 << (soil_gpio[num]*2)); // output

  GPIOB->MODER = tmp;
}

// soil capacitor charge
void tsc_ct_charge (int num)
{
  // GPIO drive one
  gpio_set (GPIOB, soil_gpio[num]);

  // GPIO to output
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (soil_gpio[num]*2)); // clear bits
  tmp |=  (0b01 << (soil_gpio[num]*2)); // output

  GPIOB->MODER = tmp;
}

// soil capacitor floating
void tsc_ct_float (int num)
{
  // GPIO to input
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (soil_gpio[num]*2));  // input

  GPIOB->MODER = tmp;
}

// adjust analog switches Routing Interface
void tcs_switches (int num)
{
  switch (num)
  {
    // select sampling cap & sensor SOIL0
    case 0:
    {
      RI->ASCR2 = (1 << 5) | (1 << 27);  // switches closed - PB5/PB6 (GRP6-2 / GR6-3)
      break;
    }

    // select sampling cap & sensor SOIL1
    case 1:
    {
      RI->ASCR2 = (1 << 5) | (1 << 28);  // switches closed - PB5/PB7 (GRP6-2 / GR6-4)
      break;
    }

    // select sampling cap & sensor SOIL2
    case 2:
    {
      RI->ASCR2 = (1 << 5) | (1 << 4);  // switches closed - PB5/PB4 (GRP6-2 / GR6-1)
      break;
    }

    default:	
       break;
  }
}

// read soil sensor
int soil_read (int sensor)
{
  int tcs_state = TCS_START;
  int cnt = 0;
 
  // sanity check
  if (sensor >= NUM_SOIL_SENSORS)
    return (TCS_ERR_SAMPLE);

  while (1)
  {
    switch (tcs_state)
    {
      // reset all
      case TCS_START:
      {
        // discharge soil sensor & sampling cap
        tsc_cs_discharge ();
        tsc_ct_discharge (sensor);
    
        // relax a bit
        usleep (50);

        // fall thru
      }
 
      // charge transfer from soil sensor to sampling cap  
      case TCS_SAMPLE:
      {
        // float inputs sampling cap & soil sensor
        tsc_cs_float ();
        tsc_ct_float (sensor);
    
        // relax a bit
        usleep (10);
    
        // charge soil sensor & let float
        tsc_ct_charge (sensor);
        tsc_ct_float (sensor);   

        // set Routing Interface to transfer charge from soil sensor to sampling cap
        tcs_switches (sensor);

        // why no delay here?

        // all Routing Interface switches open, stop charge transfer
        RI->ASCR2 = 0;    

        // check if GPIO input level == '1'
        if (GPIOB->IDR & (1 << GPIO_CAP_SAMPLE))
        {
          return (cnt);                // return sample count & bail  
        }
        else
        {
          // check for time-out
          if (cnt++ > TCS_MAX_SAMPLE)
            return (TCS_ERR_SAMPLE);   // return error count & bail
          else
            tcs_state = TCS_SAMPLE;    // next sample
        }
        break;
      }
    
      default:
        break;
    }
  }
}

// read soil sensors
void soil_update (void)
{
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
    moist[i] = soil_read (i);
}

// dump soil sensors
void soil_dump (void)
{
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    // scale to 0..100%
    float tmp = 100.0*( (float)(moist[i] - soil_min[i]) / (float)(soil_max[i] - soil_min[i]) );

    // sanity check
    if (tmp < 0.0)
      tmp = 0.0;

    if (tmp > 100.0)
      tmp = 100.0;

    printf2 ("%3.1f ", tmp);
  }

  printf2 ("\n");
}

// dump soil sensors - raw data
void soil_dump_raw (void)
{
  printf2 ("SOIL: ");

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    printf2 ("%3.1f ", (float)moist[i]);
  }

  printf2 ("\n");
}

// dump soil sensors to BLE module - <> raw data for now, 0..100 scale later
void soil_dump_ble (void)
{
  printf3 ("SOIL: ");

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    printf3 ("%3.1f ", (float)moist[i]);
  }
}


// read sensor bound parameters from EEPROM
void tsc_get_bounds (void)
{
  uint32_t adr = EEPROM_BASE;

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  { 
    soil_min[i] = eeprom_read_long (adr);

    adr += 4;
  }

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    soil_max[i] = eeprom_read_long (adr);

    adr += 4;
  }
}

// write sensor bound parameters te EEPROM
void tsc_set_bounds (int num, int err)
{
  uint32_t adr = EEPROM_BASE + num*4;
  
  eeprom_write_long (adr, err);
  
  // update global state
  tsc_get_bounds ();
}

// dump soil sensor min/max bounds
void tsc_dump_bounds (void)
{
  printf2 ("# -- Soil Sensor Bounds --\n#\n");
   
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    printf2 ("# sensor%d: %d %d\n", i, soil_min[i], soil_max[i]);
  } 
}

// set sensors lower bound
void tcs_set_min (void)
{
  int tmp = 0;

  for (int sensor = 0; sensor < NUM_SOIL_SENSORS; sensor++)
  {
    // average a few samples
    for (int i = 0; i < 16; i++)
      tmp += soil_read (sensor);
    
    // set lower bound       
    tsc_set_bounds (sensor, tmp/16);

    tmp = 0;
  }
}

// set sensors upper bound
void tcs_set_max (void)
{
  int tmp = 0;

  for (int sensor = 0; sensor < NUM_SOIL_SENSORS; sensor++)
  {
    // average a few samples
    for (int i = 0; i < 16; i++)
      tmp += soil_read (sensor);
    
    // set upper bound       
    tsc_set_bounds (sensor + NUM_SOIL_SENSORS, tmp/16);

    tmp = 0;
  }
}





