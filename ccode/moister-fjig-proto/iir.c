// file    : iir.c
// author  : rb
// purpose : IIR Biquad filters - w. Iowa Hills' IIRFilters.exe
// date    : 210730
// last    : 220329
//
// note    : sample rate = 500Hz (as per Timer 4 interrupt rate) <> todo

#include "includes.h"

// 50Hz notch filter
static struct iir iir_notch50_0 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.348614809372099540,
  .a2 =  0.666979579846835313,
  .b0 =  0.833489789923417712,
  .b1 = -1.348614809372099540,
  .b2 =  0.833489789923417712
};

static struct iir iir_notch50_1 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.670682300657164280,
  .a2 =  0.894542502016088204,
  .b0 =  0.950788972116680453,
  .b1 = -1.538408876042686830,
  .b2 =  0.950788980047548304
};
  
static struct iir iir_notch50_2 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.288191207013592800,
  .a2 =  0.844213261493906852,
  .b0 =  0.897295176835894881,
  .b1 = -1.451854091202940730,
  .b2 =  0.897295169351237876 
};


// 8-order 5Hz LPF
static struct iir iir_lpf5_0 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.940761909202553870,
  .a2 =  0.941770257622680740,
  .b0 =  0.000252087105031689,
  .b1 =  0.000504174210063378,
  .b2 =  0.000252087105031689
};

static struct iir iir_lpf5_1 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.949600248615885880,
  .a2 =  0.951350833184156919,
  .b0 =  0.000437646142067737,
  .b1 =  0.000875292284135475,
  .b2 =  0.000437646142067737
};

static struct iir iir_lpf5_2 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.964959804451816840,
  .a2 =  0.967864968565405714,
  .b0 =  0.000726291028397227,
  .b1 =  0.001452582056794455,
  .b2 =  0.000726291028397227
};

static struct iir iir_lpf5_3 =
{
  .a0 =  1.000000000000000000,
  .a1 = -1.984868674377245010,
  .a2 =  0.988674383488785002,
  .b0 =  0.000951427277885012,
  .b1 =  0.001902854555770023,
  .b2 =  0.000951427277885012
};

// init wave table & filters
void init_iir (void)
{
  // init filters
//iir_init_filters ();
}

// set filter coeff's
void iir_init_filters (void)
{
}

// filter data
float iir_update (uint32_t dat)
{
  double tmp0 = (double)dat;

  double tmp1 = iir_form1 (&iir_notch50_0, tmp0);
  double tmp2 = iir_form1 (&iir_notch50_1, tmp1);
  double tmp3 = iir_form1 (&iir_notch50_2, tmp2);
  double tmp4 = iir_form1 (&iir_lpf5_0,    tmp3);
  double tmp5 = iir_form1 (&iir_lpf5_1,    tmp4);
  double tmp6 = iir_form1 (&iir_lpf5_2,    tmp5);
  double tmp7 = iir_form1 (&iir_lpf5_3,    tmp6);  

  double y = tmp7;

  return ((float)y);
}

// IIR filter - Biquad Form 1
double iir_form1 (struct iir *p, double dat)
{
  double x = (double)dat;

  // calculate nominator (center tap)
  double t = p->b0*x + p->b1*p->x1 + p->b2*p->x2;
  
  // calculate denominator (output)
  double y = p->a0*t - p->a1*p->y1 - p->a2*p->y2;
  
  // update state
  p->x2 = p->x1;
  p->x1 = x;
 
  p->y2 = p->y1;
  p->y1 = y;
 
  return (y);
}

// IIR filter - Biquad Form 2
double iir_form2 (struct iir *p, double dat)
{
  double x = (double)dat;

  // calculate denominator
  double t = p->a0*x - p->a1*p->x1 - p->a2*p->x2;

  // calculate nominator
  double y = p->b0*t + p->b1*p->x1 + p->b2*p->x2;

  // shift state
  p->x2 = p->x1;
  p->x1 = t;

  return (y);    
} 

// dump IIR filter parameters
void iir_dump (struct iir p)
{
  printf2 ("#a0 : %8.4f\n", p.a0);
  printf2 ("#a1 : %8.4f\n", p.a1);
  printf2 ("#a2 : %8.4f\n", p.a2);
  printf2 ("#b0 : %8.4f\n", p.b0);
  printf2 ("#b1 : %8.4f\n", p.b1);
  printf2 ("#b2 : %8.4f\n", p.b2);
  printf2 ("#x1 : %8.4f\n", p.x1);
  printf2 ("#x2 : %8.4f\n", p.x2);
  printf2 ("#y1 : %8.4f\n", p.y1);
  printf2 ("#y2 : %8.4f\n", p.y2);
  printf2 ("#\n");
}




/*
Stukje code voor 3 pole filter. Alle constanten & state samen in een
struct, en een functie om filter stap te doen.

struct iir3
{
    float b3, b2, b1, b0;
    float a3, a2, a1;
    float x1, x2, x3;
    float y1, y2, y3;
};

float iir3( float x, struct iir3 *iir )
{
    float y;

    y  = iir->b0 * x;
    y += iir->b1 * iir->x1;
    y += iir->b2 * iir->x2;
    y += iir->b3 * iir->x3;
    y -= iir->a1 * iir->y1;
    y -= iir->a2 * iir->y2;
    y -= iir->a3 * iir->y3;

    // update state
    iir->x3 = iir->x2;
    iir->x2 = iir->x1;
    iir->x1 = x;

    iir->y3 = iir->y2;
    iir->y2 = iir->y1;
    iir->y1 = y;

    return y;
}

en dan kun je zo initialiseren:

static struct iir3 butterworth_lpf =
{
   .b3 =  1.809893300751442610E-2,
   .b2 =  5.429679902254328280E-2,
   .b1 =  5.429679902254328280E-2,
   .b0 =  1.809893300751442610E-2,
   .a3 = -2.780599176345464940E-1,
   .a2 =  1.182893262037830960E0,
   .a1 = -1.760041880343169000E0
};

De overige state variabelen staan dan automagisch allemaal op 0.0
*/









