// file    : main.c
// author  : rb
// purpose : MOISTER - Capacitive Soil Moisture Sensor Proto
// board   : MOISTER v1.0
// date    : 220304
// last    : 220907
//

#include "includes.h"

int main (void)  
{
  // init all the things!1!!
  init_clock  ();                      // turn on clocks
  init_ticker ();                      // start timers
  init_ports  ();                      // setup GPIOs
  init_uart2  (115200);                // setup UART2 - CLI, only needed for debugging
  init_uart3  (115200);                // setup UART3 - BLE
  init_power  ();                      // setup core voltage
  init_tcs    ();                      // setup touch sensing
  init_adc    ();                      // setup ADC
  init_ntc    ();                      // setup NTC sensors
  init_ble    ();                      // setup BLE module

  version ();

  while (1)
  {
  //poll_cli ();                       // poll commandline - only needed for debugging
  //poll_ble ();                       // poll BLE module  - only needed for initialization 

    if (sec_elapsed)
    {
      ble_do ();

      sec_elapsed = 0;
    }
  }
}
