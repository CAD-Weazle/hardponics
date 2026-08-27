// file    : uart2.h
// author  : rb
// purpose : header file for uart2.c
// date    : 170813
// last    : 170813
//

// -- prototypes
void init_uart2 (int baud);

void uart2_putc (int c);
void uart2_puts (const char *s);
int  uart2_getc (void);

int uart2_flush (void *dev);


