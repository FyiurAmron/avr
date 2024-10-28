#ifndef	_BITS_H
#define _BITS_H  1
// bit tools

// FIXME
#define b(byteno, n)  ( (uint8_t)( n >> ( 8 * byteno ) ) )

#define sbi(port, bit)  (port) |= _BV(bit)
#define cbi(port, bit)  (port) &= ~_BV(bit)
#define tbi(port, bit)  (port) ^= _BV(bit)

// bit_is_set
// bit_is_clear

#define bit_set_to(port, bit, value) \
do { \
    if ( value ) { \
	    sbi( port, bit ); \
    } else { \
        cbi( port, bit ); \
    } \
} while ( 0 )

#define bi  bit_set_to

#define bv8(x)                      ( (uint8_t)( 1 << (x) ) )

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

#endif // bits.h