// file    : eeprom.c
// author  : rb
// purpose : FLASH memory routines - STM32L1xx
// date    : 180820
// last    : 221126
//

#include "includes.h"

// EEPROM write long (32 bit)
void eeprom_write_long (uint32_t adr, int32_t dat)
{
  // sanity check
  if ((adr < EEPROM_BASE) || (adr > (EEPROM_BASE + EEPROM_SIZE)))
    return;
  
  // unlock EEPROM
  eeprom_unlock ();

  // wait for a EEPROM busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;

  // write data
  *(volatile int32_t *)adr = dat;
 
  // lock EEPROM
  eeprom_lock ();
}

// EEPROM read word (32 bit)
int32_t eeprom_read_long (uint32_t adr) 
{
  // sanity check
  if ((adr < EEPROM_BASE) || (adr > (EEPROM_BASE + EEPROM_SIZE)))
    return 0;

  return (*(volatile int32_t *)adr);
}

// EEPROM write float
void eeprom_write_float (uint32_t adr, float fdat)
{
  eeprom_write_long (EEPROM_BASE, *(uint32_t *)&fdat);
}

// EEPROM read float
float eeprom_read_float (uint32_t adr) 
{
  return (*(volatile float *)adr);
}

// unlock Data EEPROM block & FLASH_PECR
void eeprom_unlock (void) 
{
  // check for locked EEPROM
  if (FLASH->PECR & FLASH_PECR_PELOCK) 
  {
    // write magic words
    FLASH->PEKEYR = FLASH_PEKEY1;
    FLASH->PEKEYR = FLASH_PEKEY2;
  }
}

// lock Data EEPROM block & FLASH_PECR
void eeprom_lock (void) 
{
  FLASH->PECR |= FLASH_PECR_PELOCK;
}

// dump EEPROM memory
void eeprom_dump (uint16_t len)
{
  // sanity check
  if (len > EEPROM_SIZE)
    return;
	
  dump_ascii ((char *)EEPROM_BASE, len);
}

// dump FLASH registers
void flash_dump_registers (void)
{
  printf2 ("FLASH_ACR     : 0x%08lx\n", FLASH->ACR);
  printf2 ("FLASH_PECR    : 0x%08lx\n", FLASH->PECR);
  printf2 ("FLASH_PDKEYR  : 0x%08lx\n", FLASH->PDKEYR);
  printf2 ("FLASH_PEKEYR  : 0x%08lx\n", FLASH->PEKEYR);
  printf2 ("FLASH_PRGKEYR : 0x%08lx\n", FLASH->PRGKEYR);
  printf2 ("FLASH_OPTKEYR : 0x%08lx\n", FLASH->OPTKEYR);    
  printf2 ("FLASH_SR      : 0x%08lx\n", FLASH->SR);
  printf2 ("FLASH_OBR     : 0x%08lx\n", FLASH->OBR);
  printf2 ("FLASH_WRPR1   : 0x%08lx\n", FLASH->WRPR1);
//printf2 ("FLASH_WRPR2   : 0x%08lx\n", FLASH->WRPR2);
}

