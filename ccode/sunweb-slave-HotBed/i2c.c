// file    : i2c.c
// author  : rb
// purpose : I2C routines STM32F042xx
// date    : 181010
// last    : 181207
//

#include "includes.h"

#define I2C_DEBUG     1 

// init I2C 
void init_i2c (void)
{
  // select I2C1 clock
  RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;     // I2C1 clock is HSI
//RCC->CFGR3 |=  RCC_CFGR3_I2C1SW;     // I2C1 clock is SYSCLK

  init_i2c1 ();
}

// init I2C1 module
void init_i2c1 (void)
{
  // enable I2C1 clock
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // disable I2C1
  I2C1->CR1 &= ~I2C_CR1_PE;

  // set up I2C1 timing for 100 kHz operation, clock is HSI (8Mc)
  // note: data from p. 650 User Manual
  I2C1->TIMINGR = (1  << 28) |         // prescaler
                  (4  << 20) |         // SCL delay        (SCLDEL + 1)
                  (2  << 16) |         // SDA delay        (SDADEL + 0) 
                  (15 <<  8) |         // SCL high period  (SCLH + 1)
                  (19 <<  0);          // SCL low period   (SCLL + 1)

  // enable I2C1
  I2C1->CR1 |= I2C_CR1_PE;
}

// I2C1 write 
void i2c1_write (uint8_t adr, uint8_t *buf, uint8_t len)
{
  // set up I2C1
  I2C1->CR2 = I2C_CR2_AUTOEND     |         // autoend
              (len << 16)         |         // number of bytes to send
            //I2C_CR2_RD_WRN      |         // perform write action
              adr;                          // slave address

  // generate I2C START condition
  I2C1->CR2 |= I2C_CR2_START;

  // write bytes
  for (int i = 0; i < len; i++) 
  {
    // wait while busy
    while ((I2C1->ISR & I2C_ISR_TXE) == 0)
      ;

    // write data
    I2C1->TXDR = buf[i];
    msleep (1); // <> debug, remove later 190124
  }

  // relax a bit, make sure writes are not abutted
//usleep (500); 
  usleep (1500);  // <> debug, remove later 190124
}

// I2C1 read
void i2c1_read (uint8_t adr, uint8_t *buf, uint8_t len)
{
  // set up I2C1
  I2C1->CR2 = I2C_CR2_AUTOEND     |         // autoend
              (len << 16)         |         // number of bytes to read
              I2C_CR2_RD_WRN      |         // perform read action
              adr;                          // slave address

  // generate I2C START condition
  I2C1->CR2 |= I2C_CR2_START;

  // read bytes
  for (int i = 0; i < len; i++)
  {
    // wait while busy
    while ((I2C1->ISR & I2C_ISR_RXNE) == 0)
      ;

    // read data    
    buf[i] = I2C1->RXDR;
  }
}

// dump I2C1 registers
void i2c1_dump_registers (void)
{
  printf2 ("I2C1_CR1    : 0x%08x\n", I2C1->CR1);
  printf2 ("I2C1_CR2    : 0x%08x\n", I2C1->CR2);
  printf2 ("I2C1_OAR1   : 0x%08x\n", I2C1->OAR1);
  printf2 ("I2C1_OAR2   : 0x%08x\n", I2C1->OAR2);
  printf2 ("I2C1_TIMINGR: 0x%08x\n", I2C1->TIMINGR);
  printf2 ("I2C1_ISR    : 0x%08x\n", I2C1->ISR);
  printf2 ("I2C1_TXDR   : 0x%08x\n", I2C1->TXDR);
  printf2 ("I2C1_RXDR   : 0x%08x\n", I2C1->RXDR);
  printf2 ("\n");
}

