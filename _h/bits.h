#ifndef	_BITS_H
#define _BITS_H  1
// bit tools

//#include <stdint.h>
#include <stdbool.h>

void bit_setTo( volatile uint8_t* where, uint8_t what, bool value );
void bit_set( volatile uint8_t* where, uint8_t what );
void bit_clear( volatile uint8_t* where, uint8_t what );
void bit_toggle( volatile uint8_t* where, uint8_t what );
bool bit_is( volatile uint8_t* where, uint8_t what );

inline void bit_setTo( volatile uint8_t* where, uint8_t what, bool value ) {
    if ( value ) {
	    bit_set( where, what );
    } else {
        bit_clear( where, what );
    }
}

inline void bit_set( volatile uint8_t* where, uint8_t what ) {
	*where |= (uint8_t)(1<< what);
}

inline void bit_clear( volatile uint8_t* where, uint8_t what ) {
	*where &= (uint8_t)(~(uint8_t)(1<< what));
}

inline void bit_toggle( volatile uint8_t* where, uint8_t what ) {
	*where ^= (uint8_t)(1<< what);
}

inline bool bit_is( volatile uint8_t* where, uint8_t what ) {
	return *where & (uint8_t)(1<< what);
}

#ifdef BITS_SHORT_NAMES
#define set_to bit_set_to
#define set    bit_set
#define clear  bit_clear
#define toggle bit_toggle
#define is     bit_is
#endif // BITS_SHORT_NAMES

#endif // bits.h