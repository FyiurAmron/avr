#define F_CPU  8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define COMPILE_SINGLE_FILE

#define BAUD  2400
#include "vax/macro.h"
#include "vax/uart.h"

char buf[256];

//uint8_t EEMEM vThreshold = 21;
// use EEPROM to store current RGB settings

/*
ISR( TIMER1_OVF_vect ) {
}
*/

#define BASE_DELAY  500

void setBasicColor( uint8_t red, uint8_t green, uint8_t blue ) {
    bi( PORTD, 6, red );
    bi( PORTB, 1, green );
    bi( PORTD, 3, blue );
}

void setBasicColorArray( uint8_t rgb[3] ) {
    setBasicColor( rgb[0], rgb[1], rgb[2] );
}

uint8_t TEST_SEQUENCE[][3] = {
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 },
  { 0, 0, 0 },
  { 0, 0, 1 },
  { 0, 1, 0 },
  { 1, 0, 0 },
  { 0, 0, 0 },
  { 1, 1, 0 },
  { 0, 1, 1 },
  { 1, 0, 1 },
  { 0, 0, 0 },
  { 1, 0, 1 },
  { 0, 1, 1 },
  { 1, 1, 0 },
  { 0, 0, 0 },
  { 1, 1, 1 },
  { 0, 0, 0 },
  { 1, 1, 1 },
  { 0, 0, 0 },
  { 1, 1, 1 },
  { 0, 0, 0 },
};

void startupTest( void ) {
    for( uint8_t i = 0; i < ARRAY_LENGTH( TEST_SEQUENCE ); i++ ) {
        setBasicColorArray( TEST_SEQUENCE[i] );
        _delay_ms( BASE_DELAY );
    }
}

int main( void ) {
    // DDRA = 0;
    DDRB = 0b00000010;
    // DDRC = 0b00111111;
    DDRD = 0b01001000;

    // TIMSK1 = ( 1 << TOIE1 );
    // TCCR1B = 0b00000100; // t/256

    uart_init();
    uart_stdio();

    startupTest(); // LED + power test
  
    // main loop
    while ( 1 ) {
        _delay_ms(500);
        // sei();

        uint8_t i = 0;
        char c = '\0';
        while ( c != '\r' ) {
          c = uart_getchar();
          buf[i++] = c;
        }
        buf[i] = '\0';

        // cli();

//        sscanf( buf, "%c%*d.%*d %*d.%*d %*d.%*d %hhu %*d.%*d %hhu.%*d %*c%*c.%*c %c%*c%*c%*c%*c%*c%*c%c\r",
//                     &cmdChar, &ampPercent, &vBat, &mainsDisconnected, &beeper );

//        switch ( cmdChar ) {
//        }
  }
}
