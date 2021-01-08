#define F_CPU  8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define COMPILE_SINGLE_FILE

#define BAUD  2400
#include "vax/uart.h"

char buf[256];

volatile uint8_t portC;
volatile uint8_t portD;
volatile uint8_t timeFlag;
volatile uint16_t vIn;

ISR( TIMER1_OVF_vect ) {
    if ( timeFlag && vIn == 0 ) {
        PORTC = 0;
        PORTD = 0;
    } else {
        PORTC = portC;
        PORTD = portD;
    }
    timeFlag = !timeFlag;

    puts( "Q1\r" );
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

    // puts( "Q\r" );
   
    while(1) {
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
        sscanf( buf, "(%d.%*d %*d.%*d %*d.%*d %hhu %*d.%*d %hhu.%*d %*d.%*d %*c%*c%*c%*c%*c%*c%*c%c\r", &vIn, &ampPercent, &vBat, &beeper );

             if ( vBat > 24 ) { portC = 0b00000001; portD = 0b00000000; }
        else if ( vBat > 20 ) { portC = 0b00000010; portD = 0b00000000; }
        else if ( vBat > 16 ) { portC = 0b00000100; portD = 0b00000000; }
        else if ( vBat > 12 ) { portC = 0b00001000; portD = 0b00000000; }
        else if ( vBat >  8 ) { portC = 0b00010000; portD = 0b00000000; }
        else if ( vBat >  4 ) { portC = 0b00100000; portD = 0b00000000; }
        else               { portC = 0b00000000; portD = 0b00000100; }

        if ( beeper == '1' ) {
            puts( "Q\r" );
        }
    }
}
