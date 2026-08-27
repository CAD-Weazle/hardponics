// file    : i2c.h
// author  : rb
// purpose : header file i2c.c
// date    : 221103
// last    : 221103

#ifndef __I2C__
#define __I2C_H__

// -- defines
#define I2C_TIMEOUT                           100  // I2C timeout
#define I2C_WAITDELAY                         100  // I2C timeout wait delay [us]

// -- prototypes
void init_i2c  (void);

void init_i2c1 (void);

uint16_t i2c_read_word  (uint8_t reg);
void     i2c_write_word (uint8_t reg, uint16_t dat);

void i2c1_dump_registers (void);

#endif


