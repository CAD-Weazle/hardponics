// file    : eeprom.h
// author  : rb
// purpose : header file for eeprom.c
// date    : 180820
// last    : 200713
//

#ifndef _EEPROM_H_
#define _EEPROM_H_

// -- defines
#define FLASH_PEKEY1                (0x89abcdefU)     // FLASH program erase key1 
#define FLASH_PEKEY2                (0x02030405U)     // FLASH program erase key2

#define EEPROM_BASE                   0x08080000      // STM32L1xx EEPROM start addres
#define EEPROM_SIZE                        0xfff      // STM32L1xx EEPROM size
#define EEPROM_BASE_CAPS             EEPROM_BASE      // soil sensors parameters EEPROM base address 
#define EEPROM_BASE_NTC       (EEPROM_BASE + 32)      // NTC sensors parameters EEPROM base address 

// -- prototypes
void    eeprom_write_long (uint32_t adr, int32_t dat);
int32_t eeprom_read_long  (uint32_t adr);

void eeprom_dump (uint16_t len);

void eeprom_unlock (void); 
void eeprom_lock   (void); 

#endif