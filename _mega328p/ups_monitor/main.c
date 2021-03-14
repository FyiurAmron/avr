#define F_CPU  8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define COMPILE_SINGLE_FILE

#define BAUD  2400
#include "vax/uart.h"

#define PORTC_MAX  6

#define TEST_DELAY  100

char buf[256];

volatile uint8_t portC;
volatile uint8_t portD;
volatile uint8_t timeFlag;
volatile uint8_t mainsDisconnected;

uint8_t EEMEM vThreshold = 21;

ISR( TIMER1_OVF_vect ) {
    if ( timeFlag && mainsDisconnected == '1' ) {
        PORTC = 0;
        PORTD = 0;
    } else {
        PORTC = portC;
        PORTD = portD;
    }
    timeFlag = !timeFlag;

    puts( "Q1\r" );
}

void startupTest( void ) {
    for ( uint8_t i = 0; i < PORTC_MAX; i++ ) {
        PORTC = 1 << i;
        _delay_ms( TEST_DELAY );
    }
    PORTC = 0;
    _delay_ms( TEST_DELAY );
    PORTD = 0b00000100;
    _delay_ms( TEST_DELAY );
    PORTD = 0;
    _delay_ms( TEST_DELAY );
    PORTD = 0b00000100;
    _delay_ms( TEST_DELAY );
    PORTD = 0;
    for ( int8_t i = PORTC_MAX - 1; i >= 0; i-- ) {
        PORTC = 1 << i;
        _delay_ms( TEST_DELAY );
    }
    PORTC = 0;
}

int main( void ) {
    // DDRA = 0;
    DDRB = 0;
    DDRC = 0b00111111;
    DDRD = 0b00000100;

    TIMSK1 = ( 1 << TOIE1 );
    TCCR1B = 0b00000100; // t/256

    uart_init();
    uart_stdio();

    startupTest(); // LED + power test
  
    // main loop
    while ( 1 ) {
        _delay_ms(500);
        sei();

        uint8_t i = 0;
        char c = '\0';
        while ( c != '\r' ) {
          c = uart_getchar();
          buf[i++] = c;
        }
        buf[i] = '\0';

        cli();

        uint8_t vBat;
        uint8_t ampPercent;

        uint8_t beeper;
        uint8_t cmdChar;
        // note the vBat format SS.S is for stand-by UPS
        // also workaround the TT.T returned as --.- for some UPSes (Quer anyone?)
        sscanf( buf, "%c%*d.%*d %*d.%*d %*d.%*d %hhu %*d.%*d %hhu.%*d %*c%*c.%*c %c%*c%*c%*c%*c%*c%*c%c\r",
                     &cmdChar, &ampPercent, &vBat, &mainsDisconnected, &beeper );

        switch ( cmdChar ) {
          case '(': { // the actual command we get from the UPS
            int8_t vDiff = vBat - eeprom_read_byte( &vThreshold );

                 if ( vDiff > 5 ) { portC = 0b00000001; portD = 0b00000000; }
            else if ( vDiff > 4 ) { portC = 0b00000010; portD = 0b00000000; }
            else if ( vDiff > 3 ) { portC = 0b00000100; portD = 0b00000000; }
            else if ( vDiff > 2 ) { portC = 0b00001000; portD = 0b00000000; }
            else if ( vDiff > 1 ) { portC = 0b00010000; portD = 0b00000000; }
            else if ( vDiff > 0 ) { portC = 0b00100000; portD = 0b00000000; }
            else                  { portC = 0b00000000; portD = 0b00000100; }
            break;
          }
          case '!': { // setting vThreshold via direct UART connection, e.g.
            eeprom_write_byte( &vThreshold, vBat ); // !0.0 0.0 0.0 000 0.0 21.0 --.- 00000000
            break; // would set the threshold to 21V
          }
        }

        if ( beeper == '1' ) {
            puts( "Q\r" );
        }
    }
}
