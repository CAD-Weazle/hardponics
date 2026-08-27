// file    : main.c
// author  : rb
// purpose : SunWeb SimSun
// board   : STM32F042F6P6 SimSun CONTROL
// date    : 180609
// last    : 190109

#include "includes.h"

int main (void)  
{
  // init all the things!1!!
  init_power  ();                                // power control
  init_clock  ();                                // MCU clock
  init_ports  ();                                // GPIOs
  init_ticker ();                                // Timers
  init_uart1  (9600);                            // UART1 - RS-485 for bus
  init_uart2  (115200);                          // UART2 - RS-232 for debug
  init_adc    ();                                // ADC
  init_leds   ();                                // SimSun LEDs
  init_rtc    ();                                // RTC 
  init_simsun ();                                // SimSun
  init_sunweb ();                                // SunWeb
  init_flash  ();                                // FLASH NTC parameters
  init_ntc    ();                                // NTC sensor

  // display version information
  version ();

  // beertje non-termineertje
  while (1)
  {
    // handle command line interface & SunWeb 
    poll_cli ();
    poll_bus ();

    // parse received packets
    parse_bus ();

    if (csec_elapsed)
    {
      // get temperature SimSun PCB
      ntc_update ();

      csec_elapsed = 0;
    }

    if (sec_elapsed)   
    {
      // der Blinkenlights
      led_grn_toggle ();

      // run light program
      simsun_update ();

      // log RTC time to terminal
      rtc_log ();

      sec_elapsed = 0;
    }
  }
}
