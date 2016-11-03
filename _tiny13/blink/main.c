#define F_CPU  9600000UL * 4

#include <avr/io.h>
#include <util/delay.h>

#include "vax/misc.h"
#include "vax/bits.h"

#define TEST      B
#define TEST_BIT  1

void delay_ms( uint8_t ms ) {
    for (; ms > 0; ms--) {
       _delay_ms(1);
    }
}

void blink( int time ) {
	bit_toggle( &xPORT(TEST), TEST_BIT );
    //TEST_PORT ^= (1<< TEST_BIT);
    delay_ms(time);

    bit_toggle( &xPORT(TEST), TEST_BIT );
	//TEST_PORT ^= (1<< TEST_BIT);
    delay_ms(time);
}

void blinkPort( int time ) {
	xPORT(TEST) = 0x00;
    delay_ms(time);

	xPORT(TEST) = 0xFF;
    delay_ms(time);
}

int main( void ) {
    //init();

    //TEST_DDR |= (1<< TEST_BIT);
	bit_set( &xDDR(TEST), TEST_BIT );
    //xDDR(TEST) = 0xFF;
    //DDRA = 0xFF;
    //DDRC = 0xFF;
    //PORTA = 0xFF;
    //PORTC = 0xFF;
    
    while(1) {
	    //for( int i = 100; i < 1000; i += 100 ) {
	    //    blink( i );
        //}
        blink(1000);
    }
}
