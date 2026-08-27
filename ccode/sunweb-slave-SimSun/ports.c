// file    : ports.c
// author  : ao/rb
// purpose : IO port routines - STM32F042xx
// date    : 170810
// last    : 190109
//
// note    : make sure GPIO is in mode GPIO_SPEED_FREQ_HIGH for fast signals
//           (IO in slow mode will block fast signals!1!)

#include "includes.h"

// GPIO bit fields
static const struct port_info
{
  uint16_t mode : 2;
  uint16_t speed: 2;
  uint16_t od   : 1;
  uint16_t pupd : 2;
  uint16_t afr  : 4;
} port_info_type; 


// GPIO Port A settings - normal mode
static const struct port_info porta[16] = 
{
  [ 0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // TEMP      - ADC0 ('additional function')
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF2_TIM2},   // DRED_PWM0 - Timer 2 PWM out
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF1_USART2}, // TXD_232
  [ 3] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF1_USART2}, // RXD_232
  [ 4] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF4_TIM14},  // DRED_PWM1 - Timer 14 PWM out
  [ 5] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // DIR_485
  [ 6] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF5_TIM16},  // DRED_PWM2 - Timer 16 PWM out
  [ 7] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF5_TIM17},  // RBLU_PWM0 - Timer 17 PWM out
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_MCO},    // MCO       - Nucleo-32 only
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF1_USART1}, // RS_485    - TXD1
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF1_USART1}, // RS_485    - RXD1
  [11] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        - Nucleo-32 only
  [12] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        - Nucleo-32 only
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_SWDIO},  // SWDIO
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_SWCLK},  // SWCLK
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        - Nucleo-32 only
};

// GPIO Port B settings - normal mode
static const struct port_info portb[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // FRED_PWM0
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 3] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // LED grn            - Nucleo-32
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU                 - Nucleo-32 only
};

// GPIO Port F settings - normal mode
static const struct port_info portf[16] = 
{
   [0] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // LED_RED
   [1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // LED_GRN
};


// init GPIO Port
static void init_port (GPIO_TypeDef *GPIOx, const struct port_info *port)
{
  int i;
  uint32_t mode = 0, speed = 0, type = 0, pupd = 0;
  uint64_t afr = 0;

  for (i = 0; i < 16; i++)
  {
    mode  |= (uint32_t) port[i].mode  << i*2;
    speed |= (uint32_t) port[i].speed << i*2;
    pupd  |= (uint32_t) port[i].pupd  << i*2;
    type  |= (uint32_t) port[i].od    << i;
    afr   |= (uint64_t) port[i].afr   << i*4;
  }

  GPIOx->MODER   = mode;
  GPIOx->OSPEEDR = speed;
  GPIOx->PUPDR   = pupd;
  GPIOx->OTYPER  = type;
  GPIOx->AFR[0]  = afr;
  GPIOx->AFR[1]  = afr >> 32;
}
 
// initialize GPIO ports
void init_ports (void) 
{
  // enable clocks for GPIO ports
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN |
                 RCC_AHBENR_GPIOBEN |
                 RCC_AHBENR_GPIOFEN ;

  // set up GPIO ports
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);
  init_port (GPIOF, portf);
}

// -- red LED routines
void led_red_on (void)
{
  GPIOF->ODR |= GPIO_ODR_0;
}

void led_red_off (void)
{
 GPIOF->ODR &= ~GPIO_ODR_0; 
}

void led_red_toggle (void)
{
  if (GPIOF->ODR & GPIO_ODR_0)
    GPIOF->ODR &= ~GPIO_ODR_0;
  else
    GPIOF->ODR |= GPIO_ODR_0;
}

void led_red_flash (uint16_t delay)
{
  GPIOF->ODR |= GPIO_ODR_0;
  msleep (delay);  
  GPIOF->ODR &= ~GPIO_ODR_0;
}
// -- green LED routines
void led_grn_on (void)
{
  GPIOF->ODR |= GPIO_ODR_1;
  GPIOB->ODR |= GPIO_ODR_3;       // <> Nucleo-32, remove later
}

void led_grn_off (void)
{
 GPIOF->ODR &= ~GPIO_ODR_1; 
 GPIOB->ODR &= ~GPIO_ODR_3;       // <> Nucleo-32, remove later
}

void led_grn_toggle (void)
{
  if (GPIOF->ODR & GPIO_ODR_1)
  {
    GPIOF->ODR &= ~GPIO_ODR_1;
    GPIOB->ODR &= ~GPIO_ODR_3;    // <> Nucleo-32, remove later
  }
  else
  {
    GPIOF->ODR |= GPIO_ODR_1;
    GPIOB->ODR |= GPIO_ODR_3;     // <> Nucleo-32, remove later
  }
}

void led_grn_flash (uint16_t delay)
{
  GPIOF->ODR |= GPIO_ODR_1;
  GPIOB->ODR |= GPIO_ODR_3;       // <> Nucleo-32, remove later
  msleep (delay);  
  GPIOB->ODR &= ~GPIO_ODR_3;      // <> Nucleo-32, remove later
  GPIOF->ODR &= ~GPIO_ODR_1;
}

// set RS-485 direction
void rs485_dir (uint16_t dir)
{
  if (dir)
    GPIOA->ODR |= GPIO_ODR_5;     // RS-485 transmit
  else
    GPIOA->ODR &= ~GPIO_ODR_5;    // RS-485 receive
}

// SimSun Far Red on
void led_fred_on (void)
{
  GPIOB->ODR |= GPIO_ODR_1;
}

// SimSun Far Red off
void led_fred_off (void)
{
 GPIOB->ODR &= ~GPIO_ODR_1; 
}


