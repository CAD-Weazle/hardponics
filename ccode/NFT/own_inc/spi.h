// file    : spi.h
// author  : rb
// purpose : header file for spi.c
// date    : 170820
// last    : 230112
//

#ifndef _SPI_H_
#define _SPI_H_

// -- prototypes
void init_spi (void);

void spi1_init (void);
void spi2_init (void);
void spi3_init (void);

void spi1_write_word (int data);
void spi2_write_word (int data);
void spi3_write_word (int data);

uint8_t spi1_rw_byte    (uint8_t data);
void    spi1_write_byte (uint8_t data);
uint8_t spi1_read_byte  (void);


#endif


