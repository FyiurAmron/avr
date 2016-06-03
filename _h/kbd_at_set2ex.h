#ifndef	_KBD_AT_SET2EX_H
#define _KBD_AT_SET2EX_H  1
// AT keyboard scancode set 2 helper methods
#include <avr/pgmspace.h>
#include <stdbool.h>

extern PROGMEM const char const KBD_CODE_MAP[];
extern PROGMEM const char const KBD_CODE_MAP_SHIFT[];

uint8_t kbd_set2_getChar( uint8_t keyCode, bool isShift );

#ifdef COMPILE_SINGLE_FILE
#include "kbd_at_set2ex.c"
#endif

#endif // _KBD_AT_SET2_EX_H