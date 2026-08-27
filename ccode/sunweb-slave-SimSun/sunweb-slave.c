// file    : sunweb-slave.c
// author  : rb
// purpose : SunWeb RS-485 Serial Bus SLAVE - for SimSun Devices
// date    : 180826
// last    : 190109
//
// note    : packet format:
//           SYN | ID | CMD | 0x00 | CHK           - no payload  : LEN == 0
//           SYN | ID | CMD | LEN  | PAY |...| CHK - with payload: LEN == payload length
//      rx[]  0     1    2     3      4         N
//
// probe/attach:
// 1 - receive 'probe' command with matching Device address
// 2 - respond with Device ID
// 3 - receive 'attach' command
// 4 - Device is attached to SunWeb & can process normal commands

#include "includes.h"

#define SW_DEBUG                       0    // dump low-level SunWeb parsing
#define SW_DBG_PARSE                   1    // dump Rx/Tx SunWeb commands
#define SW_RX_ECHO                     0    // dump Rx SunWeb packets
#define SW_TX_ECHO                     0    // dump Tx SunWeb packets

// SunWeb Tx/Rx buffers
uint8_t tx_buf [TX_BUFLEN];                 // Tx packet buffer
uint8_t rx_buf [RX_BUFLEN];                 // Rx packet buffer

// Rx packet state
uint8_t rx_cnt = 0;                         // Rx packet buffer counter
uint8_t rx_new = 0;                         // new Rx packet valid flag

// Rx packet payload state
uint8_t  pcnt = 0;                          // Rx packet payload counter
uint32_t plen = 0;                          // Rx packet payload length

// Device state
uint8_t probed   = 0;                       // probe flag
uint8_t attached = 0;                       // attach flag
uint8_t dev_id;                             // Device ID

// init SunWeb state
void init_sunweb (void)
{
  // get Device ID from FLASH
  dev_id = flash_id_read ();                
}

// poll Sunweb Bus USART1
void poll_bus (void)
{
  static uint32_t poll_state = SW_POLL_START;

  // get next character from SunWeb Bus
  int c = uart1_getc ();

  // bail when UART receive buffer empty
  if (c < 0)
    return;

  // parse SunWeb packet
  switch (poll_state)
  {
    // wait for SYNC byte
    case SW_POLL_START:
    {
      if (c == SW_SYNC)
      {
        rx_cnt = 0;       // reset Rx buffer counter
        pcnt   = 0;       // reset payload counter

        rx_buf[rx_cnt++] = c;

        debug2 (SW_DEBUG, "\n---\nsync found     : 0x%02x\n", rx_buf[rx_cnt-1]);

        poll_state = SW_POLL_GETID;
      }

      break;
    }

    // get Device ID byte
    case SW_POLL_GETID:
    {
      rx_buf[rx_cnt++] = c;
      debug2 (SW_DEBUG, "packet ID      : 0x%02x\n", rx_buf[rx_cnt-1]);

      poll_state = SW_POLL_GETCMD;
      break;
    }

    // get Bus Command byte
    case SW_POLL_GETCMD:
    {
      rx_buf[rx_cnt++] = c;
      debug2 (SW_DEBUG, "packet CMD     : 0x%02x\n", rx_buf[rx_cnt-1]);

      poll_state = SW_POLL_GETLENGTH;
      break;
    }

    // get payload length
    case SW_POLL_GETLENGTH:
    {
      // get payload length, check for valid length
      if (c > SW_MAXLEN)
      {
        printf2 ("payload too long: %d (%d)\n", c, rx_buf[rx_cnt-1]);
        poll_state = SW_POLL_ERROR;
      }
      else
      {
        plen = c;
        rx_buf[rx_cnt++] = c;
        debug2 (SW_DEBUG, "payload length  : %d\n", rx_buf[rx_cnt-1]);

        // check for payload
        if (plen)
          poll_state = SW_POLL_GETPAYLOAD;
        else
          poll_state = SW_POLL_GETCHECK;
      }     

      break;
    }

    // get payload
    case SW_POLL_GETPAYLOAD:
    {
      if (++pcnt <= plen)
      {
        // first, get payload
        rx_buf[rx_cnt++] = c;
        break;
      }

      poll_state = SW_POLL_GETCHECK;
      // fall thru
    }

    // & checksum
    case SW_POLL_GETCHECK:
    {
      // get checksum
      rx_buf[rx_cnt] = c;
      debug2 (SW_DEBUG, "checksum       : 0x%02x\n", rx_buf[rx_cnt]);

      poll_state = SW_POLL_DONE;
      // fall thru
    }

    // ready, all ok
    case SW_POLL_DONE:
    {
      // set Rx packet received flag
      rx_new = 1;
      debug2 (SW_DEBUG, "new packet found, ok\n---\n");

      poll_state = SW_POLL_START;
      break;
    }
    
    // some error, start over
    case SW_POLL_ERROR:
    {
      printf2 ("some error, bail\n");

      poll_state = SW_POLL_START;
      break;
    }

    default:
      break;
  }
}

// check & parse Rx packet
void parse_bus (void)
{
  // bail when nothing received
  if (!rx_new)
    return;
  
  // dump last Rx packet
  if (SW_RX_ECHO)
    sw_rxdump ();

  // check checksum ok & then parse Rx packet
  if (sw_rxcheck ())
    sw_parse_rxpacket ();
  
  // clear new Rx packet flag
  rx_new = 0;
}

// check Rx packet checksum
uint8_t sw_rxcheck (void)
{
  uint8_t sum = 0;
  uint8_t len = RX_LEN_BYTE;

  static uint32_t ok  = 0;
  static uint32_t err = 0;

  // calculate checksum, skip SYNC byte
  for (int i = 1; i < SW_HDRLEN + len; i++)
    sum ^= rx_buf[i];

  if (sum == rx_buf [SW_HDRLEN + len])
  {
    debug2 (SW_DEBUG, "Rx checksum ok   : %d/%d\n", ++ok, err);
    return 1;
  }
  else
  {
    printf2 ("Rx checksum ERROR: %d/%d\n", ok, ++err);
    return 0;
  }
}

// SunWeb Command parser - slave
void sw_parse_rxpacket (void)
{
  static uint8_t state = SW_PARSE_PROBE;

  switch (state)
  {
    // probe Device 
    case SW_PARSE_PROBE:
    {
      sw_handle_probe ();

      if (probed)
        state = SW_PARSE_ATTACH;

      break;
    }

    // attach Device to SunWeb
    case SW_PARSE_ATTACH:
    {
      sw_handle_attach ();

      if (attached)
        state = SW_PARSE_NORMAL;

      break;
    }

    // handle normal commands when attached
    case SW_PARSE_NORMAL:
    {
      sw_handle_command ();

      // restart state machine after detach
      if (!attached)
        state = SW_PARSE_PROBE;

      break;
    }

    default:
      break;
  }
}

// handle bus probe
void sw_handle_probe (void)
{
  // check Rx packet for matching command & Device ID 
  if ((RX_CMD_BYTE == SW_CMD_PROBE) && (RX_ID_BYTE == dev_id))
  {
    // probe response: send Device ID to Master
    sw_txpack (dev_id, SW_CMD_PROBE, 0, NULL);
    sw_txsend ();

    // update flags   
    probed   = 1;            // Device has been probed
    attached = 0;            // .. but is not yet attached
    debug2 (SW_DBG_PARSE, "sent Device ID to Master: 0x%02x\n", dev_id);
  }
}

// handle bus attach
void sw_handle_attach (void)
{
  // check Rx packet for matching command & Device ID      
  if ((RX_CMD_BYTE == SW_CMD_ATTACH) && (RX_ID_BYTE == dev_id))
  {
    attached = 1;        // Device attached
    debug2 (SW_DBG_PARSE, "SunWeb attach received, go on\n");
  }
}

// handle normal commands - SimSun
void sw_handle_command (void)
{
  // Rx packet not for this Device
  if ((RX_ID_BYTE != SW_ID_BROADCAST) && 
      (RX_ID_BYTE != SW_ID_BCASTLEDS) && 
      (RX_ID_BYTE != dev_id))
    return;

  // handle Device command
  switch (RX_CMD_BYTE)
  {
    // ----- SimSun commands -----
    // set Device dawn time (LEDs on)
    case SS_CMD_SETDAWN:
    {
      simsun_set_dawn (rx_buf[4], rx_buf[5], rx_buf[6]);
      debug2 (SW_DBG_PARSE, "device dawn time set: %02d:%02d:%02d\n", rx_buf[4], rx_buf[5], rx_buf[6]);
      break;
    }

    // set Device dusk time (LEDs off)
    case SS_CMD_SETDUSK:
    {
      simsun_set_dusk (rx_buf[4], rx_buf[5], rx_buf[6]);
      debug2 (SW_DBG_PARSE, "device dusk time set: %02d:%02d:%02d\n", rx_buf[4], rx_buf[5], rx_buf[6]);
      break;
    }

    // set light intensity Deep Red LEDs
    case SS_CMD_SETDRED:
    {
      leds_dred (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "Deep Red light intensity set to %d\n", rx_buf[4]);
      break;
    }

    // set light intensity Royal Blue LEDs
    case SS_CMD_SETRBLU:
    {
      leds_rblu (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "Royal Blue light intensity set to %d\n", rx_buf[4]);
      break;
    }

    // set light intensity Far Red LEDs
    case SS_CMD_SETFRED:
    {
      leds_fred (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "Far Red light intensity set to %d\n", rx_buf[4]);
      break;
    }

    // set maximum light intensity Deep Red LEDs
    case SS_CMD_SETDREDMAX:
    {
      simsun_dred_max (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "maximum Deep Red light intensity set to %d\n", rx_buf[4]);
      break;
    }

    // set maximum light intensity Royal Blue LEDs
    case SS_CMD_SETRBLUMAX:
    {
      simsun_rblu_max (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "maximum Royal Blue light intensity set to %d\n", rx_buf[4]);
      break;
    }

    // start SimSun LED program
    case SS_CMD_SIMSUNSTART:
    {
      simsun_start ();
      debug2 (SW_DBG_PARSE, "start SimSun\n");
      break;
    }

    // stop SimSun LED program
    case SS_CMD_SIMSUNSTOP:
    {
      simsun_stop ();
      debug2 (SW_DBG_PARSE, "stop SimSun\n");
      break;
    }

    // send back temperature & LED intensity to master
    case SS_CMD_GETSTATE:
    {
      uint8_t  buf[5];
      uint16_t cur_temp = ntc_gettemp ();

      // current temperature
      buf[0] = (uint8_t)((cur_temp & 0xff00) >> 8);
      buf[1] = (uint8_t) (cur_temp & 0x00ff);
    
      // current LED intensity
      buf[2] = simsun_get_dred ();
      buf[3] = simsun_get_rblu ();
      buf[4] = simsun_get_fred ();

      sw_txpack (dev_id, SS_CMD_GETSTATE, 5, buf);
      sw_txsend ();
      debug2 (SW_DBG_PARSE, "sent GETSTATE response to master: %d, %d, %d, %d\n", cur_temp, buf[2], buf[3], buf[4]);
      break;
    }

    // update NTC calibration parameter
    case SS_CMD_NTCERR:
    {
      uint16_t err = ((int16_t)rx_buf[4] << 8) + rx_buf[5];

      ntc_set_error (err);
      break;
    }

    // ----- misc. SunWeb commands -----
    // set Device ID
    case SW_CMD_SETBUSID:
    {
      flash_id_write (rx_buf[4]);
      debug2 (SW_DBG_PARSE, "SunWeb ID: 0x%04x\n", flash_id_read ());
      break;
    }

    // SunWeb release
    case SW_CMD_RELEASE:
    {
      // clear flags, detach from SunWeb
      probed   = 0;
      attached = 0;
      debug2 (SW_DBG_PARSE, "SunWeb bus release received, stop\n");
      break;
    }

    // set local time Device 
    case SW_CMD_SETRTC:
    {
      rtc_set_time_bcd (rx_buf[4], rx_buf[5], rx_buf[6]);
      debug2 (SW_DBG_PARSE, "device time set: %02d:%02d:%02d\n", 
                (((rx_buf[4]&0xf0) >> 4)*10 + (rx_buf[4]&0x0f)),
                (((rx_buf[5]&0xf0) >> 4)*10 + (rx_buf[5]&0x0f)), 
                (((rx_buf[6]&0xf0) >> 4)*10 + (rx_buf[6]&0x0f)));
      break;
    }

    // reset local time to midnight (start a new day)
    case SW_CMD_SYNCRTC:
    {
      rtc_set_time (0, 0, 0);
      debug2 (SW_DBG_PARSE, "SYNCRTC command received, RTC reset to midnight\n");
      break;
    }

    default:
    {
      printf2 ("command not implemented: 0x%02x\n", RX_CMD_BYTE);
      break;
    }
  }
}

// assemble SunWeb Tx packet
void sw_txpack (uint8_t id, uint8_t cmd, uint8_t len, uint8_t *buf)
{
  uint8_t sum = 0;

  // assemble Tx header
  tx_buf[0] = SW_SYNC;
  tx_buf[1] = id;
  tx_buf[2] = cmd;
  tx_buf[3] = len;

  // copy payload to Tx buffer
  for (int i = 0; i < len; i++)
    tx_buf[SW_HDRLEN + i] = buf[i];

  // calculate checksum, skip SYNC byte
  for (int i = 1; i < SW_HDRLEN + len; i++)
    sum ^= tx_buf[i];

  // & add checksum to Tx packet
  tx_buf[SW_HDRLEN + len] = sum; 
}

// send SunWeb Tx packet
void sw_txsend (void)
{
  uint8_t len;

  // dump Tx packet
  if (SW_TX_ECHO)
    sw_txdump ();

  // get Tx packet length
  len = SW_HDRLEN + TX_LEN_BYTE + 1;

  // switch transceiver to transmit mode
  msleep (10);                // give Master time for bus turn-around
  rs485_dir (SW_SEND);
  usleep (100);               // relax a bit

  // send SunWeb packet
  for (int i = 0; i < len; i++)
  {
    uart1_putc (tx_buf[i]);

    // wait for UART transmit complete
    while (!(USART1->ISR & USART_ISR_TXE))
      ;
  }

  // wait for transmit done & switch transceiver to receive mode
  msleep (5);                 
  rs485_dir (SW_RECV);
}

// dump Tx packet buffer
void sw_txdump (void)
{
  printf2 ("Tx: ");
  dump_ascii (tx_buf, (uint16_t)(SW_HDRLEN + TX_LEN_BYTE + 1));
}

// dump Rx packet buffer
void sw_rxdump (void)
{
  printf2 ("Rx: ");
  dump_ascii (rx_buf, (uint16_t)(SW_HDRLEN + TX_LEN_BYTE + 1));
}

