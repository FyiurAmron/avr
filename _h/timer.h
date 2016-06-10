#ifndef	_TIMER_H
#define _TIMER_H  1
// timer tools
#include "macro.h"

#define START_TIMER0  bit8_set( TCCR0B, (BV(CS0)|BV(CS2))
#define STOP_TIMER0   bit8_and( TCCR0B, 0b11111000 )
#define CLEAR_TIMER0  STATEMENT( TCNT0 = 0; )

#endif // _TIMER_H