#ifndef _MACRO_H
#define _MACRO_H  1
#include <stdint.h>
#include <stdbool.h>

#define NOTHING
#define STATEMENT(...)  do { __VA_ARGS__ } while(0)
#define EMPTY_STATEMENT  STATEMENT(NOTHING)

#define QUOTE0(...)     #__VA_ARGS__
#define QUOTE(...)      QUOTE0(__VA_ARGS__)
#define CONCAT0(x,y)  x ## y
#define CONCAT(x,y)   CONCAT0(x,y)
//#define EXPAND(x)     x

#define xPORT(x)      CONCAT(PORT,x)
#define xDDR(x)       CONCAT(DDR,x)
#define xPIN(x)       CONCAT(PIN,x)

#define ESC  "\x1B"

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

#define bv8(x)                      ( (uint8_t)( 1<< (x) ) )

#define bit8_and(x,y)               ( (uint8_t)( (uint8_t)(x) & (uint8_t)(y) ) )
#define bit8_or(x,y)                ( (uint8_t)( (uint8_t)(x) | (uint8_t)(y) ) )
#define bit8_xor(x,y)               ( (uint8_t)( (uint8_t)(x) ^ (uint8_t)(y) ) )
#define bit8_not(x)                 ( (uint8_t)~(x) )

#define bit8_set(x,y)               STATEMENT( (x) |= (uint8_t)(y); )
#define bit8_clear(x,y)             STATEMENT( (x) &= bit8_not(y); )
#define bit8_toggle(x,y)            STATEMENT( (x) ^= (uint8_t)(y); )
// note that both |= & &= on uint8_t LHS doesn't require casts by themselves!
#define bit8_if(c,x,y)              STATEMENT( if ( c ) { bit8_set(x,y); } else { bit8_clear(x,y); } )
#define bit8_lsb(x)                 ( (uint8_t)( x & 1 ) )
#define BV                          bv8

#define _delay_ticks(x)  for( volatile uint8_t _delayer = (x); _delayer > 0; _delayer-- )

#define ARRAY_LENGTH(x)  ( sizeof(x) / sizeof(x[0]) )

#ifdef DEBUG
#define debug_printf(...)   printf( __VA_ARGS__ )
#define debug_print_line()  printf( "dbg@%d ", __LINE__ );
#else
#define debug_printf(...)   EMPTY_STATEMENT
#define debug_print_line()  EMPTY_STATEMENT
//#define debug_printf(...)  NOTHING
#endif

#endif // _MACRO_H