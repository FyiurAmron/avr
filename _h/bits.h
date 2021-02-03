#ifndef	_BITS_H
#define _BITS_H  1
// bit tools

#include <stdbool.h>

#define sbi(port, bit)  (port) |= _BV(bit)
#define cbi(port, bit)  (port) &= ~_BV(bit)
#define tbi(port, bit)  (port) ^= _BV(bit)

// bit_is_set
// bit_is_clear

#define bit_set_to(port, bit, value)  { \
    if ( value ) { \
	    sbi( where, what ); \
    } else { \
        cbi( where, what ); \
    } \
}

#endif // bits.h