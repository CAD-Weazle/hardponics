// file    : touch.c
// author  : rb
// purpose : STM32L1xx Touch IO port routines
// date    : 220304
// last    : 230202
//
// note    : sampling capacitor on PB4  - group 6-1 
//           BUTTON0  capacitor on PB5  - group 6-2
//           BUTTON1  capacitor on PB6  - group 6-3
//           BUTTON2  capacitor on PB7  - group 6-4
//           
// note    : sampling capacitor on PB12 - group 7-1 
//           BUTTON3  capacitor on PB13 - group 7-2
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

// touch button GPIO mapping
int touch_gpio[TOUCH_NUM_BUTTONS] = {GPIO_CAP_BUTTON0, GPIO_CAP_BUTTON1, GPIO_CAP_BUTTON2, GPIO_CAP_BUTTON3};

int touch_dat[TOUCH_NUM_BUTTONS];                 // current touch input value
int touch_old[TOUCH_NUM_BUTTONS];                 // former touch input value
int touch_but[TOUCH_NUM_BUTTONS];                 // touch button state (0: not pressed / 1: pressed)

int touch_press[TOUCH_NUM_BUTTONS] = {0};         // touch button pressed flag, used in other routines

int touch_enabled = 1;                            // touch buttons active flag (mostly off due to spurs)

// setup GPIOs for Touch operation
void init_touch (void)
{
  // enable Comparator interface clock to access RI registers
  RCC->APB1ENR |= RCC_APB1ENR_COMPEN;

  // no hysteresis on sampling cap pin PB4/PB12 (& PB5, PB6, PB7, PB13???)
  RI->HYSCR1 = (1 << (GPIO_CAP_SAMPLE1 + 16))    // add 16 for hi-word bits
             | (1 << (GPIO_CAP_BUTTON0 + 16))
             | (1 << (GPIO_CAP_BUTTON1 + 16))
             | (1 << (GPIO_CAP_BUTTON2 + 16))
             | (1 << (GPIO_CAP_SAMPLE2 + 16))
             | (1 << (GPIO_CAP_BUTTON3 + 16));

  // clear all 
  gpio_clr (GPIOB, GPIO_CAP_SAMPLE1);
  gpio_clr (GPIOB, GPIO_CAP_BUTTON0);
  gpio_clr (GPIOB, GPIO_CAP_BUTTON1);
  gpio_clr (GPIOB, GPIO_CAP_BUTTON2);
  gpio_clr (GPIOB, GPIO_CAP_SAMPLE2);
  gpio_clr (GPIOB, GPIO_CAP_BUTTON3);

  // ADC analog switches open!!! - st
//RI->ASCR1 &= (uint32_t)(~0x80000000); // ADC analog switches closed if the corresponding I/O switch is also closed (????)
  RI->ASCR1 = 0; // ADC analog switches closed if the corresponding I/O switch is also closed (????)
  RI->ASCR2 = 0; 

  // get steady state (unpressed) touch button values
  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
    touch_old[i] = touch_read (i);
}

// read touch buttons
void touch_update (void)
{
  if (!touch_enabled)
    return;

  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
  {
    touch_dat[i] = touch_read (i);

    if ((touch_old[i] - touch_dat[i]) > TOUCH_DELTA)
    //touch_but[i] = 1;
      touch_but[i] += 1;

    else
      touch_but[i] = 0;
  }

  // update touch button state
  touch_scan ();

  // patch: faulty touch button always off (has sawtooth, turns on randomly - WTF?)
  touch_but[2] = 0;
   
  // update serial LEDs
//ws2815_touchstate (touch_but);
}

// scan touch buttons & 'debounce'
void touch_scan (void)
{
  static int pressed[TOUCH_NUM_BUTTONS]  = {0,0,0,0};
  static int released[TOUCH_NUM_BUTTONS] = {[0 ... TOUCH_NUM_BUTTONS-1] = 1};

  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
  {
    // swap touch button order
    int but_num = (TOUCH_NUM_BUTTONS-1) - i;  

    // check for touch button pressed
    if ((touch_but[but_num] == 1) && (released[but_num] == 1))
    {
      // update state
      pressed[but_num]  = 1;
      released[but_num] = 0;
    
      // flag button pressed (use in other routines)
      touch_press[but_num] = 1;
    }
    
    // check for touch button released
    if ((touch_but[but_num] == 0) && (pressed[but_num] == 1))
    {
      // update state
      pressed[but_num]  = 0;
      released[but_num] = 1;
    }
  }
}

// get touch button state OLED on/off control 
int touch_oled_pressed (void)
{
  if (touch_press[TOUCH_BUTTON_OLED] == 1)
  {
    touch_press[TOUCH_BUTTON_OLED] = 0;
    return (1);
  }
  else
    return (0);
}

// get touch button state PUMP0 on/off controll
int touch_pump0_pressed (void)
{
  if (touch_press[TOUCH_BUTTON_PUMP0] == 1)
  {
    touch_press[TOUCH_BUTTON_PUMP0] = 0;
    return (1);
  }
  else
    return (0);
}

// get touch button state PUMP1 on/off controll
int touch_pump1_pressed (void)
{
  if (touch_press[TOUCH_BUTTON_PUMP1] == 1)
  {
    touch_press[TOUCH_BUTTON_PUMP1] = 0;
    return (1);
  }
  else
    return (0);
}

// read touch buttons
int touch_read (int button)
{
  int tcs_state = TCS_START;
  int cnt = 0;
 
  // sanity check
  if (button >= TOUCH_NUM_BUTTONS)
    return (TCS_ERR_SAMPLE);

  while (1)
  {
    switch (tcs_state)
    {
      // reset all
      case TCS_START:
      {
        // discharge touch button & sampling cap
        tsc_cs_discharge (button);
        tsc_ct_discharge (button);
    
        // relax a bit
        usleep (50);

        // fall thru
      }
 
      // charge transfer from touch button to sampling cap  
      case TCS_SAMPLE:
      {
        // float input sampling cap & touch button
        tsc_cs_float (button);
        tsc_ct_float (button);
    
        // relax a bit
        usleep (10);
    
        // charge touch button & let float
        tsc_ct_charge (button);
        tsc_ct_float (button);   

        // set Routing Interface to transfer charge from touch button to sampling cap
        tcs_switches (button);

        // why no delay here?

        // all Routing Interface switches open, stop charge transfer
      //if (button < 3)
          RI->ASCR2 = 0;    
      //else
          RI->ASCR1 = 0;    

        if (button < 3) // RI group 6
        {
          // check if GPIO input level == '1' - RI group 6
          if (GPIOB->IDR & (1 << GPIO_CAP_SAMPLE1))
          {
            return (cnt);                   // return sample count & bail  
          }                                 
          else                              
          {                                 
            // check for time-out           
            if (cnt++ > TCS_MAX_SAMPLE)     
              return (TCS_ERR_SAMPLE);      // return error count & bail
            else                            
              tcs_state = TCS_SAMPLE;       // next sample
          }
        }
        else // RI group 7
        {
          // check if GPIO input level == '1' - RI group 7
          if (GPIOB->IDR & (1 << GPIO_CAP_SAMPLE2))
          {
            return (cnt);                   // return sample count & bail  
          }
          else
          {
            // check for time-out
            if (cnt++ > TCS_MAX_SAMPLE)
              return (TCS_ERR_SAMPLE);      // return error count & bail
            else                            
              tcs_state = TCS_SAMPLE;       // next sample
          }
        }

        break;
      }
    
      default:
        break;
    }
  }
}

// touch buttons active
void touch_enable (void)
{
  touch_enabled = 1;
}

// touch buttons disabled
void touch_disable (void)
{
  touch_enabled = 0;
}

// dump touch sensor values
void touch_dump (void)
{
  // touch button value
  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
    printf2 ("%d ", touch_dat[i]);

  // touch button state
  for (int i = 0; i < TOUCH_NUM_BUTTONS; i++)
    printf2 ("%d ", touch_but[i]);

  printf2 ("\n");
}


// -- Touch GPIO & RI switching matrix routines -------------------------------

// sampling capacitor discharge
void tsc_cs_discharge (int button)
{
  int tmp = GPIOB->MODER;

  if (button < 3)
  {
    // GPIO drive zero
    gpio_clr (GPIOB, GPIO_CAP_SAMPLE1);
  
    // GPIO to output 
    tmp &= ~(0b11 << (GPIO_CAP_SAMPLE1*2)); // clear bits
    tmp |=  (0b01 << (GPIO_CAP_SAMPLE1*2)); // output
  }                                         
  else                                      
  {                                         
    // GPIO drive zero                      
    gpio_clr (GPIOB, GPIO_CAP_SAMPLE2);     
                                            
    // GPIO to output                       
    tmp &= ~(0b11 << (GPIO_CAP_SAMPLE2*2)); // clear bits
    tmp |=  (0b01 << (GPIO_CAP_SAMPLE2*2)); // output
  }                                         
                                            
  GPIOB->MODER = tmp;                       
}                                           
                                            
// sampling capacitor floating              
void tsc_cs_float (int button)              
{                                           
  // GPIO to input                          
  int tmp = GPIOB->MODER;                   
                                            
  if (button < 3)                           
    tmp &= ~(0b11 << (GPIO_CAP_SAMPLE1*2)); // input
  else                                      
    tmp &= ~(0b11 << (GPIO_CAP_SAMPLE2*2)); // input

  GPIOB->MODER = tmp;
}

// touch button capacitor discharge
void tsc_ct_discharge (int button)
{
  // GPIO drive zero
  gpio_clr (GPIOB, touch_gpio[button]);

  // GPIO to output
  int tmp = GPIOB->MODER;

  tmp &= ~(0b11 << (touch_gpio[button]*2)); // clear bits
  tmp |=  (0b01 << (touch_gpio[button]*2)); // output
                                            
  GPIOB->MODER = tmp;                       
}                                           
                                            
// touch button capacitor charge            
void tsc_ct_charge (int button)             
{                                           
  // GPIO drive one                         
  gpio_set (GPIOB, touch_gpio[button]);     
                                            
  // GPIO to output                         
  int tmp = GPIOB->MODER;                   
                                            
  tmp &= ~(0b11 << (touch_gpio[button]*2)); // clear bits
  tmp |=  (0b01 << (touch_gpio[button]*2)); // output
                                            
  GPIOB->MODER = tmp;                       
}                                           
                                            
// touch button capacitor floating          
void tsc_ct_float (int button)              
{                                           
  // GPIO to input                          
  int tmp = GPIOB->MODER;                   
                                            
  tmp &= ~(0b11 << (touch_gpio[button]*2)); // input

  GPIOB->MODER = tmp;
}

// adjust analog switches Routing Interface
void tcs_switches (int button)
{
  switch (button)
  {
    // select sampling cap SAMPLE1 & BUTTON0
    case 0:
    {
      RI->ASCR2 = (1 << 4) | (1 <<  5);     // switches closed - PB4/PB5 (GRP6-1/GR6-2)
      break;
    }

    // select sampling cap SAMPLE1 & BUTTON1
    case 1:
    {
      RI->ASCR2 = (1 << 4) | (1 << 27);     // switches closed - PB4/PB6 (GRP6-1/GR6-3)
      break;
    }

    // select sampling cap SAMPLE1 & BUTTON2
    case 2:
    {
      RI->ASCR2 = (1 << 4) | (1 << 28);     // switches closed - PB4/PB7 (GRP6-1/GR6-4)
      break;
    }

    // select sampling cap SAMPLE2 & BUTTON3
    case 3:
    {
      RI->ASCR1 = (1 << 18) | (1 << 19);    // switches closed - PB12/PB13 (GRP7-1/GR7-2)
      break;
    }

    default:	
       break;
  }
}

