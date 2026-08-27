// file    : esp32.c
// author  : rb
// purpose : ESP32-WROOM WIFI module routines
// date    : 211124
// last    : 230225
//

#include "includes.h"

uint8_t esp_buf_len;                        // input buffer (from WIFI module to ARM)
char    esp_buf[ESP_INBUF_LEN];             // input buffer length

int esp_echo = 0;
int esp_log  = 0;
int esp_rdy  = 0;                           // RESET done, module ready
int esp_con  = 0;                           // connection status

int esp_res = 0;

int esp_off = 50;                           // ESP32 power down timer

int esp_wd = 0;                             // watchdog counter

// init WIFI module
void init_esp32 (void) 
{ 
  esp32_reset ();
}

// control ESP32 power consumption
void esp32_update (void)
{
  if (esp_off > 0)
    esp_off--; 
  else
    esp32_run ();
}

// apply reset pulse to WIFI module
void esp32_reset (void)
{
  gpio_clr (GPIOC, WIFI_EN);
  msleep (10);
  gpio_set (GPIOC, WIFI_EN);
}

// apply reset condition to WIFI module
void esp32_stop (int time)
{
  gpio_clr (GPIOC, WIFI_EN);
  led_red_off ();
  esp_off = time;
}

// remove reset condition to WIFI module <> power consumption test
void esp32_run (void)
{
  gpio_set (GPIOC, WIFI_EN);
  led_red_on ();
}

// reset watchdog counter
void esp32_wd_reset (void)
{
  esp_wd = 0;
  printf2 (">ESP32 watchdog reset\n"); // <> remove later
}

// check ESP32 watchdog
void esp32_wd_test (void)
{
  esp_wd++;

  // reset EPS32 module after too long with no response
  if (esp_wd > ESP_WATCHDOG)
  {
    esp32_reset ();

    esp_wd = (ESP_WATCHDOG - 10);

    printf2 (">ESP32 watchdog tripped\n");
  }
}


#ifdef USE_LATER
// parse data from ESP12-F WIFI module 
void poll_esp32 (void)
{
  // read WIFI module
  int c = uart3_getc ();

  // test for Rx buffer empty
  if (c < 0)
    return;

  // read data from BLE Jig
  if (c == '\n')      // check for enter to end string
  {
    // '\0' terminate buffer
    esp_buf[esp_buf_len] = '\0';            

    // send WIFI module response to serial port
    if ((esp_echo) || (esp_log))
      printf2 ("%s\n", esp_buf);

    // parse data from WIFI module
    esp_parse_line (esp_buf, esp_buf_len);
   
    // reset pointer
    esp_buf_len = 0;               
  }
  else if (isprint(c) && (esp_buf_len < ESP_INBUF_LEN))
    esp_buf[esp_buf_len++] = c;
}
#endif




