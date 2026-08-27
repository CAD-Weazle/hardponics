// file    : simsun.h
// author  : rb
// purpose : header file for simsun.c
// date    : 180823
// last    : 190106
//

#ifndef _SIMSUN_H_
#define _SIMSUN_H_

// -- globals
extern uint8_t do_simsun;              // run light program

// -- prototypes
void init_simsun (void);

void simsun_start (void);
void simsun_stop  (void);

void simsun_update (void);

void simsun_set_dawn (uint8_t hrs, uint8_t min, uint8_t sec);
void simsun_set_dusk (uint8_t hrs, uint8_t min, uint8_t sec);

void simsun_dred_max (uint8_t val);
void simsun_rblu_max (uint8_t val);

uint8_t simsun_get_dred (void);
uint8_t simsun_get_rblu (void);
uint8_t simsun_get_fred (void);

void simsun_dump_state (void);

#endif


