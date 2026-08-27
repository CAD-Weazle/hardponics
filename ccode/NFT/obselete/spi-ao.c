/*
 * spi.c
 *
 * (C) Copyright Arlet Ottens 2014, <arlet@c-scape.nl>
 */

#include <stdlib.h>
#include <stdint.h>
#include "gpio.h"
#include "printf.h"
#include "spi.h"
#include "stm.h"

static const uint16_t ws[16] = 
{ 
    0x8888, 0x888e, 0x88e8, 0x88ee,
    0x8e88, 0x8e8e, 0x8ee8, 0x8eee,
    0xe888, 0xe88e, 0xe8e8, 0xe8ee,
    0xee88, 0xee8e, 0xeee8, 0xeeee,
};

/*
 * spi2_init: initialize SPI2 for AD7793 communication
 */
void init_spi( void )
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    SPI2->CR2 = SPI_CR2_SSOE;
    SPI2->CR1 = (2 << 3) | SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_SPE | SPI_CR1_DFF;
}

void spi_write_word( int data )
{
    while( !(SPI2->SR & SPI_SR_TXE) )
       ;
    SPI2->DR = data;
}

void spi_write_ws( uint32_t col )
{
    int i;

    for( i = 20; i >= 0; i -= 4 )
        spi_write_word( ws[(col >> i) & 15] );
}
