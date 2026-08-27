// file    : flash.c
// author  : rb
// purpose : FLASH memory routines - STM32F042xx
// date    : 180820
// last    : 181221
//
// note    : default Option Byte values:
//           0x1ffff800: 0x00ff55aa  // RDP
//           0x1ffff804: 0x00ff00ff  // user bytes, only use these
//           0x1ffff808: 0x00ff00ff  // WRP0 & WRP1
//           0x1ffff80c: 0x00ff00ff  // WRP2 & WRP3
//

#define FLASH_DEBUG     0

#include "includes.h"

// temperature parameters stored in user FLASH (default 0.0, extra last entry magic number)
int32_t rtd_parms[PT_NUM+1] __attribute__((section (".userflash"))) __attribute__ ((aligned (4))) = {0, 0, 0, 1234};

// init FLASH
void init_flash (void)
{
  // erase FLASH page when magic is zero-filled (after burning firmware)
  if (rtd_parms[PT_NUM] != 1234)
  {
    // magic number not correct, erase FLASH page 
    flash_erase_page (USERFLASH_BASE);   
    debug2 (FLASH_DEBUG, "#Pt100 offset parameters magic missing, erase FLASH page\n");
  }
  else
    debug2 (FLASH_DEBUG, "#Pt100 offset parameters not empty nor erased, ok\n");
}

// write Pt100 offset errors to FLASH
void flash_write_rtderr (int32_t *buf, uint8_t len)
{
  // erase FLASH page first
  flash_erase_page (USERFLASH_BASE);   

  // write Pt100 error parameters to FLASH
  for (int i = 0; i < len; i++)
    flash_write (&rtd_parms[i], buf[i]);

  // write magic in last location
  flash_write (&rtd_parms[PT_NUM], 1234);
}

// read Pt100 offset errors from FLASH
void flash_read_rtderr (int32_t *buf, uint8_t len)
{
  for (int i = 0; i < len; i++)
    buf[i] = rtd_parms[i];
}

// dump Pt100 offset errors <> dev only
void flash_dump_rtderr (void)
{
  int32_t sign;

  for (int i = 0; i < PT_NUM; i++)
  {
    sign = (rtd_parms[i] < 0) ? -1 : 1;
    printf2 ("E%d: %d.%02d\n", i, rtd_parms[i]/100, sign*rtd_parms[i]%100);
  }
}

// write to FLASH, 32-bit writes
void flash_write (int32_t *ptr, int32_t dat)
{
  uint16_t *wr_ptr = (uint16_t *)ptr;

  // unlock FLASH for write action
  flash_unlock ();

  // enter programming mode
  FLASH->CR |= FLASH_CR_PG;

  // perform 16-bit writes
  *wr_ptr     = (dat & 0x0000ffff);
  *(wr_ptr+1) = (dat & 0xffff0000) >> 16;

  // wait while FLASH busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;

  // check End of Operation flag
  if (FLASH->SR & FLASH_SR_EOP)
  {
    FLASH->SR = FLASH_SR_EOP; // reset by writing '1'
  }
  else  
  {
    // catch errors...
  }

  // leave programming mode
  FLASH->CR &= ~FLASH_CR_PG;
}

// erase FLASH page
void flash_erase_page (uint32_t adr)
{
  // unlock FLASH for write action
  flash_unlock ();

  // enter page erasing mode
  FLASH->CR |= FLASH_CR_PER;

  // write FLASH_AR with "address within page to erase"
  FLASH->AR = adr;

  // start erasing
  FLASH->CR |= FLASH_CR_STRT;

  // wait while busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;

  // check End of Operation flag
  if (FLASH->SR & FLASH_SR_EOP)
  {  
    FLASH->SR = FLASH_SR_EOP; // reset by writing '1'
  }    
  else  
  {
    // catch errors...
  }

  // leave page erasing mode
  FLASH->CR &= ~FLASH_CR_PER;
}

// write Serial Bus ID to FLASH Option Bytes
void flash_id_write (uint16_t id)
{
  // unlock FLASH Option Bytes
  flash_ob_unlock ();

  // erase Option Bytes
  flash_ob_erase ();

  // write ID & rewrite deafult read/write protection bits
  flash_ob_write (0, 0xffaa);  // set RDP to level 0 (default, no read protection)
  flash_ob_write (1, id);      // write user data
  flash_ob_write (2, 0xffff);  // set WRP0 & WRP1
  flash_ob_write (3, 0xffff);  // set WRP2 & WRP3
}

// read Serial Bus ID from FLASH Option Bytes
uint16_t flash_id_read (void)
{
  // read user data from Option Bytes (memory location 1, offset = 4)
  uint32_t dat = *(uint32_t *)(OB_BASE+4);

  return ((uint16_t) (((dat&0x00ff0000) >> 8) | (dat&0x0000ff)));
}

// unlock FLASH for write action
void flash_unlock (void)
{
  // wait while busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;  

  // unlock FLASH when locked 
  if (FLASH->CR & FLASH_CR_LOCK)
  {    
    // write unlock magics
    FLASH->KEYR = FLASH_KEY1;          // magic 0x45670123
    FLASH->KEYR = FLASH_KEY2;          // magic 0xcdef89ab
  }
}

// unlock Option Bytes FLASH for write action
void flash_ob_unlock (void)
{
  // wait while busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;

  // unlock FLASH when locked 
  if (FLASH->CR & FLASH_CR_LOCK)
  {    
    // write unlock magics
    FLASH->KEYR = FLASH_KEY1;          // magic 0x45670123
    FLASH->KEYR = FLASH_KEY2;          // magic 0xcdef89ab
  }

  // unlock Option Byte FLASH when locked 
  if ((FLASH->CR & FLASH_CR_OPTWRE) == 0)
  {
    // write Option Bytes unlock magics
    FLASH->OPTKEYR = FLASH_OPTKEY1;    // magic 0x45670123
    FLASH->OPTKEYR = FLASH_OPTKEY2;    // magic 0xcdef89ab
  }
}

// write FLASH Option Bytes
void flash_ob_write (uint8_t offset, uint16_t dat)
{
  // enable programming (set PG bit)
  FLASH->CR |= FLASH_CR_OPTPG;

  // write data byte-wise as 16-bit words with 32-bit offset (WTF?)
  *(uint16_t *)(OB_BASE + offset*4 + 0) = (uint8_t)(dat & 0x00ff);
  *(uint16_t *)(OB_BASE + offset*4 + 2) = (uint8_t)((dat & 0xff00) >> 8);

  // wait while busy
  while (FLASH->SR & FLASH_SR_BSY)
    ;

  // wait for FLASH operation completed
  if ((FLASH->SR & FLASH_SR_EOP) != 0)
  {
    // clear EOP flag by writing 1
    FLASH->SR = FLASH_SR_EOP;
  }
  else
  {
    // manage error cases
  }

  // disable programming (clear PG bit)
  FLASH->CR &= ~FLASH_CR_OPTPG;
}

// erase FLASH Option Bytes
void flash_ob_erase (void)
{
  // set the OPTER bit in the FLASH_CR register to enable option byte erasing
  FLASH->CR |= FLASH_CR_OPTER; 

  // set the STRT bit in the FLASH_CR register to start the erasing
  FLASH->CR |= FLASH_CR_STRT;

  // wait until the BSY bit is reset in the FLASH_SR register
  while ((FLASH->SR & FLASH_SR_BSY) != 0)
    ;

  // check the EOP flag in the FLASH_SR register
  if ((FLASH->SR & FLASH_SR_EOP) != 0)
  {
    // clear EOP flag by software by writing EOP at 1
    FLASH->SR = FLASH_SR_EOP; /* (5)*/
  }    
  else
  {
    // manage the error cases
  }

  // reset the PER Bit to disable the page erase
  FLASH->CR &= ~FLASH_CR_OPTER;
}

// dump FLASH Option Byte block 
void flash_dump_optionbytes (void)
{
  uint32_t *ob_ptr = (uint32_t *)(OB_BASE);

  // 4 word block
  for (int i = 0; i < 4; i++)
  {
    printf2 ("Option Byte location %d: 0x%08x - 0x%08x \n", i, &ob_ptr[i], ob_ptr[i]);
  }

  printf2 ("done\n");
}

// read UID & generate seed for random back-off
uint32_t flash_get_uidseed (void)
{
  // get UID & calculate seed
  uint32_t uid[3];

  uid[0] = *(uint32_t *)(UID_BASE);
  uid[1] = *(uint32_t *)(UID_BASE + 4);
  uid[2] = *(uint32_t *)(UID_BASE + 8);
/*
  printf2 ("UID0: 0x%08x\n", uid[0]);
  printf2 ("UID1: 0x%08x\n", uid[1]);
  printf2 ("UID2: 0x%08x\n", uid[2]);
*/
  return (uid[0] ^ uid[1] ^ uid[2]);
}

// dump FLASH registers
void flash_dump_registers (void)
{
  printf2 ("FLASH_ACR    : 0x%08x\n", FLASH->ACR);
//printf2 ("FLASH_KEYR   : 0x%08x\n", FLASH->KEYR);
//printf2 ("FLASH_OPTKEYR: 0x%08x\n", FLASH->OPTKEYR);
  printf2 ("FLASH_SR     : 0x%08x\n", FLASH->SR);
  printf2 ("FLASH_CR     : 0x%08x\n", FLASH->CR);
  printf2 ("FLASH_AR     : 0x%08x\n", FLASH->AR);
  printf2 ("FLASH_OBR    : 0x%08x\n", FLASH->OBR);
  printf2 ("FLASH_WRPR   : 0x%08x\n", FLASH->WRPR);
}
