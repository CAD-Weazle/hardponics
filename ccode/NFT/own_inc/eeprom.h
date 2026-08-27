// file    : eeprom.h
// author  : rb
// purpose : header file for eeprom.c
// date    : 180820
// last    : 230124
//

#ifndef _EEPROM_H_
#define _EEPROM_H_

// -- defines
#define FLASH_PEKEY1                (0x89abcdefU)     // FLASH program erase key1 
#define FLASH_PEKEY2                (0x02030405U)     // FLASH program erase key2

#define EEPROM_BASE       ((uint32_t)0x08080000)      // STM32L1xx EEPROM start addres
#define EEPROM_SIZE                        0xfff      // STM32L1xx EEPROM size
#define EEPROM_BASE_NTC            (EEPROM_BASE)      // EC sensor NTC parameter EEPROM base address 
#define EEPROM_BASE_EC        (EEPROM_BASE + 10)      // EC sensor cell constant EEPROM base address 
#define EEPROM_BASE_PUMP      (EEPROM_BASE + 20)      // PUMP on/off timing parameters

// -- prototypes
void    eeprom_write_long (uint32_t adr, int32_t dat);
int32_t eeprom_read_long  (uint32_t adr);

void  eeprom_write_float (uint32_t adr, float fdat);
float eeprom_read_float  (uint32_t adr);

void eeprom_unlock (void); 
void eeprom_lock   (void); 

void eeprom_dump (uint16_t len);

#endif