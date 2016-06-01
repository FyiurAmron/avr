#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "../_h/misc.h"
#include "../_h/bits.h"

#define TEST      C
#define TEST_BIT  7

void blink( int time ) {
	bit_toggle( &xPORT(TEST), TEST_BIT );
    //TEST_PORT ^= (1<< TEST_BIT);
    _delay_ms(time);

    bit_toggle( &xPORT(TEST), TEST_BIT );
	//TEST_PORT ^= (1<< TEST_BIT);
    _delay_ms(time);
}

void blinkPort( int time ) {
	xPORT(TEST) = 0x00;
    _delay_ms(time);

	xPORT(TEST) = 0xFF;
    _delay_ms(time);
}

int main( void ) {
    init();

    //TEST_DDR |= (1<< TEST_BIT);
	bit_set( &xDDR(TEST), TEST_BIT );
    //xDDR(TEST) = 0xFF;
    
    while(1) {
	    for( int i = 100; i < 1000; i += 100 ) {
	        blink( i );
        }
    }
}
