#ifndef	_KBD_AT_SET2_H
#define _KBD_AT_SET2_H  1
// AT keyboard scancode set 2
#include <avr/pgmspace.h>
#include <stdbool.h>

PROGMEM const char const KBD_CODE_MAP[] = ""
"�������������\t`�"
"�����q1���zsaw2�"
"�cxde43�� vftr5�"
"�nbhgy6���mju78�"
"�,kio09��./l;p-�"
"��'�[=����\n]�\\��"
"������\b���������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"";

PROGMEM const char const KBD_CODE_MAP_SHIFT[] = ""
"�������������\t~�"
"�����Q!���ZSAW@�"
"�CXDE$#�� VFTR%�"
"�NBHGY^���MJU&*�"
"�<KIO)(��>?L:P_�"
"��\"�{+����\n}�|��"
"������\b���������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"����������������"
"";

uint8_t kbd_set2_getChar( uint8_t keyCode, bool isShift );

inline uint8_t kbd_set2_getChar( uint8_t keyCode, bool isShift ) {
    return pgm_read_byte( ( isShift ? KBD_CODE_MAP_SHIFT : KBD_CODE_MAP ) + keyCode );
}

#endif // _KBD_AT_SET2_H