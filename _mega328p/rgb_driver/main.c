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

volatile uint8_t* rgb[] = {
    // &OCR0A, &OCR1A, &OCR2B  // Vishay RGB (CC)
    // &OCR1A, &OCR0A, &OCR2B // makeshift RGB (CA)
    &OCR0A, &OCR2B, &OCR1A // LED strip
};

/*
// Vishay RGB (CC)
void setRgb( uint8_t red, uint8_t green, uint8_t blue ) {
    bi( PORTD, 6, red );
    bi( PORTB, 1, green );
    bi( PORTD, 3, blue );
}
*/

// MOSFET CA
void setRgb( uint8_t red, uint8_t green, uint8_t blue ) {
    bi( PORTB, 1, red );
    bi( PORTD, 6, green );
    bi( PORTD, 3, blue );
}

void setRgbArray( uint8_t rgb[3] ) {
    setRgb( rgb[0], rgb[1], rgb[2] );
}

void setRgbPwm( uint8_t idx, uint8_t val ) {
/*
    if ( idx == 2 ) { // superbright blue fix
        val = val / 12;
    }
*/
    *(rgb[idx]) = val;
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
        setRgbArray( TEST_SEQUENCE[i] );
        _delay_ms( 100 );
    }
}

#define PWM_MAX  255

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
    setRgb( 0, 0, 0 );

    // PD6 - 8-bit
    sbi( TCCR0A, WGM00 ); // PWM
    sbi( TCCR0A, COM0A1 ); // enable OCR0A Phase Correct PWM
	sbi( TCCR0B, CS00 ); //
    sbi( TCCR0B, CS01 ); // /64 prescaler ~ 500 Hz

    // PB1 - 16-bit
    sbi( TCCR1A, WGM10 ); // PWM
    sbi( TCCR1A, COM1A1 ); // enable OCR1A Phase Correct 8-bit PWM
	sbi( TCCR1B, CS10 ); //
    sbi( TCCR1B, CS11 ); // /64 prescaler ~ 500 Hz

    // PD3 - 8-bit
    sbi( TCCR2A, WGM20 ); // PWM
    sbi( TCCR2A, COM2B1 ); // enable OCR2B Phase Correct PWM
	sbi( TCCR2B, CS20 ); //
    sbi( TCCR2B, CS21 ); // /64 prescaler ~ 500 Hz

    /*
    TCNT0 = 0;
    TCNT1 = 0;
    TCNT2 = 0;
    */

    uint8_t i = 0;
    uint8_t rise = 1;
    uint8_t idx = 0;

    for( uint8_t i = 0; i < ARRAY_LENGTH( rgb ); i++ ) {
        setRgbPwm( i, 0 );
    }

    uint8_t rgbLoop = 0;

    // main loop

    while ( 1 ) {
        if ( rgbLoop ) {
            setRgbPwm( idx, rise ? i : PWM_MAX - i );

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

            _delay_ms(1);
            continue;
        }

        // sei();

        uint8_t i = 0;
        char c = '\0';
        while ( c != '\r' ) {
          c = uart_getchar();
          uart_putchar( c ); // ECHO ON
          buf[i++] = c;
        }
        buf[i] = '\0';

        // cli();

        uint8_t cmdChar, val, idx;
        sscanf( buf, "%c%hhu\r", &cmdChar, &val );

        switch ( cmdChar ) {
          case 'R': idx =   0; break;
          case 'G': idx =   1; break;
          case 'B': idx =   2; break;
          case '@': idx =   3; break;
          default:  idx = 255; break;
        }

        if ( idx <= 2 ) {
            setRgbPwm( idx, val );
        }
        if ( idx == 3 ) {
            rgbLoop = 1;
        }
  }
}
