#ifndef _ASM_H
#define _ASM_H  1

#include "macro.h"

#ifndef __GNUC__
#define __asm__  asm
#endif

#define ASM_CMD(...)                STATEMENT( __asm__  __volatile__ ( __VA_ARGS__ ); )

#define ASM_NOP()             ASM_CMD( "nop" )
#define ASM_ADD(target,val)   ASM_CMD( "add %0,%2" : "=r" (target) : "0" (target), "r" (val) )
#define ASM_ADC(target,val)   ASM_CMD( "adc %0,%2" : "=r" (target) : "0" (target), "r" (val) )
#define ASM_SUBI(target,imm)  ASM_CMD( "subi %0,%2" : "=r" (target) : "0" (target), "M" (imm) )
#define ASM_ST_INC(ptr,val)   ASM_CMD( "st %a0+,%2": "=e" (ptr) : "0" (ptr), "r" (val) )

#endif // _ASM_H