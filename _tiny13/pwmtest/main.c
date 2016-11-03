#define F_CPU  9600000UL
// the value for Fast PWM; 37500 Hz actually
#define PWM_FREQ  F_CPU / 256

#include <avr/io.h>
#include <util/delay.h>

#include <vax/misc.h>
#include <vax/bits.h>

#define TEST      B
#define TEST_BIT  1
// MISO pin BTW

int main( void ) {
	bit8_set( xDDR(TEST), BV(TEST_BIT) );

    bit8_set( TCCR0A, BV(WGM01) | BV(WGM00) ); // Fast PWM, full 8-bit
    bit8_set( TCCR0A, BV(COM0B1) ); // enable OCR0B Fast PWM: Clear on Compare Match, set at TOP
	bit8_set( TCCR0B, BV(CS00) ); // full speed
#define PWM_MAX  100
    uint8_t val = PWM_MAX, change = 1;
    while(1) {
        val -= change;
        if ( val == 0 || val == PWM_MAX ) {
            change = -change;
        }
        OCR0B = val;
        _delay_ms(10);
    }
}
