// file    : i2c.h
// author  : rb
// purpose : header file i2c.h
// date    : 181010
// last    : 181012

#ifndef __I2C_H__
#define __I2C_H__

// -- prototypes

void init_i2c  (void);
void init_i2c1 (void);

void i2c1_write (uint8_t adr, uint8_t *buf, uint8_t len);
void i2c1_read  (uint8_t adr, uint8_t *buf, uint8_t len);

void i2c1_dump_registers (void);

#endif
