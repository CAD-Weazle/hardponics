// file    : ports.c
// author  : ao/rb
// purpose : IO port routines
// board   : STM32F042F6P6 SimSun PID - HEATER Driver PCB
// date    : 170810
// last    : 181010
//
// note    : make sure GPIO is in mode GPIO_SPEED_FREQ_HIGH for fast signals
//           (IO in slow mode will block fast signals!1!)

#include "includes.h"

// used with STTM32L151 <> move later
#define  GPIO_MODE_IN        (0x00000000u)       // GPIO set to input
#define  GPIO_MODE_OUT       (0x00000001u)       // GPIO set to output
#define  GPIO_MODE_AF        (0x00000002u)       // GPIO set to alternate function
#define  GPIO_MODE_AN        (0x00000003u)       // GPIO set to analog mode

#define GPIO_OTYPE_PP        (0x00000000u)       // output push-pull
#define GPIO_OTYPE_OD        (0x00000001u)       // output open-drain

// from STM32F042 '...hal_gpio.h'
#define  GPIO_MODE_INPUT                        (0x00000000U)   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    (0x00000001U)   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    (0x00000011U)   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        (0x00000002U)   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        (0x00000012U)   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_ANALOG                       (0x00000003U)   /*!< Analog Mode  */  
#define  GPIO_MODE_IT_RISING                    (0x10110000U)   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   (0x10210000U)   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            (0x10310000U)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
#define  GPIO_MODE_EVT_RISING                   (0x10120000U)   /*!< External Event Mode with Rising edge trigger detection               */
#define  GPIO_MODE_EVT_FALLING                  (0x10220000U)   /*!< External Event Mode with Falling edge trigger detection              */
#define  GPIO_MODE_EVT_RISING_FALLING           (0x10320000U)   /*!< External Event Mode with Rising/Falling edge trigger detection       */


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
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // NU

  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF2_TIM2},   // HEATER    - Timer 2 PWM out
//[ 1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // HEATER    - GPIO <> test 

  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF1_USART2}, // TXD_232
  [ 3] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF1_USART2}, // RXD_232
  [ 4] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF4_TIM14},  // FAN       - Timer 14 PWM out
  [ 5] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // DIR_485
  [ 6] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN, GPIO_AF5_TIM16},  // LEDS      - Timer 16 PWM out
  [ 7] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                  // LED_RED
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_MCO},    // MCO       * Nucleo-32 only
  [ 9] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF1_USART1}, // RS_485    - TXD1
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF1_USART1}, // RS_485    - RXD1
  [11] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [12] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_SWDIO},  // SWDIO
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL,   GPIO_AF0_SWCLK},  // SWCLK
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
};

// GPIO Port B settings - normal mode
static const struct port_info portb[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // LED_GRN
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                    // NU        * Nucleo-32 only
};

// GPIO Port F settings - normal mode
static const struct port_info portf[16] = 
{
  [ 0] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_NOPULL, GPIO_AF1_I2C1},     // SDA
  [ 1] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_NOPULL, GPIO_AF1_I2C1},     // SCL
};

static void init_port (GPIO_TypeDef *GPIOx, const struct port_info *port);

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

// green LED routines
void led_grn_on (void)
{
  GPIOB->ODR |= GPIO_ODR_1;
}

void led_grn_off (void)
{
 GPIOB->ODR &= ~GPIO_ODR_1; 
}

void led_grn_toggle (void)
{
  if (GPIOB->ODR & GPIO_ODR_1)
    GPIOB->ODR &= ~GPIO_ODR_1;
  else
    GPIOB->ODR |= GPIO_ODR_1;
}

void led_grn_flash (uint16_t delay)
{
  GPIOB->ODR |= GPIO_ODR_1;
  msleep (delay);  
  GPIOB->ODR &= ~GPIO_ODR_1;
}

// red LED routines
void led_red_on (void)
{
  GPIOA->ODR |= GPIO_ODR_7;
}

void led_red_off (void)
{
 GPIOA->ODR &= ~GPIO_ODR_7; 
}

void led_red_toggle (void)
{
  if (GPIOA->ODR & GPIO_ODR_7)
    GPIOA->ODR &= ~GPIO_ODR_7;
  else
    GPIOA->ODR |= GPIO_ODR_7;
}

void led_red_flash (uint16_t delay)
{
  GPIOA->ODR |= GPIO_ODR_7;
  msleep (delay);  
  GPIOA->ODR &= ~GPIO_ODR_7;
}

// RS-485 routines
void rs485_dir (uint16_t dir)
{
  if (dir)
    GPIOA->ODR |= GPIO_ODR_5;     // RS-485 transmit
  else
    GPIOA->ODR &= ~GPIO_ODR_5;    // RS-485 receive
}
