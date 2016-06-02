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

#ifdef SINGLE_FILE
#include "keypad.c"
#endif

#endif // _KEYPAD_4X4_H