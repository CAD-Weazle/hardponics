// file    : leds.c
// author  : rb
// purpose : AL8860 LED driver routines
// date    : 180805
// last    : 190109
//
// note    : DC dimming 0V3 <= V_CTRL pin <= 2V5 (I_LED: 0%..100%)

#include "includes.h"

#define LED_DEBUG           0

// init SimSun LEDs
void init_leds (void)
{
  // all LEDs off
  leds_dred (LED_OFF);
  leds_rblu (LED_OFF);
  leds_fred (LED_OFF);
}

// set light intensity Deep Red LEDs
void leds_dred (uint8_t intensity)
{
  if (intensity <= 100)
  {
    pwm_set (PWM_DRED0, intensity);
    pwm_set (PWM_DRED1, intensity);
    pwm_set (PWM_DRED2, intensity);
  }
  else
  {
    pwm_set (PWM_DRED0, 100);
    pwm_set (PWM_DRED1, 100);
    pwm_set (PWM_DRED2, 100);
  }

  debug2 (LED_DEBUG, "LED DRED intensity: %d\n", intensity);
}

// set light intensity Royal Blue LEDs
void leds_rblu (uint8_t intensity)
{
  if (intensity <= 100)
    pwm_set (PWM_RBLU, intensity);
  else
    pwm_set (PWM_RBLU, 100);

  debug2 (LED_DEBUG, "LED RBLU intensity: %d\n", intensity);
}

// set light intensity Far Red LEDs <> no PWM yet, might be not needed
void leds_fred (uint8_t intensity)
{
  if (intensity)
    led_fred_on ();
  else
    led_fred_off ();

  debug2 (LED_DEBUG, "LED FRED intensity: %d\n", intensity ? 100:0);
}
