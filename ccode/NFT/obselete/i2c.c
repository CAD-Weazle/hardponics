// file    : i2c.c
// author  : rb
// purpose : I2C routines STM32L1xx
// date    : 190701
// last    : 240223
//
// note    : during POR both SDA and SCL are pulled down for a brief moment,
//           this gives gezak in the I2C FSM, adding delay & clearing state is needed

#include "includes.h"

#define I2C_ERR             1  // dump I2C timeout errors

uint16_t to_cnt = 0;

// init I2C 
void init_i2c (void)
{
  // relax a bit to let bus lines settle, will prevent hanging after POR
  msleep (200);

  // setup I2C module
//init_i2c1 ();
  init_i2c2 ();
//init_i2c3 ();
}

// init I2C1 module
void init_i2c1 (void)
{
  // enable I2C1 clock
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // reset I2C block to resolve errors during POR
  I2C1->CR1 |=  I2C_CR1_SWRST;
  I2C1->CR1 &= ~I2C_CR1_SWRST;

  // disable I2C1
  I2C1->CR1 &= ~I2C_CR1_PE;

  // define APB clock frequency, no DMA, no interrupts
  I2C1->CR2 = 32;       // APB1CLK = 32Mc (from 'pll.h')

  // select Slow mode, duty cycle & set SCL frequency
  // note: 50% dutycycle -> T = (2*CCR)/APB1CLK -> CCR = 32Mc / 2*100kHz
  I2C1->CCR = 160;      // 100 kHz SCL
//I2C1->CCR = 80;       // 200 kHz SCL  <> check later

  // set SCL rise time 
  I2C1->TRISE = 42 + 1; // <> WTF??? <> check this

  // enable I2C1
  I2C1->CR1 |= I2C_CR1_PE;
}

















// I2C read word
uint16_t i2c_read_word (uint8_t reg)
{
  uint16_t ret = 0;

  // generate I2C START condition
  I2C1->CR1 |= I2C_CR1_START;

  // wait for START condition generated
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_SB) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // -- first, write slave write address & register to be read --

  // write I2C write address (= first byte after START condition)
  I2C1->DR = LTC4162_WADR;

  // wait for I2C address sent
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_ADDR) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // read SR2 to clear ADDR bit
  (void)I2C1->SR2;

  // write register number
  I2C1->DR = reg;

  // wait while busy
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_TXE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // -- then, write slave read address & read register --

  // generate I2C RESTART condition
  I2C1->CR1 |= I2C_CR1_START;

  // wait for START condition generated
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_SB) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // write I2C read address (= first byte after START condition)
  I2C1->DR = LTC4162_RADR;

  // wait for I2C address sent
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_ADDR) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // read SR2 to clear ADDR bit
  (void)I2C1->SR2;

  // read two bytes: clear ACK, set POS (see p. 671 UM, "2 byte reception")
  I2C1->CR1 &= ~I2C_CR1_ACK; 
  I2C1->CR1 |= I2C_CR1_POS; 
 
  // wait while busy
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_RXNE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // read first byte
  ret = I2C1->DR;

  // wait while busy
  to_cnt = 0;

  while ((I2C1->SR1 & I2C_SR1_RXNE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return 0;
    }
    else
      usleep (I2C_WAITDELAY);
  }

  // read second byte
  ret |= ((int16_t)I2C1->DR << 8);

  // generate I2C STOP condition
  I2C1->CR1 |= I2C_CR1_STOP;

  // return data    
  return (ret);
}

// I2C write word
void i2c_write_word (uint8_t reg, uint16_t dat)
{
  // generate I2C START condition
  I2C1->CR1 |= I2C_CR1_START;
  
  // wait for START condition generated
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_SB) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // -- first, write register address to be written --
  
  // write I2C write address (= first byte after START condition)
  I2C1->DR = LTC4162_WADR; 
  
  // wait for I2C address sent
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_ADDR) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // read SR2 to clear ADDR bit
  (void)I2C1->SR2;
  
  // write register number
  I2C1->DR = reg;
  
  // wait while busy
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_TXE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // -- then, write register contents --
  
  // write data
  I2C1->DR = (uint8_t)(dat & 0x00ff);
  
  // wait while busy
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_TXE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // wait for byte transmit finished
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_BTF) == 0) 
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // write data
  I2C1->DR = (uint8_t)((dat & 0xff00) >> 8);
  
  // wait while busy
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_TXE) == 0)
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }
  
  // wait for byte transmit finished
  to_cnt = 0;
  
  while ((I2C1->SR1 & I2C_SR1_BTF) == 0) 
  {
    if (to_cnt++ > I2C_TIMEOUT)
    {
      debug (I2C_ERR, "\E[35m#I2C ERROR: %s, line %d\n\E[30m", __FILE__, __LINE__);
      return;
    }
    else
      usleep (50);
  }

  // generate I2C STOP condition
  I2C1->CR1 |= I2C_CR1_STOP;
}

// dump I2C1 registers
void i2c1_dump_registers (void)
{
  printf2 ("I2C1_CR1    : 0x%08x\n", I2C1->CR1);
  printf2 ("I2C1_CR2    : 0x%08x\n", I2C1->CR2);
//printf2 ("I2C1_OAR1   : 0x%08x\n", I2C1->OAR1);
//printf2 ("I2C1_OAR2   : 0x%08x\n", I2C1->OAR2);
  printf2 ("I2C1_DR     : 0x%08x\n", I2C1->DR);
  printf2 ("I2C1_SR1    : 0x%08x\n", I2C1->SR1);
  printf2 ("I2C1_SR2    : 0x%08x\n", I2C1->SR2);
  printf2 ("I2C1_CCR    : 0x%08x\n", I2C1->CCR);
  printf2 ("I2C1_TRISE  : 0x%08x\n", I2C1->TRISE);
  printf2 ("\n");
}
