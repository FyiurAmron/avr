#ifndef _ASM_H
#define _ASM_H  1

#include "macro.h"

#ifndef __GNUC__
#define __asm__  asm
#endif

#define ASM_CMD(x)                STATEMENT( __asm__( x ); )

//#define ASM_NOP()           do { __asm__ __volatile__ ("nop"); } while (0)
#define ASM_NOP()            STATEMENT( __asm__ __volatile__ ( "nop" ); )
#define ASM_ADD(r,a,b)       ASM_CMD( "add %0, %1" : "=r" (r) : "r" (a), "r" (b) )
#define ASM_ADC(r,a,b)       ASM_CMD( "adc %0, %1" : "=r" (r) : "r" (a), "r" (b) )

#endif // _ASM_H