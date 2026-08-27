// file    : main.c
// author  : rb
// purpose : SunWeb HotBed Device
// board   : STM32F042F6P6 SimSun PID
// date    : 180609
// last    : 181227

#include "includes.h"

int main (void)  
{
  // init all the things!1!!
  init_power   ();                               // power control
  init_clock   ();                               // MCU clock
  init_ports   ();                               // GPIOs
  init_ticker  ();                               // Timers
  init_uart1   (9600);                           // UART1 - RS-485 for bus
  init_uart2   (115200);                         // UART2 - RS-232 for debug
  init_i2c     ();                               // I2C
  init_pwm     ();                               // SimSun LEDs/heater/fan
  init_rtc     ();                               // RTC   
  init_sunweb  ();                               // SunWeb
  init_mcp3421 ();                               // MCP3421 ADC
  init_flash   ();                               // FLASH Pt100 parameters
  init_rtd     ();                               // Pt100 sensors
  init_pid     ();                               // PID temperature controller

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
      // bottom LEDs 'show'
      pwm_leds_tick ();

      csec_elapsed = 0;
    }

    if (dsec_elapsed)
    {
      // update temperature
      pt_update ();

      dsec_elapsed = 0;
    }

    if (sec_elapsed)   
    {
      // der Blinkenlights
      led_red_toggle ();

      // control temperature HotBed
      pid_update ();

      // dump actual temperatures to terminal
      pt_dump ();

      // dump wallclock to terminal 
      rtc_dump_time ();

      sec_elapsed = 0;
    }
  }
}
