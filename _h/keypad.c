//
#include "keypad.h"

PROGMEM const char * const KEYPAD_KEYCODES1 = "123A456B789C*0#D";
PROGMEM const char * const KEYPAD_KEYCODES2 = "147*2580369#ABCD";
PROGMEM const char * const KEYPAD_KEYCODES3 = "D#0*C987B654A321";
PROGMEM const char * const KEYPAD_KEYCODES4 = "DCBA#9630852*741";

PROGMEM const PGM_P const KEYPAD_KEYCODES[] = {
    KEYPAD_KEYCODES1,
    KEYPAD_KEYCODES2,
    KEYPAD_KEYCODES3,
    KEYPAD_KEYCODES4,
}

inline uint8_t keypad_getChar( uint8_t keyCode, uint8_t keycodeSet ) {
    return pgm_read_byte( KEYPAD_KEYCODES[keycodeSet] + keyCode );
}

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
