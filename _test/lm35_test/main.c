#define F_CPU  20000000UL
//#define F_CPU 1000000UL
#define COMPILE_SINGLE_FILE
#include "vax/misc.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define BAUD  9600
#include "vax/uart.h"

int main( void ) {
    init();

    uart_init();
    uart_stdio();
    printf( "\n\rUART device started: %ld %ld %d\n\r", UBRRH_VALUE, UBRRL_VALUE, USE_2X );

  // setup ADC
  ADMUX  = 0b11100000; // 2.56V, ADLAR, ADC0
/*
  ADCSRA = 0b11000000; // enable ADC & start first conversion
*/
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 128 - 125KHz sample rate @ 16MHz
   ADCSRA |= (1 << ADEN);  // Enable ADC
   ADCSRA |= (1 << ADSC);  // Start A2D Conversions

    while( ADCSRA & _BV( ADSC ) )
        ; // first conversion to set up the ADC, since it's 2x clocks for the first time
    for(;;) {
    ADCSRA |= _BV( ADSC );
    while( ADCSRA & _BV( ADSC ) )
      ;
        printf( "%d\n\r", ADCH );
        //uart_putchar( 'q' );
        //_delay_ms(1000);
    }
} // main()
