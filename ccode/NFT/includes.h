//#include <stdlib.h>
//#include "stm32l1xx_hal_rcc.h"
//#include "stm32l1xx_hal_gpio_ex.h"
//#include "stm32l1xx_hal_uart.h"

#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>  // needed for 'atof'

#include "stm32l1xx.h"
#include "stm32l1xx_hal_gpio.h"

#include "version.h"

#include "pll.h"
#include "ports.h"
#include "ticker.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "printf.h"
#include "printf2.h"
#include "printf3.h"
#include "dump.h"
#include "cli.h"
#include "eeprom.h"
#include "spi.h"
#include "adc.h"
#include "dac.h"
#include "ws2815.h"
#include "esp32.h"
#include "sht15.h"
#include "touch.h"
#include "ec.h"
#include "ntc.h"
#include "oled.h"
#include "control.h"

