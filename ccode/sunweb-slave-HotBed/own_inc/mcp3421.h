// file    : mcp3421.h
// author  : rb
// purpose : header file for mcp3421.c
// date    : 160819
// last    : 18101

#ifndef __MCP3421_H__
#define __MCP3421_H__

// -- defines
//#define MCP3421A0_READ     0xd1      // MCP3421A0 I2C read address
//#define MCP3421A0_WRITE    0xd0      // MCP3421A0 I2C write address

#define MCP3421A0          0xd0        // MCP3421A0 I2C address (R/W bit not needed for STM32)
#define MCP3421A1          0xd2        // MCP3421A1 I2C address (R/W bit not needed for STM32)
#define MCP3421A3          0xd6        // MCP3421A3 I2C address (R/W bit not needed for STM32)

// configuration register
#define MCP3421_GAIN1   (0 << 0)       // PGA gain 1x
#define MCP3421_GAIN2   (1 << 0)       // PGA gain 2x
#define MCP3421_GAIN4   (2 << 0)       // PGA gain 4x
#define MCP3421_GAIN8   (3 << 0)       // PGA gain 8x

#define MCP3421_12BITS  (0 << 2)       //  240 SPS, 12-bit mode
#define MCP3421_14BITS  (1 << 2)       //   60 SPS, 14-bit mode
#define MCP3421_16BITS  (2 << 2)       //   15 SPS, 16-bit mode
#define MCP3421_18BITS  (3 << 2)       // 3.75 SPS, 18-bit mode

#define MCP3421_OC      (1 << 4)       // 0: one-shot, 1: continuous conversion

#define MCP3421_RDY_L   (1 << 7)       // conversion ready (active low)

// -- prototypes
void init_mcp3421 (void);

uint16_t mcp3421_read  (uint8_t adr);

#endif
