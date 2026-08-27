// file    : power.c
// author  : rb
// purpose : STM32F042xx power control routines
// date    : 180609
// last    : 180609
//

#include "includes.h"

// <> move to header file later or remove?
#define PWR_MCU_CLEAR      (0b11 << PWR_CR_VOS_Pos)
#define PWR_MCU_1V8        (0b01 << PWR_CR_VOS_Pos)
#define PWR_MCU_1V5        (0b10 << PWR_CR_VOS_Pos)
#define PWR_MCU_1V2        (0b11 << PWR_CR_VOS_Pos)

// setup power control
void init_power (void)
{
  // debug mode off
  DBGMCU->CR     = 0;
  DBGMCU->APB1FZ = 0;
  DBGMCU->APB2FZ = 0;
}

