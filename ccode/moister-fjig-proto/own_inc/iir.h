// file    : iir.h
// author  : rb
// purpose : header file for iir.c
// date    : 210730
// last    : 220330
//

#ifndef _IIR_H_
#define _IIR_H_

extern int filter_on;
extern uint16_t max;

struct iir_pars
{
  double a0, a1, a2;			  
  double b0, b1, b2;
  double x1, x2;
  double y1, y2;
};

// filter parameters & state
struct iir
{
  float b3, b2, b1, b0;
  float a3, a2, a1, a0;
  float x1, x2, x3;
  float y1, y2, y3;
};

// -- defines
//#define F_SAMPLE                           1000.0  // sample frequency
//#define F_FILTER                             50.0  // filter notch at 50Hz mains
											  
#define PI                            3.141592654  // good old pi, you know...

// -- prototypes
void init_iir (void);

void iir_init_filters (void);

float iir_update (uint32_t dat);

double iir_form1 (struct iir *p, double dat);
double iir_form2 (struct iir *p, double dat);

void iir_dump (struct iir p);

#endif



