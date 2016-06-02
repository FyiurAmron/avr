//
#include "keypad.h"

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
