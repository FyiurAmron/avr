#ifndef	_KEYPAD_H
#define _KEYPAD_H  1
// keypad read code

#include "misc.h"

#ifndef KEYPAD_IO_DELAY_TICKS
#  define KEYPAD_IO_DELAY_TICKS  1
#endif

#ifdef KEYPAD
#  define KEYPAD_LOW   KEYPAD
#  define KEYPAD_HIGH  KEYPAD
#  if !defined(KEYPAD_LOW_START) && !defined(KEYPAD_LOW_END) && !defined(KEYPAD_HIGH_START) && !defined(KEYPAD_HIGH_END)
#    define KEYPAD_DEFAULT_BITMASK  0xF0
#  endif
#endif

// e.g. column - high bits; row - low bits

#ifndef KEYPAD_LOW_START
#  define KEYPAD_LOW_START  0
#endif

#ifndef KEYPAD_LOW_END
#  define KEYPAD_LOW_END  3
#endif

#ifndef KEYPAD_HIGH_START
#  define KEYPAD_HIGH_START  4
#endif

#ifndef KEYPAD_HIGH_END
#  define KEYPAD_HIGH_END  7
#endif

const uint8_t KEYPAD_NO_KEY_PRESSED = 0xFF;

const uint8_t * const KEYPAD_KEYCODES1 = (uint8_t * const) "123A456B789C*0#D";
const uint8_t * const KEYPAD_KEYCODES2 = (uint8_t * const) "147*2580369#ABCD";
const uint8_t * const KEYPAD_KEYCODES3 = (uint8_t * const) "D#0*C987B654A321";
const uint8_t * const KEYPAD_KEYCODES4 = (uint8_t * const) "DCBA#9630852*741";

uint8_t getKeyPressed(void);
uint8_t getKeyPressedInv(void);

uint8_t getKeyPressed(void) {
#ifdef KEYPAD_DEFAULT_BITMASK
    xPORT(KEYPAD) = KEYPAD_DEFAULT_BITMASK;
#else
#  ifdef KEYPAD_LOW_BITMASK
    xPORT(KEYPAD_LOW) = KEYPAD_LOW_BITMASK;
    xPORT(KEYPAD_HIGH) = KEYPAD_HIGH_BITMASK;
#  else
    for( int8_t c = KEYPAD_LOW_END; c >= KEYPAD_LOW_START; c-- ) {
        xPORT(KEYPAD_LOW)  &=~(1 << c);
    }
    for( int8_t c = KEYPAD_HIGH_END; c >= KEYPAD_HIGH_START; c-- ) {
        xPORT(KEYPAD_HIGH) |= (1 << c);
    }
#  endif
#endif // KEYPAD_DEFAULT_BITMASK
    for( int8_t c = KEYPAD_LOW_END; c >= KEYPAD_LOW_START; c-- ) {
        xDDR(KEYPAD_LOW) = (1 << c);
        _delay_ticks(KEYPAD_IO_DELAY_TICKS); // needed to sync I with O
        for( int8_t r = KEYPAD_HIGH_END; r >= KEYPAD_HIGH_START; r-- ) {
            if( ~xPIN(KEYPAD_HIGH) & (1 << r) ) {
                return 4 * ( r - KEYPAD_HIGH_START ) + c;
            }
        }
    }
    return KEYPAD_NO_KEY_PRESSED;
}

#endif // _KEYPAD_4X4_H