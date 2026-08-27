// file    : bat.h
// author  : rb
// purpose : header file for bat.c
// date    : 211015
// last    : 211015
//

#ifndef _BAT_H_
#define _BAT_H_

// -- defines 
#define CHARGE_NONE                               0
#define CHARGE_BAT1                               1
#define CHARGE_BAT2                               2

#define MON_START                                 0
#define MON_CHARGE1                               1
#define MON_CHARGE2                               2

// -- protoypes
void init_bat (void);

void vbat_monitor (void);

void use_bat1 (void);
void use_bat2 (void);

#endif
