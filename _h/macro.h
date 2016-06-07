#ifndef _MACRO_H
#define _MACRO_H  1
#include <stdint.h>
#include <stdbool.h>

#define NOTHING
#define STATEMENT(x)  do { x } while(0)
#define EMPTY_STATEMENT  STATEMENT(NOTHING)

#define QUOTE0(x)     #x
#define QUOTE(x)      QUOTE0(x)
#define CONCAT0(x,y)  x ## y
#define CONCAT(x,y)   CONCAT0(x,y)
//#define EXPAND(x)     x

#define xPORT(x)  CONCAT(PORT,x)
#define xDDR(x)   CONCAT(DDR,x)
#define xPIN(x)   CONCAT(PIN,x)

//#define _NOP()  do { __asm__ __volatile__ ("nop"); } while (0)
#define _NOP()  STATEMENT( __asm__ __volatile__ ("nop"); )
#define _delay_ticks(x)  for( volatile uint8_t _delayer = x; _delayer > 0; _delayer-- )

#define BYTE2BIN_FORMAT "%c%c%c%c%c%c%c%c"
#define BYTE2BIN_ARG(byte)   \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define ESC  "\x1B"

#ifdef DEBUG
#define debug_printf(...)   printf( __VA_ARGS__ )
#define debug_print_line()  printf( "dbg@%d ", __LINE__ );
#else
#define debug_printf(...)   EMPTY_STATEMENT
#define debug_print_line()  EMPTY_STATEMENT
//#define debug_printf(...)  NOTHING
#endif

#endif // _MACRO_H