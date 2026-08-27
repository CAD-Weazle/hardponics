// file    : uart2.h
// author  : rb
// purpose : header file for uart2.c
// date    : 170813
// last    : 210318
//

#ifndef _UART2_H_
#define _UART2_H_

// -- defines
#define RX2_SIZE             128
#define TX2_SIZE             128
//#define INBUF_LEN            RX2_SIZE  // for old 'cli.c' code

// -- prototypes
void init_uart2 (int baud);

void uart2_putc (int c);
void uart2_puts (const char *s);
int  uart2_getc (void);

#endif