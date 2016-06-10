#define F_CPU  1000000UL
//#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>

#include "vax/bits.h"
#include "vax/misc.h"

#define KEYPAD C /*A*/
#include "vax/keypad.h"

#include "vax/uart.h"

void blink_init( volatile uint8_t* where, uint8_t what ) {
    bit_set( where, what );
}

void blink( volatile uint8_t* where, uint8_t what, int time ) {
	bit_toggle( where, what );
    _delay_ms(time);
    bit_toggle( where, what );
    _delay_ms(time);
}

int main( void ) {
    init();

    power_adc_disable();
    //etc.
    //ADCSRA = 0;
    xDDR(B) |= (1<< 0 ); // keypad status
    PORTB = 0;

    //while(1) {} // to quickly disable uC code

    uart_init();
    uart_as_stdio();

    puts("\n\rDevice started...\r");

    uint8_t keyNum = NO_KEY_PRESSED, keyNumLast = 0xFE;

    for(;;) {
        keyNum = getKeyPressed();
        //printf( BYTE2BIN_FORMAT"\n\r...\n\r", BYTE2BIN_ARG(xPIN(KEYPAD)) );
        if ( keyNum != NO_KEY_PRESSED ) {
            xPORT(B) |= (1<< 0 ); // keypad status
            if ( keyNumLast != keyNum ) {
                printf( "%c", KEYPAD_KEYCODES3[keyNum] );
                keyNumLast = keyNum;
            }
        } else {
            keyNumLast = keyNum;
            xPORT(B) &=~(1<< 0 ); // keypad status
        }
    } // while(1)
} // main()
