#ifndef	_KBD_AT_SET2_H
#define _KBD_AT_SET2_H  1
// AT keyboard scancode set 2
#include <avr/pgmspace.h>
#include <stdbool.h>

PROGMEM const char const KBD_CODE_MAP[] = ""
"ÿÿÿÿÿÿÿÿÿÿÿÿÿ\t`ÿ"
"ÿÿÿÿÿq1ÿÿÿzsaw2ÿ"
"ÿcxde43ÿÿ vftr5ÿ"
"ÿnbhgy6ÿÿÿmju78ÿ"
"ÿ,kio09ÿÿ./l;p-ÿ"
"ÿÿ'ÿ[=ÿÿÿÿ\n]ÿ\\ÿÿ"
"ÿÿÿÿÿÿ\bÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"";

PROGMEM const char const KBD_CODE_MAP_SHIFT[] = ""
"ÿÿÿÿÿÿÿÿÿÿÿÿÿ\t~ÿ"
"ÿÿÿÿÿQ!ÿÿÿZSAW@ÿ"
"ÿCXDE$#ÿÿ VFTR%ÿ"
"ÿNBHGY^ÿÿÿMJU&*ÿ"
"ÿ<KIO)(ÿÿ>?L:P_ÿ"
"ÿÿ\"ÿ{+ÿÿÿÿ\n}ÿ|ÿÿ"
"ÿÿÿÿÿÿ\bÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ"
"";

uint8_t kbd_set2_getChar( uint8_t keyCode, bool isShift );

inline uint8_t kbd_set2_getChar( uint8_t keyCode, bool isShift ) {
    return pgm_read_byte( ( isShift ? KBD_CODE_MAP_SHIFT : KBD_CODE_MAP ) + keyCode );
}

#endif // _KBD_AT_SET2_H