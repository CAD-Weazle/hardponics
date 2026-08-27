// file    : caps.c
// author  : rb
// purpose : STM32L1xx Touch IO port routines
// date    : 220304
// last    : 220304
//
// note    : sampling capacitor on PA0 - group 1
//           'touch' capacitor on PA1  - group 1

#define SAMPLE_NUM 2000

#include "includes.h"

// GPIO bit fields
//static const struct port_info
//{
//  uint16_t mode : 2;
//  uint16_t speed: 2;
//  uint16_t od   : 1;
//  uint16_t pupd : 2;
//  uint16_t afr  : 4;
//} port_info_type; 

// init GPIO Port
//static void init_port (GPIO_TypeDef *GPIOx, const struct port_info *port)
//{
//  int i;
//  uint32_t mode = 0, speed = 0, type = 0, pupd = 0;
//  uint64_t afr = 0;
//
//  for (i = 0; i < 16; i++)
//  {
//    mode  |= (uint32_t) port[i].mode  << i*2;
//    speed |= (uint32_t) port[i].speed << i*2;
//    pupd  |= (uint32_t) port[i].pupd  << i*2;
//    type  |= (uint32_t) port[i].od    << i;
//    afr   |= (uint64_t) port[i].afr   << i*4;
//  }
//
//  GPIOx->AFR[0]  = afr;           // set AFR registers first
//  GPIOx->AFR[1]  = afr >> 32;     // set AFR registers first
//  GPIOx->MODER   = mode;
//  GPIOx->OSPEEDR = speed;
//  GPIOx->PUPDR   = pupd;
//  GPIOx->OTYPER  = type;
//}

// setup GPIOs for Touch operation
void init_tcs (void)
{
  // enable Comparator interface clock to access RI registers
  RCC->APB1ENR |= RCC_APB1ENR_COMPEN;

  // no hysteresis on PA0 (& PA1???)
  RI->HYSCR1 = 0b11;

  // set output value
  gpio_clr (GPIOA, 0);           // PA0 is zero
  gpio_clr (GPIOA, 1);           // PA1 is zero

  // ADC analog switches open !!! - st
  RI->ASCR1 &= (uint32_t)(~0x80000000); // ADC analog switches closed if the corresponding I/O switch is also closed (????)
}

// sampling capacitor discharge
void tsc_cs_discharge (void)
{
  // GPIO output - drive zero
  gpio_clr (GPIOA, 0);           // PA0 is zero

  int tmp = GPIOA->MODER;

  tmp &= ~(0b11 << 0);
  tmp |=  (0b01 << 0);

  GPIOA->MODER = tmp;
}

// sampling capacitor floating
void tsc_cs_float (void)
{
  // GPIO to input
  int tmp = GPIOA->MODER;

  tmp &= ~(0b11 << 0);

  GPIOA->MODER = tmp;
}

// touch capacitor discharge
void tsc_ct_discharge (void)
{
  // GPIO to output - drive zero
  gpio_clr (GPIOA, 1);           // PA1 is zero

  int tmp = GPIOA->MODER;

  tmp &= ~(0b11 << 2);
  tmp |=  (0b01 << 2);

  GPIOA->MODER = tmp;
}

// touch capacitor floating
void tsc_ct_float (void)
{
  // GPIO input
  int tmp = GPIOA->MODER;

  tmp &= ~(3UL << 2); // input

  GPIOA->MODER = tmp;
}

// touch capacitor charge
void tsc_ct_charge (void)
{
  // GPIO output - drive one
  gpio_set (GPIOA, 1);           // PA1 is one

  int tmp = GPIOA->MODER;

  tmp &= ~(0b11 << 2); // clear all
  tmp |=  (0b01 << 2); // output

  GPIOA->MODER = tmp;
}


#define TCS_START   0
#define TCS_PHASE1  1
#define TCS_PHASE2  2
#define TCS_PHASE3  3
#define TCS_PHASE4  4
#define TCS_PHASE5  5
#define TCS_PHASE6  6
#define TCS_PHASE7  7

int tcs_state = TCS_START;

//State  Channel I/O configuration                 Sampling capacitor I/O configuration
//
// #1    Output push-pull low                      Output push-pull low
// #2    Input floating                            Input floating with hysteresis disabled 
// #3    Output push-pull high                     Input floating with hysteresis disabled 
// #4    Input floating                            Input floating with hysteresis disabled 
// #5    Input floating with analog switch closed  Input floating with hysteresis disabled and analog switch closed
// #6    Input floating                            Input floating with hysteresis disable

void tcs_next (void)
{
  static int cnt = 0;
  static int done = 0;
  int busy = 1;

  while (busy)
  {
    switch (tcs_state)
    {
      case TCS_START:
      {
        cnt = 0;
        tcs_state = TCS_PHASE1;
      //break;
      }
    
      // discharge Ct & Cs
      case TCS_PHASE1:
      {
        tsc_cs_discharge ();
        tsc_ct_discharge ();
    
        usleep (500);
    
        tcs_state = TCS_PHASE2;
      //break;
      }

      // float Ct & Cs    
      case TCS_PHASE2:
      {
        tsc_cs_float ();
        tsc_ct_float ();
    
        usleep (10);
    
        tcs_state = TCS_PHASE3;
      //break;
      }

      // charge Ct    
      case TCS_PHASE3:
      {
        tsc_ct_charge ();
    
      //usleep (2);
    
        tcs_state = TCS_PHASE4;
      //break;
      }

      // float Ct    
      case TCS_PHASE4:
      {
        tsc_ct_float ();
    
      //usleep (10);
    
        tcs_state = TCS_PHASE5;
      //break;
      }

      // charge transfer (sure?)    
      case TCS_PHASE5:
      {
        RI->ASCR1 = 3ul;  // switches closed
    
      //usleep (20);
    
        tcs_state = TCS_PHASE6;
      //break;
      }

      // compare Cs 'full'    
      case TCS_PHASE6:
      {
        // check PA0
        if (GPIOA->IDR & 1)
          done = 1;      
        else
          cnt++;
    
        tcs_state = TCS_PHASE7;
        break;
      }

      // next round or terminate    
      case TCS_PHASE7:
      {
        RI->ASCR1 = 0;    // switches open
    
      //usleep (10);
    
        if (done)
        {
          printf2 ("cnt: %d %d\n", cnt, cnt);

          busy = 0;
          done = 0;
          cnt = 0;
          tcs_state = TCS_START;
          return;
        }
        else
          tcs_state = TCS_PHASE2;
        break;
      }
    
      default:
        break;
    }
  }
}

