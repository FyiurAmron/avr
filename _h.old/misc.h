#ifndef	_MISC_H
#define _MISC_H  1
// misc utils
#include <avr/wdt.h>
#include "macro.h"

#define soft_reset()  do { wdt_enable(WDTO_15MS); while(1) {} } while(0)

#define init()  MCUSR = 0; wdt_disable(); \
                MCUCR = _BV( JTD ); MCUCR = _BV( JTD ); \
                DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; \
                PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0
// disable JTAG (e.g. to enable PC2/3/4/5 pins); MCUCR = _BV( JTD ) has to be done twice to work, per doc

#endif // _MISC_H