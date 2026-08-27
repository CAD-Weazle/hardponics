// file    : sunweb.h
// author  : rb
// purpose : header file for sunweb.c
// date    : 180916
// last    : 190122
//

#ifndef _SUNWEB_SLAVE_H_
#define _SUNWEB_SLAVE_H_

// -- defines
#define SW_RECV                 0      // switch RS-485 transceiver to listening
#define SW_SEND                 1      // switch RS-485 transceiver to sending

#define SW_MAXLEN              16      // SunWeb maximum packet length
#define TX_BUFLEN       SW_MAXLEN      // buffer Tx packets
#define RX_BUFLEN       SW_MAXLEN      // buffer Rx packets

#define SW_HDRLEN               4      // SunWeb Packet header length (SYNC+ID+CMD+LEN)

#define RX_ID_BYTE      rx_buf[1]      // Rx packet byte holding Device ID
#define RX_CMD_BYTE     rx_buf[2]      // Rx packet byte holding command
#define RX_LEN_BYTE     rx_buf[3]      // Rx packet byte holding packet length
#define TX_LEN_BYTE     tx_buf[3]      // Tx packet byte holding packet length

// SunWeb Receive Packet parsing statemachine
#define SW_POLL_START           0
#define SW_POLL_GETID           1
#define SW_POLL_GETCMD          2
#define SW_POLL_GETLENGTH       3
#define SW_POLL_GETPAYLOAD      4
#define SW_POLL_GETCHECK        5
#define SW_POLL_ERROR          98
#define SW_POLL_DONE           99

// SunWeb command parsing statemachine
#define SW_PARSE_PROBE          0
#define SW_PARSE_ATTACH         1
#define SW_PARSE_NORMAL         2

// SunWeb Packet defines
// -- SYNC byte
#define SW_SYNC              0x55      // sync byte 

// -- Device IDs
#define SW_ID_BROADCAST      0x00      // broadcast address all Devices
#define SW_ID_BCASTLEDS      0x10      // broadcast address LED Devices    <> needed?
#define SW_ID_BCASTHEAT      0x20      // broadcast address Heater Devices <> needed?

// SunWeb Commands
// -- probe/attach commands
#define SW_CMD_PROBE         0xa0      // probe SunWeb Devices
#define SW_CMD_ATTACH        0xa1      // attach SunWeb Device
#define SW_CMD_RELEASE       0xa2      // detach SunWeb Device

// -- SunWeb commands
#define SW_CMD_SETBUSID      0xb0      // set SunWeb Device Bus ID
#define SW_CMD_SYNCRTC       0xb1      // sync ARM RTC at midnight 00:00:00
#define SW_CMD_SETRTC        0xb2      // set ARM RTC 

// -- SimSun commands
#define SS_CMD_SETDAWN       0x10      // set dawn time (LEDs on)
#define SS_CMD_SETDUSK       0x11      // set dusk time (LEDs off)
#define SS_CMD_SETDRED       0x12      // set light intensity Deep Red LEDs
#define SS_CMD_SETRBLU       0x13      // set light intensity Royal Blue LEDs
#define SS_CMD_SETFRED       0x14      // set light intensity Far Red LEDs
#define SS_CMD_SETDREDMAX    0x15      // set maximum light intensity Deep Red LEDs
#define SS_CMD_SETRBLUMAX    0x16      // set maximum light intensity Royal Blue LEDs
#define SS_CMD_SIMSUNSTART   0x17      // start SimSun program
#define SS_CMD_SIMSUNSTOP    0x18      // stop SimSun program
#define SS_CMD_GETSTATE      0x19      // get temperature & LED intensity
#define SS_CMD_NTCERR        0x1a      // write NTC calibration parameter

// -- HotBed commands
#define HB_CMD_SETTEMP       0x20      // set setpoint HotBed heater [100*oC]
#define HB_CMD_GETTEMP       0x21      // get setpoint & actual HotBed temperature [100*oC]
#define HB_CMD_SETLEDS       0x22      // set bottom LEDs light intensity [0..100]
#define HB_CMD_SETFAN        0x23      // set fan speed [0..100]
#define HB_CMD_STARTPID      0x24      // start PID temperature control
#define HB_CMD_STOPPID       0x25      // stop PID temperature control
#define HB_CMD_STARTSHOW     0x26      // start bottom LEDs 'show'
#define HB_CMD_STOPSHOW      0x27      // stop bottom LEDs 'show'
#define HB_CMD_RTDERR        0x28      // write Pt100 calibration parameter
#define HB_CMD_GETSTATE      0x29      // get LEDs, fan, PID state
#define HB_CMD_GETPT100      0x2a      // get all Pt100 sensors, for error calibration only

// -- prototypes
void init_sunweb (void);

void poll_bus  (void);
void parse_bus (void);

void sw_parse_rxpacket (void);

void sw_handle_probe     (void);
void sw_handle_attach    (void);
void sw_handle_command   (void);

uint8_t sw_rxcheck (void);

void sw_txpack (uint8_t id, uint8_t cmd, uint8_t len, uint8_t *buf);
void sw_txsend (void);

void sw_txdump (void);
void sw_rxdump (void);

#endif


