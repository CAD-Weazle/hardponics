// file    : uart2.c
// author  : ao/rb
// purpose : UART2 routines
// date    : 170811
// last    : 180707
//

#include "includes.h"

#define RX_SIZE 128
#define TX_SIZE 128

// input/output FIFOs 
//static char uart2_rx_buf[RX_SIZE] __attribute__ ((section(".bss.ccm")));
//static char uart2_tx_buf[TX_SIZE] __attribute__ ((section(".bss.ccm")));

static char uart2_rx_buf[RX_SIZE];
static char uart2_tx_buf[TX_SIZE];

// FIFO indexes 
volatile static uint32_t tx_tail = 0;
volatile static uint32_t rx_head = 0;
         static uint32_t tx_head = 0;
         static uint32_t rx_tail = 0;

//#define rx_queued()  ( (int) (rx_head - rx_tail) )

// USART2 ISR
void USART2_IRQHandler (void)
{
  int isr = USART2->ISR;

  // read character
  if (isr & USART_ISR_RXNE)
  {
    int c = USART2->RDR;

    if ((int)(rx_head - rx_tail) < RX_SIZE) 
      uart2_rx_buf[rx_head++ % RX_SIZE] = c;
  }

  // write character
  if (isr & USART_ISR_TXE)
  {
    if (tx_head == tx_tail)
      USART2->CR1 &= ~USART_CR1_TXEIE; 
    else
      USART2->TDR = uart2_tx_buf[tx_tail++ % TX_SIZE];
  }
}

// write character to USART2
void uart2_putc (int c)
{
  if (c == '\n')
    uart2_putc( '\r' );

  while ((int)(tx_head - tx_tail) >= TX_SIZE)
    ;

  uart2_tx_buf[tx_head++ % TX_SIZE] = c;

  USART2->CR1 |= USART_CR1_TXEIE;
}

int uart2_flush (void *dev)
{
  while (tx_head != tx_tail)
    ;

  return 0;
}

// write string to USART2
void uart2_puts (const char *s)
{
  int c;

  while ((c = *s++) != 0)
    uart2_putc (c);
}

// get character from USART2
int uart2_getc (void)
{
  if (rx_head == rx_tail)
    return(-1);
  else
    return (uart2_rx_buf[rx_tail++ % INBUF_LEN]);
}

/* <> no used?
static int uart2_write (void *dev, const void *buf, int len)
{
  int retval = len;
  const char *s = buf;

  while (--len >= 0)
    uart2_putc (*s++);

  return retval;
}

static int uart2_read (void *dev, void *buf, int len)
{
  int retval = len;
  char *s = buf;

  while (--len >= 0)
    *s++ = uart2_getc ();

  return retval;
}
*/

// init USART2
void init_uart2 (int baud)
{
  // enable USART2 clock
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  USART2->CR3 = 0;
  USART2->CR2 = 0;                               // 1 stop bit
  USART2->BRR = ((APB2CLK << 4) / baud) >> 4;    // fixed-point that shit

  // note: oversampling by 16
  USART2->CR1 = USART_CR1_TE     | 
                USART_CR1_RE     | 
                USART_CR1_RXNEIE | 
                USART_CR1_UE;

  // enable USART2 interrupt
  NVIC_EnableIRQ (USART2_IRQn);
}










