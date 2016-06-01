#ifndef	_TIMER_H
#define _TIMER_H  1
// timer tools

#define START_TIMER0 TCCR0B |= (1<<CS0)|(1<<CS2)
#define STOP_TIMER0  TCCR0B &= 0B11111000
#define CLEAR_TIMER0 TCNT0 = 0

#endif // _TIMER_H