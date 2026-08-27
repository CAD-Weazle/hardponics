// file    : ble.c
// author  : rb
// purpose : RN4871 Bluetooth LE module interface
// date    : 191014
// last    : 191026
//
// note    : only connects to BLE module connected with Grandpa Box
// note    : print using color: printf ("\E[35m%c\E[30m", c);

#include "includes.h"

#define BLE_DEBUG                      0    // debug output on/off

// for command parser (from 'cli.c')
int   bargc;
char *bargv[10];

// RN4871 module state
uint8_t ble_mode;                      // 0: data mode / 1: command mode

uint8_t ble_dev_found;                 // Grandpa Box found during scanning
char    ble_dev_mac[12];               // Grandpa Box BLE module 48-bit MAC address
uint8_t ble_connected;                 // Grandpa Box connected

uint8_t ble_buf_len;                   // input buffer (from BLE module to ARM)
char    ble_buf[BLE_INBUF_LEN];        // input bufer length

// set up RN4871 BLE modules
void init_ble (void)
{
  // clear flags
  ble_mode = BLE_DAT_MODE;

  ble_dev_found = 0; 
  ble_connected = 0;          
}

// reset module
void ble_reset (void)
{
  GPIOC->CLR = BLE_RESET_L;
  msleep (200);
  GPIOC->SET = BLE_RESET_L;
}

// enter command mode
void ble_cmd_mode (void)
{
  debug (BLE_DEBUG, "BLE: enter command mode\n");

  msleep (100);                        // relax a bit (p. 12 RN4871 User Manual)
  printf4 ("$$$");
}

// enter data mode
void ble_data_mode (void)
{
  debug (BLE_DEBUG, "BLE: enter data mode\n");
  printf4 ("---\r");
}

// start scanning for client Devices
void ble_scan_start (void)
{
  if (ble_mode == BLE_CMD_MODE)
  {
    // send command
    printf4 ("F\r");
    debug (BLE_DEBUG, "BLE: start scanning as Central GAP\n");
  }
  else
    printf ("error: BLE not in command mode\n");
}

// stop scanning for client Devices
void ble_scan_stop (void)
{
  // send command
  printf4 ("X\r");
  debug (BLE_DEBUG, "BLE: stop scanning as Central GAP\n");
}

// process advertising BLE device
uint8_t ble_get_device (char *str)
{
  char c;
  char    buf[5][32];  // storage for BLE advertising data
  uint8_t chr = 0;     // response charachter pointer
  uint8_t wrd = 0;     // response word pointer

  printf ("BLE: client Device found: %s\n", str);

  // process advertising data
  for (int i = 0; (c = str[i]) != '\0'; i++)
  {
    if (c == ',')
    {
      // end of word
      buf[wrd][chr] = '\0';
      chr = 0;
      wrd++;
    }
    else
    {
      // read character of word
      if (c != '%')
        buf[wrd][chr++] = c;
    }    
  }

  // check for connectable & Grandpa Box, ignore rest
  if ((buf[1][0] == '0') && (!strcmp (buf[2], "GrandpaBox")))
  {
    strcpy (ble_dev_mac, buf[0]);
    
    ble_dev_found = 1;

    debug (BLE_DEBUG, "BLE: connectable client Device found, MAC: 0x%s\n", ble_dev_mac);

    return 1;
  }
  else
    return 0;
}

// connect device using MAC address
void ble_connect_device (char *mac)
{
  printf4 ("C,0,%s\r", mac);
  printf  ("BLE: connect %s\r", mac);
}

// scan BLE response
uint8_t ble_scan_response (uint8_t resp)
{
  static uint8_t scan_state = 0;

  int c = uart4_getc ();

  if (c < 0)
    return 0;

//printf ("\E[35m%c\E[30m\n", c);

  // jump in FSM 
  if (scan_state == 0)
  {
    if ((resp == BLE_RESPONSE_REBOOT)     || 
        (resp == BLE_RESPONSE_SCAN_START) || 
        (resp == BLE_RESPONSE_CONNECT)    ||
        (resp == BLE_RESPONSE_STREAM_OPEN))
      scan_state = 1;
    else if (resp == BLE_RESPONSE_CMD)
      scan_state = 11;
    else if (resp == BLE_RESPONSE_SCAN_STOP)
      scan_state = 21;
    else if (resp == BLE_RESPONSE_DATA_MODE)
      scan_state = 31;
  }

  switch (scan_state)
  {
    case 1:
    {
      // scan for response of format: '%<response>%'
      if (c == '%')
      {
        // reset pointer
        ble_buf_len = 0;
        ble_buf[ble_buf_len++] = c;

        // switch state
        scan_state = 2;
      }

      break;   
    }

    // read rest of response & scan for last character
    case 2:
    {
      // read in rest untill trailing '%' found
      if (c == '%')
      {
        ble_buf[ble_buf_len++] = c;
        ble_buf[ble_buf_len] = '\0';   // done, terminate string

        // reset FSM state
        scan_state = 0;

        if (resp == BLE_RESPONSE_REBOOT)
        {
          // check for '%REBOOT%' response
          if (!strcmp (ble_buf, "%REBOOT%")); 
            return 1;
        }
        else if (resp == BLE_RESPONSE_SCAN_START)
        {
          // check for client device
          if (ble_get_device (ble_buf))
            return 1;   
        }
        else if (resp == BLE_RESPONSE_CONNECT)
        {
          // check for '%CONNECT%' response (-or- '%CONNECT,0,801F12B42646%' WTF?)
          if (!strcmp (ble_buf, "%CONNECT")); 
            return 1;
        }
        else if (resp == BLE_RESPONSE_STREAM_OPEN)
        {
          // check for '%STREAM_OPEN% or %DISCONNECT% response
          if (!strcmp (ble_buf, "%STREAM_OPEN%")) 
            return 1;

          if (!strcmp (ble_buf, "%DISCONNECT%")) 
            return 2;
        }
      }
      else if (isprint (c))
      {
        ble_buf[ble_buf_len++] = c;    // add character
      }

      break;
    }
   
    // scan 'CMD> ' prompt
    case 11:
    {
      // scan for response of format: 'CMD> '
      if (c == 'C')
      {
        // reset pointer
        ble_buf_len = 0;
        ble_buf[ble_buf_len++] = c;

        // switch state
        scan_state = 12;
      }

      break;
    }

    case 12:
    {
      // read in rest untill trailing ' ' found
      if (c == ' ')
      {
        ble_buf[ble_buf_len++] = c;
        ble_buf[ble_buf_len] = '\0';   // done, terminate string

        scan_state = 0;

        // check for 'CMD> ' response
        if (!strcmp (ble_buf, "CMD> ")); 
          return 1;
      }
      else if (isprint (c))
      {
        ble_buf[ble_buf_len++] = c;    // add character
      }

      break;
    }

    // scan for 'AOK' scan stop response
    case 21:
    {
      // scan for response of format: 'AOK'
      if (c == 'A')
      {
        // reset pointer
        ble_buf_len = 0;
        ble_buf[ble_buf_len++] = c;

        // switch state
        scan_state = 22;
      }

      break;
    }

    case 22:
    {
      // read in rest untill trailing 'K' found
      if (c == 'K')
      {
        ble_buf[ble_buf_len++] = c;
        ble_buf[ble_buf_len] = '\0';   // done, terminate string

        scan_state = 0;

        // check for 'AOK' response
        if (!strcmp (ble_buf, "AOK")); 
          return 1;
      }
      else if (isprint (c))
      {
        ble_buf[ble_buf_len++] = c;    // add character
      }

      break;
    }

    // scan 'END' prompt
    case 31:
    {
      // scan for response of format: 'END'
      if (c == 'E')
      {
        // reset pointer
        ble_buf_len = 0;
        ble_buf[ble_buf_len++] = c;

        // switch state
        scan_state = 32;
      }

      break;
    }

    case 32:
    {
      // read in rest untill trailing 'D' found
      if (c == 'D')
      {
        ble_buf[ble_buf_len++] = c;
        ble_buf[ble_buf_len] = '\0';   // done, terminate string

        scan_state = 0;

        // check for 'END' response
        if (!strcmp (ble_buf, "END")); 
          return 1;
      }
      else if (isprint (c))
      {
        ble_buf[ble_buf_len++] = c;    // add character
      }

      break;
    }

    default: 
      break;
  }

  return 0;
}

// poll BLE module via UART4
void poll_ble (void)
{
  static uint8_t ble_state = BLE_RESET_ASSERT;

  switch (ble_state)
  {
    // first reset BLE module
    case BLE_RESET_ASSERT:
    {
      // reset BLE state
      ble_mode = BLE_DAT_MODE;       
      ble_connected = 0;

      // apply reser pulse
      ble_reset ();

      ble_state = BLE_RESET_RESPONSE;   

      break;
    }

    case BLE_RESET_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_REBOOT))
      {
        printf ("BLE: device rebooting\n");

        ble_state = BLE_CMD_MODE_ENTER; 
      }
   
      break;
    }

    // enter command mode
    case BLE_CMD_MODE_ENTER:
    {
      printf ("BLE: enter command mode\n");

      ble_cmd_mode  ();

      ble_state = BLE_CMD_MODE_RESPONSE;
      break;
    }

    case BLE_CMD_MODE_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_CMD))
      {
        printf ("BLE: device entered command mode\n");

        // set BLE state
        ble_mode = BLE_CMD_MODE;

        ble_state = BLE_SCAN_START;   
      }

      break;
    }

    // start scanning for BLE devices    
    case BLE_SCAN_START:
    {
      printf ("BLE: device scanning in GAP role\n");

      ble_scan_start ();

      ble_state = BLE_SCAN_START_RESPONSE;   
      break;
    }

    case BLE_SCAN_START_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_SCAN_START))
      {
        printf ("BLE: Grandpa Box found\n");

        ble_state = BLE_SCAN_STOP;   
      }

      break;
    }

    // stop scanning for BLE devices    
    case BLE_SCAN_STOP:
    {
      printf ("BLE: stop scanning\n");

      // stop scanning
      ble_scan_stop ();
      
      ble_state = BLE_SCAN_STOP_RESPONSE;   
      break;
    }

    case BLE_SCAN_STOP_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_SCAN_STOP))
      {
        printf ("BLE: scanning stopped\n");

        ble_state = BLE_CONNECT;   
      }

      break;
    }

    // connect to client BLE
    case BLE_CONNECT:
    {
      printf ("BLE: connect with Grandpa Box\n");

      // connect
      ble_connect_device (ble_dev_mac);

      ble_state = BLE_CONNECT_RESPONSE;

      break;
    }

    case BLE_CONNECT_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_CONNECT))
      {
        printf ("BLE: connected with Grandpa Box\n");

        ble_state = BLE_DATA_MODE_ENTER;   
      }

      break;
    }

    // enter transparent UART data mode
    case BLE_DATA_MODE_ENTER:
    {
      ble_data_mode ();

      ble_state = BLE_DATA_MODE_RESPONSE;   

      break;
    }

    case BLE_DATA_MODE_RESPONSE:
    {
      if (ble_scan_response (BLE_RESPONSE_DATA_MODE))
      {
        printf ("BLE: entered data mode\n");

        ble_state = BLE_STREAM_OPEN;   
      }

      break;
    }

    // scan for '%STREAM_OPEN% or %DISCONNECT% response
    case BLE_STREAM_OPEN:
    {
      int res = ble_scan_response (BLE_RESPONSE_STREAM_OPEN);

      if (res == 1)
      {
        printf ("BLE: connection, go on with UART mode\n");
        ble_connected = 1;
        ble_state = BLE_UART_MODE;   
      }
      else if (res == 2)
      {
        printf ("BLE: no connection, start over\n");
        ble_state = BLE_RESET_ASSERT;   
      }

      break;
    }

    case BLE_UART_MODE:
    {
      // read BLE
      int c = uart4_getc ();

      // test for Rx buffer empty
      if (c < 0)
        return;

      // add byte to buffer & parse - <> dev only: use 'cli.c' commands
      if ((c == '\r') | (c == '\n'))       // check for enter to end string
    //if ((c == '\r'))                     // check for enter to end string
      {
        printf4 ("\n\r");                  // print newline

        ble_buf[ble_buf_len] = '\0';       // '\0' terminate buffer
        ble_buf_len          = 0;          // reset pointer
    
        bargc = split_line (ble_buf, bargv); // parse command
        parse_line (bargc, bargv);          

        prompt ();                         // display new prompt
      }
      else if (c == '\b' || c == 0x7f)     // check for backspace and delete
      {
        if (ble_buf_len > 0)
         ble_buf_len--;
      }
      else if (isprint(c) && ble_buf_len < BLE_INBUF_LEN)
        ble_buf[ble_buf_len++] = c;


      // read UART1 & send <> test only
      int d = uart1_getc ();
    
      if (d > 0)
        printf4 ("%c", d);
    
      break;
    }

    default:
      break;
  }
}
 
