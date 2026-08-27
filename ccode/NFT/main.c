// file    : main.c
// author  : rb
// purpose : Solar NFT Water Pump System
// board   : NFC v1.0 - STM32L151CBT6A (128K FLASH, 16K SRAM) 
// date    : 221022
// last    : 230602
//
// note    : SPI3 only implemented in STM32L151xC variant
//
//

#include "includes.h"
#include <assert.h>

int main (void)  
{
  // init all the things!1!!
  init_pll     ();           // enable clocks & PLL
  init_ticker  ();           // start timers
  init_ports   ();           // setup GPIOs
  init_uart2   (115200);     // init UART2 - debug only
  init_uart3   (115200);     // init UART3 - ESP32-WROOM WIFI module
  init_spi     ();           // setup SPI
  init_adc     ();           // setup ADC
  init_dac     ();           // setup DAC
  init_ws2815  ();           // init WS2815 LED string
  init_esp32   ();           // set up ESP32_WROOM WIFI module
  init_sht15   ();           // set up SHT15 sensor
  init_touch   ();           // set up touch buttons
  init_ec      ();           // set up EC sensor
  init_ntc     ();           // set up NTC of EC sensor
  init_oled    ();           // set up OLED display
  init_pump    ();           // set up water pumps

  // dump version
  version ();

  while (1)
  {
    // poll commandline
    poll_cli ();                                 

    // poll ESP32
    poll_esp ();

    if (csec_elapsed)
    {
      // read ADC channels
      adc_update  ();

      csec_elapsed = 0;
    }

    if (dsec_elapsed)
    {
      // update conductivity 
      ec_update ();

      // handle touch buttons
      touch_update ();

      dsec_elapsed = 0;
    }

    if (sec_elapsed)
    {
      led_grn_flash (1);

      // control pumps & log data
      control ();

      sec_elapsed = 0;
    }
  }
}

