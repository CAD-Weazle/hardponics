// file    : caps.h
// author  : rb
// purpose : header file for caps.c
// date    : 220304
// last    : 220304
//

#ifndef _CAPS_H_
#define _CAPS_H_

// -- defines 

// -- protoypes
void init_tcs (void);

void tcs_next (void);

void tsc_cs_discharge (void);
void tsc_cs_float (void);
void tsc_ct_discharge (void);
void tsc_ct_float (void);
void tsc_ct_charge (void);

#endif
