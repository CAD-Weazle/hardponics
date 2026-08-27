// file    : spi.c
// author  : ao/rb
// purpose : SPI port routines for STM32L
// date    : 170810
// last    : 230112
//

#include "includes.h"

// set up SPI
void init_spi (void)
{
  spi1_init ();  // OLED displat
  spi2_init ();  // serial LEDs
//spi3_init ();
}

// init SPI1
void spi1_init (void)
{
  // enable clock
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  // set up SPI1
  SPI1->CR1 = SPI_CR1_SPE       // SPI enable
            | SPI_CR1_MSTR      // SPI master mode
            | SPI_CR1_SSM       // slave select management (0: via SNN pin  / 1: via internal register)
            | SPI_CR1_SSI       // internal slave select   (0: SPI is slave / 1: SPI is master) 
          //| SPI_CR1_BR_2      // baud rate control: 0b001 = Fpclk/4
          //| SPI_CR1_BR_1 
            | SPI_CR1_BR_0 
          //| SPI_CR1_POL       // 0: SCK lo when idle / 1: SCK hi when idle
          //| SPI_CR1_CPHA      // 0: first SCK edge captures / 1: second SCK edge captures
          //| SPI_CR1_DFF       // 0: 8-bit data / 1: 16-bit data
            ;
}

// init SPI2
void spi2_init (void)
{
  // enable clock
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  // set up SPI2
  SPI2->CR1 = SPI_CR1_SPE  |    // SPI enable
              SPI_CR1_MSTR |    // SPI master mode
              SPI_CR1_SSM  |    // slave select management (0: via SNN pin  / 1: via internal register)
              SPI_CR1_SSI  |    // internal slave select   (0: SPI is slave / 1: SPI is master) 

// org      //SPI_CR1_BR_2 |    // baud rate control: 0b010 = Fpclk/8
// org        SPI_CR1_BR_1 |
// org      //SPI_CR1_BR_0 |

            //SPI_CR1_BR_2 |    // baud rate control: 0b001 = Fpclk/4
            //SPI_CR1_BR_1 |
              SPI_CR1_BR_0 |

            //SPI_CR1_POL  |    // 0: SCK lo when idle / 1: SCK hi when idle
              SPI_CR1_CPHA |    // 0: first SCK edge captures / 1: second SCK edge captures
              SPI_CR1_DFF;      // 0: 8-bit data / 1: 16-bit data
}

/* no SPI3 in STM32L151XBA 
// init SPI3
void spi3_init (void)
{
  // enable clock
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  // set up SPI3
  SPI3->CR1 = SPI_CR1_SPE  |    // SPI enable
              SPI_CR1_MSTR |    // SPI master mode
              SPI_CR1_SSM  |    // slave select management (0: via SNN pin  / 1: via internal register)
              SPI_CR1_SSI  |    // internal slave select   (0: SPI is slave / 1: SPI is master) 
            //SPI_CR1_BR_2 |    // baud rate control: 0b010 = Fpclk/8
              SPI_CR1_BR_1 |
            //SPI_CR1_BR_0 |
            //SPI_CR1_POL  |    // 0: SCK lo when idle / 1: SCK hi when idle
              SPI_CR1_CPHA |    // 0: first SCK edge captures / 1: second SCK edge captures
              SPI_CR1_DFF;      // 0: 8-bit data / 1: 16-bit data
}
*/

// SPI1 8-bit read/write  
uint8_t spi1_rw_byte (uint8_t data)
{
  SPI1->DR = data;

  while (!(SPI1->SR & SPI_SR_RXNE))
    continue;

  return (SPI1->DR);
}

// SPI1 8-bit write
void spi1_write_byte (uint8_t data)
{
  spi1_rw_byte (data);
}

// SPI1 8-bit read
uint8_t spi1_read_byte (void)
{
  return spi1_rw_byte (0x00);
}

// SPI1 16-bit write
void spi1_write_word (int data)
{
  while (!(SPI1->SR & SPI_SR_TXE))
    ;

  SPI1->DR = data;
}

// SPI2 16-bit write
void spi2_write_word (int data)
{
  while (!(SPI2->SR & SPI_SR_TXE))
    ;

  SPI2->DR = data;
}

/* no SPI3 in STM32L151XBA 
// SPI3 16-bit write
void spi3_write_word (int data)
{
  while (!(SPI3->SR & SPI_SR_TXE))
    ;

  SPI3->DR = data;
}
*/

