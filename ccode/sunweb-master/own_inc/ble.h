// file    : ble.h
// author  : rb
// purpose : header file for ble.c
// date    : 191014
// last    : 191026

#ifndef _BLE_H_
#define _BLE_H_

// input buffer
#define BLE_INBUF_LEN                255    // Rx buffer size (data from BLE unit to ARM)

// data/command mode
#define BLE_DAT_MODE                   0    // BLE in data mode
#define BLE_CMD_MODE                   1    // BLE in command mode

// BLE response parser
#define BLE_RESPONSE_REBOOT            0
#define BLE_RESPONSE_CMD               1
#define BLE_RESPONSE_SCAN_START        2
#define BLE_RESPONSE_SCAN_STOP         4
#define BLE_RESPONSE_CONNECT           5
#define BLE_RESPONSE_DATA_MODE         6
#define BLE_RESPONSE_STREAM_OPEN       7

// BLE polling FSM
#define BLE_RESET_ASSERT               0
#define BLE_RESET_RESPONSE             1
#define BLE_CMD_MODE_ENTER             2
#define BLE_CMD_MODE_RESPONSE          3
#define BLE_SCAN_START                 4
#define BLE_SCAN_START_RESPONSE        5
#define BLE_SCAN_STOP                  6
#define BLE_SCAN_STOP_RESPONSE         7
#define BLE_CONNECT                    8
#define BLE_CONNECT_RESPONSE           9
#define BLE_DATA_MODE_ENTER           10
#define BLE_DATA_MODE_RESPONSE        11
#define BLE_STREAM_OPEN               12
#define BLE_UART_MODE                 13

// RN4871 features 
#define BLE_FLOW_CONTROL          0x8000    // enable hardware flow control
#define BLE_NO_PROMPT             0x4000    // suppress prompt
#define BLE_FAST_MODE             0x2000    // transparent UART mode in raw mode, command/data switch via GPIO 
#define BLE_NO_BEACON_SCAN        0x1000    // suppress non-connectable beacon in scan results
#define BLE_NO_CONNECT_SCAN       0x0800    // suppress (non-???)connectable advertisement in scan results
#define BLE_NO_DUPLICATE_SCAN     0x0400    // duplicates are not filtered
#define BLE_PASSIVE_SCAN          0x0200    // scan 0: passive / 1: active
#define BLE_NO_ACK                0x0100    // transparent mode without ACK
#define BLE_REBOOT_DISCONNECT     0x0080    // reboot after disconnection.
#define BLE_SCRIPT_ON_POR         0x0040    // automatically runs script after POR
#define BLE_MLDP STREAMING        0x0020    // support RN4020 MLDP streaming service
#define BLE_DLE                   0x0010    // disbale DLE (Date Length Extension) (firmware >= V1.28)
#define BLE_CMD MODE_GUARD        0x0008    // mode switch suppressed during 1 second window before and after $$$ command

// -- prototypes
void init_ble (void);

void ble_reset (void);

void ble_cmd_mode  (void);
void ble_data_mode (void);

void ble_scan_start (void);
void ble_scan_stop  (void);

uint8_t ble_get_device (char *str);

void ble_connect_device (char *mac);

uint8_t ble_scan_response (uint8_t resp);

void poll_ble (void);

#endif
















