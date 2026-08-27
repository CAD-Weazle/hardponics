// file    : printf.h
// author  : ao/rb
// purpose : header file for printf2.c
// date    : 000101
// last    : 210625
//

#ifndef _PRINTF2_H_
#define _PRINTF2_H_

int printf2  (const char *fmt, ...);
int sprintf2 (char *buf, const char *fmt, ...);

int debug2  (uint8_t do_print, const char *fmt, ...);

int putchar (int c);
int puts    (const char *s);
int getchar (void);

#endif




