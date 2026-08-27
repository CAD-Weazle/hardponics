// file    : mcp3421.c
// author  : rb
// purpose : STM32 I2C routines for MCP3421 18-bit ADC
// date    : 160819
// last    : 181012

#include "includes.h"

// set up MCP3421 ADCs
void init_mcp3421 (void)
{
  uint8_t dat;

  dat = (MCP3421_OC      |         // continuous mode
         MCP3421_GAIN8   |         // set gain
         MCP3421_16BITS);          // set word size

  i2c1_write (MCP3421A0, &dat, 1);
  i2c1_write (MCP3421A1, &dat, 1);
  i2c1_write (MCP3421A3, &dat, 1);
}

// read MCP3421 ADC - 16-bit mode
uint16_t mcp3421_read (uint8_t adr)
{
  uint8_t buf[3];

  i2c1_read (adr, buf, 3);

  return ((uint16_t)buf[0] << 8) + buf[1];
}

