// file    : ports.c
// author  : ao/rb
// purpose : STM32L1xx IO port routines
// date    : 170810
// last    : 220613
//

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


// GPIO Port A settings
static const struct port_info porta[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // BLE_STATUS
  [ 1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // NTC_PWR_L        
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF7_USART2}, // SER_TXD
  [ 3] = {GPIO_MODE_AF , GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF7_USART2}, // SER_RXD
  [ 4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NTC0 - ADC[4]
  [ 5] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NTC1 - ADC[5]
  [ 6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NTC2 - ADC[6]
  [ 7] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NTC3 - ADC[7]
  [ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF0_MCO},    // CLOCK_OUT  - MCO (select FREQ_HIGH!!!1!)
  [ 9] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // LED_RED
  [10] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU
  [11] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_OD, GPIO_NOPULL},                  // VGND       - open drain
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF0_SWJ},    // SWD_IO
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF0_SWJ},    // SWD_CLK 
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NU
};

// GPIO Port A settings - STOP mode
static const struct port_info stopa[16] = 
{
  [ 0] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP},                   // BLE_TXI_L  - keep input for EXTI event 
  [ 1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP},                   // NTC_PWR_L  - keep high to close p-FET
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP,  GPIO_AF7_USART2}, // SER_TXD    - don't change function, keep TxD high
  [ 3] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP},                   // SER_RXD
  [ 4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 5] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 7] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 8] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [ 9] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [10] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_NOPULL},                   // VGND       - open drain
  [11] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [12] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // SWD_IO
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // SWD_CLK
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
};

// GPIO Port B settings
static const struct port_info portb[16] = 
{
  [ 0] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // BLE_TRAN     - 0: thru mode / 1: command mode
  [ 1] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU
  [ 2] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU
  [ 3] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU 
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // SOIL2 - rewired
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // CSAMPLE
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SOIL0
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SOIL1
  [ 8] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SOIL2 - wrong GPIO
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // NU
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART3}, // BLE_RXD      - UART3
  [11] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART3}, // BLE_TXD      - UART3
  [12] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // BLE_RESET_L
  [13] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VBAT
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VSYS
  [15] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NU
};

// GPIO Port B settings - STOP mode
static const struct port_info stopb[16] = 
{
   [0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // BLE_TRAN     - keep low
   [1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [2] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [3] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [5] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [7] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // SHT_DAT 
   [8] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // SHT_CLK
   [9] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [10] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // BLE_RXD      - UART3
  [11] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // BLE_TXD      - UART3
  [12] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // BLE_RESET_L  - keep high
  [13] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VBAT
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VSYS
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
};

// GPIO Port C settings - STOP mode
static const struct port_info stopc[16] = 
{
  [13] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NC
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NC 
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // NC
};

// GPIO Port H settings - STOP mode
static const struct port_info stoph[16] = 
{
   [0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN },               // NC
   [1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN },               // NC
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

  GPIOx->AFR[0]  = afr;           // set AFR registers first
  GPIOx->AFR[1]  = afr >> 32;     // set AFR registers first
  GPIOx->MODER   = mode;
  GPIOx->OSPEEDR = speed;
  GPIOx->PUPDR   = pupd;
  GPIOx->OTYPER  = type;
}

// initialize GPIO ports
void init_ports (void) 
{
  // enable clocks for GPIO ports
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN |
                 RCC_AHBENR_GPIOBEN;

  // set-up GPIO ports
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);

  // all LEDs off
  led_red_off ();
}

// set GPIO to low power inputs in STOP mode
void ports_stop (void) 
{
  init_port (GPIOA, stopa);
  init_port (GPIOB, stopb);
  init_port (GPIOC, stopc);
  init_port (GPIOH, stoph);
}

// set GPIO normal operation
void ports_resume (void) 
{
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);
}

// red LED routines
void led_red_on (void)
{
  gpio_set (GPIOA, LED_RED);
}

void led_red_off (void)
{
  gpio_clr (GPIOA, LED_RED);
}

void led_red_toggle (void)
{
  if (GPIOA->ODR & (1 << LED_RED))
    gpio_clr (GPIOA, LED_RED);
  else
    gpio_set (GPIOA, LED_RED);
}

void led_red_flash (uint16_t delay)
{
  gpio_set (GPIOA, LED_RED);
  msleep (delay);
  gpio_clr (GPIOA, LED_RED);
}

// connect voltage dividers ground to GND - open drain output
void vgnd_enable (void)
{
  gpio_clr (GPIOA, VGND);
}

// disconnect voltage dividers ground from GND - open drain output
void vgnd_disable (void)
{
  gpio_set (GPIOA, VGND);
}


// connect VDDA to NTC sensors
void ntc_enable (void)
{
  gpio_clr (GPIOA, NTC_PWR_L);
}

// remove VDDA power from NTC sensors
void ntc_disable (void)
{
  gpio_set (GPIOA, NTC_PWR_L);
}









