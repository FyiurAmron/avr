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

#define BASE_DELAY  100

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
  { 1, 1, 1 },
  { 0, 0, 0 },
};

void startupTest( void ) {
    for( uint8_t i = 0; i < ARRAY_LENGTH( TEST_SEQUENCE ); i++ ) {
        setBasicColorArray( TEST_SEQUENCE[i] );
        _delay_ms( BASE_DELAY );
    }
}

#define PWM_MAX  100

int main( void ) {
    // DDRA = 0;
    DDRB = 0b00000010;
    // DDRC = 0b00111111;
    DDRD = 0b01001000;

    // TIMSK1 = ( 1 << TOIE1 );
    // TCCR1B = 0b00000100; // t/256

    uart_init();
    uart_stdio();

    // startupTest(); // LED + power test

    sbi( TCCR0A, WGM00 ); // Fast PWM, full 8-bit (1)
    sbi( TCCR0A, WGM01 ); // Fast PWM, full 8-bit (2)
    sbi( TCCR0A, COM0A1 ); // enable OCR0B Fast PWM: Clear on Compare Match, set at TOP
	sbi( TCCR0B, CS00 ); // /1 prescaler

    sbi( TCCR1A, WGM10 ); // Fast PWM, full 8-bit (1)
    sbi( TCCR1A, WGM11 ); // Fast PWM, full 8-bit (2)
    sbi( TCCR1A, COM1A1 ); // enable OCR0B Fast PWM: Clear on Compare Match, set at TOP
	sbi( TCCR1B, CS10 ); // /1 prescaler

    sbi( TCCR2A, WGM20 ); // Fast PWM, full 8-bit (1)
    sbi( TCCR2A, WGM21 ); // Fast PWM, full 8-bit (2)
    sbi( TCCR2A, COM2B1 ); // enable OCR0B Fast PWM: Clear on Compare Match, set at TOP
	sbi( TCCR2B, CS20 ); // /1 prescaler

    TCNT0 = 0;
    TCNT1 = 0;
    TCNT2 = 0;

    // main loop
    volatile uint8_t* rgb[] = { &OCR0A, &OCR1A, &OCR2B };
    uint8_t i = 0;
    uint8_t rise = 1;
    uint8_t idx = 0;
    while ( 1 ) {
        *(rgb[idx]) = rise ? i : PWM_MAX - i;

        if ( i == PWM_MAX ) {
            rise = !rise;
            idx++;
            if ( idx >= ARRAY_LENGTH( rgb ) ) {
                idx = 0;
            }
            i = 0;
        } else {
            i++;
        }

        _delay_ms(5);

        // sei();
/*
        uint8_t i = 0;
        char c = '\0';
        while ( c != '\r' ) {
          c = uart_getchar();
          buf[i++] = c;
        }
        buf[i] = '\0';
*/
        // cli();

//        sscanf( buf, "%c%*d.%*d %*d.%*d %*d.%*d %hhu %*d.%*d %hhu.%*d %*c%*c.%*c %c%*c%*c%*c%*c%*c%*c%c\r",
//                     &cmdChar, &ampPercent, &vBat, &mainsDisconnected, &beeper );

//        switch ( cmdChar ) {
//        }
  }
}
