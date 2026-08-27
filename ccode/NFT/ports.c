// file    : ports.c
// author  : ao/rb
// purpose : STM32L1xx IO port routines
// date    : 170810
// last    : 230203
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
  [ 0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // PAR       - ADC[0]
  [ 1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // EC        - ADC[1]
  [ 2] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART2}, // SER_TXD   - UART2   
  [ 3] = {GPIO_MODE_AF , GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART2}, // SER_RXD   - UART2   
  [ 4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // EC_DAC    - DAC[1]
  [ 5] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF5_SPI1},   // OLED_SCK  - SPI1
  [ 6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_OD, GPIO_NOPULL},                  // EC_NTC    - ADC[6]
  [ 7] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF5_SPI1},   // OLED_MOSI - SPI1
  [ 8] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_NOPULL},                  // SHT_DAT   - SHT15 data (OD)
//[ 8] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF0_MCO},    // MCO       (dev only)
  [ 9] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SHT_CLK   - SHT15 clock (PP)
  [10] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // SENSOR_PWR_L
  [11] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // OLED_CS_L
  [12] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLUP},                  // OLED_CD
  [13] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF0_SWJ},    // SWD_IO    - prog port
  [14] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_PULLUP, GPIO_AF0_SWJ},    // SWD_CLK   - prog port
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // REF_EC
};

// GPIO Port A settings - STOP mode
static const struct port_info stopa[16] = 
{
   [0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [2] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [3] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [5] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [7] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [8] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
   [9] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
  [10] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_NOPULL},                   //
  [11] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
  [12] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 //
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP},                   // SWD_IO    - prog port
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLUP},                   // SWD_CLK   - prog port
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW, GPIO_OTYPE_PP, GPIO_PULLDOWN},                 // 
};

// GPIO Port B settings
static const struct port_info portb[16] = 
{
  [ 0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VBAT      - ADC8 
  [ 1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // IBAT      - ADC9
  [ 2] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // VGND
  [ 3] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // PUMP1
  [ 4] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SAMPLE1
  [ 5] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // BUTTON0
  [ 6] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // BUTTON1
  [ 7] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // BUTTON2
  [ 8] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // PUMP0
  [ 9] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_OD, GPIO_PULLUP},                  // <> fix this
  [10] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART3}, // WIFI_TXD  - UART3
  [11] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF7_USART3}, // WIFI_RXD  - UART3
  [12] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // SAMPLE2
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL},                  // BUTTON3
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // VSOL      - ADC20
  [15] = {GPIO_MODE_AF,  GPIO_SPEED_FREQ_HIGH, GPIO_OTYPE_PP, GPIO_NOPULL, GPIO_AF5_SPI2},   // LED_DIN   - MOSI2
};

// GPIO Port B settings - STOP mode
static const struct port_info stopb[16] = 
{
   [0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
   [1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
   [2] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [3] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [4] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [5] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [6] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
   [7] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
   [8] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
   [9] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [10] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  //
  [11] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  //
  [12] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [13] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
};

// GPIO Port C settings
static const struct port_info portc[16] = 
{
  [13] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // SPEED0 <> now IBAT sign
  [14] = {GPIO_MODE_IN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_NOPULL},                  // SPEED1
  [15] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_OD, GPIO_PULLUP},                  // WIFI_EN
};

// GPIO Port C settings - STOP mode
static const struct port_info stopc[16] = 
{
  [13] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
  [14] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // 
  [15] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                //
};

// GPIO Port H settings
static const struct port_info porth[16] = 
{
  [ 0] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // LED_RED
  [ 1] = {GPIO_MODE_OUT, GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN},                // LED_GRN
};

// GPIO Port H settings - STOP mode
static const struct port_info stoph[16] = 
{
   [0] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN },               //
   [1] = {GPIO_MODE_AN,  GPIO_SPEED_FREQ_LOW,  GPIO_OTYPE_PP, GPIO_PULLDOWN },               //
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

  GPIOx->AFR[0]  = afr;           // set AFR registers first (no spurs on UART Tx)
  GPIOx->AFR[1]  = afr >> 32;     // set AFR registers first (no spurs on UART Tx)
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
                 RCC_AHBENR_GPIOBEN |
                 RCC_AHBENR_GPIOCEN |
                 RCC_AHBENR_GPIOHEN;

  // set-up GPIO ports
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);
  init_port (GPIOC, portc);
  init_port (GPIOH, porth);

  // LEDs off
  led_red_off ();
  led_grn_off ();

  // disconnect input dividers from GND
  gnd_disconnect ();

  // ESP32 active
  gpio_set (GPIOC, WIFI_EN);
}

// set GPIO to low power inputs in STOP mode
void ports_stop (void) 
{
  init_port (GPIOA, stopa);
  init_port (GPIOB, stopb);
  init_port (GPIOC, stopc);
  init_port (GPIOH, stoph);
}

// red LED routines
void led_red_on (void)
{
  gpio_set (GPIOH, LED_RED);
}

void led_red_off (void)
{
  gpio_clr (GPIOH, LED_RED);
}

void led_red_toggle (void)
{
  if (GPIOH->ODR & (1 << LED_RED))
    gpio_clr (GPIOH, LED_RED);
  else
    gpio_set (GPIOH, LED_RED);
}

void led_red_flash (uint16_t delay)
{
  gpio_set (GPIOH, LED_RED);
  msleep (delay);
  gpio_clr (GPIOH, LED_RED);
}

void led_grn_on (void)
{
  gpio_set (GPIOH, LED_GRN);
}

void led_grn_off (void)
{
  gpio_clr (GPIOH, LED_GRN);
}

void led_grn_toggle (void)
{
  if (GPIOH->ODR & (1 << LED_GRN))
    gpio_clr (GPIOH, LED_GRN);
  else
    gpio_set (GPIOH, LED_GRN);
}

void led_grn_flash (uint16_t delay)
{
  gpio_set (GPIOH, LED_GRN);
  msleep (delay);
  gpio_clr (GPIOH, LED_GRN);
}

// apply power to SHT15 sensor
void sht15_power_on (void)
{
  gpio_clr (GPIOA, SHT_PWR_L);
}

// remove power from SHT15 sensor
void sht15_power_off (void)
{
  gpio_set (GPIOA, SHT_PWR_L);
}

// apply power to EC circuit
void ec_power_on (void)
{
  gpio_set (GPIOA, REF_EC);
}

// remove power from EC circuit
void ec_power_off (void)
{
  gpio_clr (GPIOA, REF_EC);
}

// water pump control
void pump0_on (void)
{
  gpio_set (GPIOB, PUMP0);
}

void pump0_off (void)
{
  gpio_clr (GPIOB, PUMP0);
}

void pump1_on (void)
{
  gpio_set (GPIOB, PUMP1);
}

void pump1_off (void)
{
  gpio_clr (GPIOB, PUMP1);
}

// VSOL/VACCU voltage dividers 
void gnd_connect (void)
{
  gpio_clr (GPIOB, VGND);
}

void gnd_disconnect (void)
{
  gpio_set (GPIOB, VGND);
}
