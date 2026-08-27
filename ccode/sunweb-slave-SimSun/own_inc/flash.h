// file    : flash.h
// author  : rb
// purpose : header file for flash.c
// date    : 180820
// last    : 190108
//

#ifndef _FLASH_H_
#define _FLASH_H_

#define USERFLASH_BASE    0x08007c00             // base address page 31, used for user data (defined in .ld script)

// -- prototypes
void init_flash (void);

void     flash_id_write (uint16_t id);
uint16_t flash_id_read  (void);

void flash_write (int32_t *ptr, int32_t dat);

void flash_erase_page (uint32_t adr);

void flash_unlock    (void);
void flash_ob_unlock (void);
void flash_ob_write  (uint8_t offset, uint16_t dat);
void flash_ob_erase  (void);

void flash_dump_optionbytes (void);
uint32_t flash_get_uidseed (void);

void flash_dump_registers   (void);

void flash_write_ntcerr (int32_t *buf, uint8_t len);
void flash_read_ntcerr  (int32_t *buf, uint8_t len);
void flash_dump_ntcerr  (void);

#endif