// file    : main.c
// author  : rb
// purpose : MOISTER - Capacitive Soil Moisture Sensor Proto
// board   : BLE-JIG v1.0 patched
// date    : 200507
// last    : 220329
//

#include "includes.h"

#define PRINT_DELAY 10
int cnt = PRINT_DELAY;

int main (void)  
{
  // init all the things!1!!
  init_clock  ();                      // turn on clocks
  init_ticker ();                      // start timers
  init_ports  ();                      // setup GPIOs
  init_uart2  (115200);                // setup UART2 - CLI, only needed for debugging
  init_adc    ();                      // setup ADCs
  init_power  ();                      // setup core voltage
  init_tcs    ();                      // setup Touch controller
//init_comp   ();                      // setup comparator
  init_iir    ();                      // setup IIR filters

  version ();

  while (1)
  {
    poll_cli ();                       // poll commandline

    if (dsec_elapsed)
    {
//    cap_time ();

      dsec_elapsed = 0;
    }

    if (sec_elapsed)
    {
      led_grn_flash (10);

//    if (cnt == PRINT_DELAY)
//    {
//      cap_time ();
//      cnt = 0;
//    }
//    else
//      cnt++;

      tcs_next ();

      sec_elapsed = 0;
    }
  }
}
