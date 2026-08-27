// file    : pwm.c
// author  : rb
// purpose : PWM control for bottom LEDs, heater and fan for SimSun HEATER PCB
// date    : 180805
// last    : 181031
//

#include "includes.h"

#define PWM_DEBUG     0 

// globals
uint8_t dat_led;             // bottom LEDs PWM duty cycle (0..100)
uint8_t dat_htr;             // heater PWM duty cycle (0..100)
uint8_t dat_fan;             // fan PWM duty cycle (0..100)

uint8_t led_pulse;

// init PWM 
void init_pwm (void)
{
  // default PWM settings
  pwm_set_heater (PWM_RESET_HEATER);
  pwm_set_leds   (PWM_RESET_LEDS);
  pwm_set_fan    (PWM_RESET_FAN);

  // adjust LED show flag
  pwm_show_stop ();
}

// set heater temperature
void pwm_set_heater (uint8_t intensity)
{
  if (intensity <= 100)
    dat_htr = intensity;
  else
    dat_htr = 100;

  pwm_set (PWM_HEATER, dat_htr);

  debug2 (PWM_DEBUG, "heater temperature   : %d\n", dat_htr);
}

// dump heater PWM state
void pwm_dump_heater (uint8_t cr_nl)
{
  printf2 ("pwm_heater: %d ", dat_htr);

  if (cr_nl)
    printf2 ("\n");
}

// set light intensity green bottom LEDs [% PWM]
void pwm_set_leds (uint8_t intensity)
{
  if (intensity <= 100)
    dat_led = intensity;
  else
    dat_led = 100;

  pwm_set (PWM_LEDS, dat_led);

  debug2 (PWM_DEBUG, "bottom LEDs intensity: %d\n", dat_led);
}

// get light intensity green bottom LEDs [% PWM]
uint8_t pwm_get_leds (void)
{
  return dat_led;
}

// dump green bottom LED PWM state
void pwm_dump_leds (uint8_t cr_nl)
{
  printf2 ("pwm_leds: %d ", dat_led);

  if (cr_nl)
    printf2 ("\n");
}

// set fan speed [% PWM]
void pwm_set_fan (uint8_t intensity)
{
  if (intensity <= 100)
    dat_fan = intensity;
  else
    dat_fan = 100;

  pwm_set (PWM_FAN, dat_fan);

  debug2 (PWM_DEBUG, "fan speed            : %d\n", dat_fan);
}

// get fan speed [% PWM]
uint8_t pwm_get_fan (void)
{
  return dat_fan;
}

// dump fan PWM state
void pwm_dump_fan (uint8_t cr_nl)
{
  printf2 ("pwm_fan: %d ", dat_fan);

  if (cr_nl)
    printf2 ("\n");
}

// dump PWM state
void pwm_state (void)
{
  pwm_dump_leds   (0);
  pwm_dump_heater (0);
  pwm_dump_fan    (0);
  printf2 ("\n");
}

// -- sine wave routines
// sine wave bottom LED intensity
static const uint8_t sine_tab[256] =
{
  128,131,134,137,140,143,146,149,152,156,159,162,165,168,171,174,
  176,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
  218,220,222,224,226,228,230,232,234,236,237,239,240,242,243,245,
  246,247,248,249,250,251,252,252,253,254,254,255,255,255,255,255,
  255,255,255,255,255,255,254,254,253,252,252,251,250,249,248,247,
  246,245,243,242,240,239,237,236,234,232,230,228,226,224,222,220,
  218,216,213,211,209,206,204,201,199,196,193,191,188,185,182,179,
  176,174,171,168,165,162,159,156,152,149,146,143,140,137,134,131,
  128,124,121,118,115,112,109,106,103, 99, 96, 93, 90, 87, 84, 81,
   79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39,
   37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10,
    9,  8,  7,  6,  5,  4,  3,  3,  2,  1,  1,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  1,  1,  2,  3,  3,  4,  5,  6,  7,  8,
    9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35,
   37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76,
   79, 81, 84, 87, 90, 93, 96, 99, 103,106,109,112,115,118,121,124
};

#define SINE_SAMPLES       256
#define SINE_AMP           100

#define PWM_TICK_START       0
#define PWM_TICK_STEADY      1
#define PWM_TICK_PULSE       2
#define PWM_TICK_DONE        3

#define PWM_STEADY_DELAY  1000

// sine drive bottom LEDs
static uint8_t  state = PWM_TICK_START;

void pwm_leds_tick (void)
{
  static uint16_t idx, cnt, ofs;

  static uint8_t  dat_tmp;

  if (!led_pulse)
    return;

  switch (state)
  {
    case PWM_TICK_START:
    {
      // reset state
      cnt = 0;
      idx = 0;
      dat_tmp = dat_led;

      // determine entry point sine table for smooth transition
      if (dat_led > 50)
      {
        // LED intensity > max_intensity/2
        ofs = 0;

        while (dat_led > ((SINE_AMP*sine_tab[ofs++]) / 256))
          ;
      }
      else
      {
        // LED intensity <= max_intensity/2
        ofs = 255;

        while (dat_led < ((SINE_AMP*sine_tab[--ofs]) / 256))
          ;
      }

      state = PWM_TICK_STEADY;
      // fall thru
    }

    case PWM_TICK_STEADY:
    {
      if (cnt++ >= PWM_STEADY_DELAY)
        state = PWM_TICK_PULSE;

      break;
    }

    case PWM_TICK_PULSE:
    {
      pwm_set_leds ((SINE_AMP*sine_tab[(idx + ofs) % 256]) / 256);

      if (idx++ >= 255)
      {
        // restore former LED intensity against round-off run-away
        dat_led = dat_tmp;

        state = PWM_TICK_START;
      }

      break;
    }
  }
}

// dump sine wave table
void pwm_dump_table (void)
{
  uint16_t i;

  for (i = 0; i < SINE_SAMPLES; i++)
    printf2 ("%d %d\n", i , (SINE_AMP*sine_tab[i]) / 256);
}

// start bottom LED 'show'
void pwm_show_start (void)
{
  led_pulse = 1;
  state = PWM_TICK_START;  // reset FSM
}

// stop bottom LED 'show'
void pwm_show_stop (void)
{
  led_pulse = 0;
}












