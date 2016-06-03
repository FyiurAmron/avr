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

extern const uint8_t KEYPAD_NO_KEY_PRESSED;
extern PROGMEM const char * const KEYPAD_KEYCODE1;
extern PROGMEM const char * const KEYPAD_KEYCODE2;
extern PROGMEM const char * const KEYPAD_KEYCODE3;
extern PROGMEM const char * const KEYPAD_KEYCODE4;
extern PROGMEM PGM_P const KEYPAD_KEYCODES[];

uint8_t keypad_get(void);
uint8_t keypad_getChar( uint8_t keyCode, uint8_t keycodeSet );

#ifdef COMPILE_SINGLE_FILE
#include "keypad.c"
#endif

#endif // _KEYPAD_4X4_H