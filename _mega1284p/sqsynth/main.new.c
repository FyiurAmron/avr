#define F_CPU  20000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <vax/misc.h>
#include <vax/bits.h>

#define PWM_PORT      B
#define PWM_PORT_BIT  4

// fine tune here:
#define PWM_FREQ  51500

#include "freq.h"

#define HLD  0
#define RST  1

#define NOTE_TIMER_COUNT  ( ( ( F_CPU * 60 * 2 / PWM_SCALER / BPM ) + SHORTEST_NOTE / 2 ) / SHORTEST_NOTE )
// 60 - seconds per minute; 2 - Fast PWM factor; added 0.5-ish rounding; actual result is e.g.
// (F_CPU 20MHz, PWM_SCALER 256, BPM 120, SHORTEST_NOTE 16):
//#define NOTE_TIMER_COUNT  4882

#define SHORTEST_NOTE  16
#define BPM  120
//#define CHANNELS  5
#define CHANNELS  4

PROGMEM const uint16_t notes[] = {
    D_1, HLD, HLD, HLD, D_2, HLD, HLD, HLD, HLD, HLD, HLD, HLD, HLD, HLD, HLD, HLD,
    A_0, HLD, HLD, HLD, HLD, HLD, C_1, HLD, HLD, HLD, HLD, HLD, Db1, HLD, HLD, HLD,

    RST, RST, RST, RST, D_3, HLD, HLD, HLD, A_2, HLD, HLD, HLD, C_3, HLD, HLD, HLD,
    A_2, HLD, HLD, HLD, HLD, HLD, C_3, HLD, HLD, HLD, HLD, HLD, Db3, HLD, HLD, HLD,

    RST, RST, RST, RST, D_6, HLD, RST, RST, RST, RST, RST, RST, D_6, HLD, RST, RST,
    RST, RST, RST, RST, D_6, RST, D_6, RST, RST, RST, RST, RST, D_6, HLD, RST, RST,

    D_4, HLD, G_4, HLD, D_4, HLD, F_4, HLD, HLD, HLD, D_4, HLD, C_4, HLD, F_3, HLD,
    D_4, HLD, HLD, HLD, HLD, HLD, F_3, HLD, HLD, HLD, HLD, HLD, G_4, HLD, HLD, HLD,
/*
    D_5, HLD, G_5, HLD, D_5, HLD, F_5, HLD, HLD, HLD, D_5, HLD, C_5, HLD, F_4, HLD,
    D_5, HLD, HLD, HLD, HLD, HLD, F_4, HLD, HLD, HLD, HLD, HLD, G_5, HLD, HLD, HLD,
*/
};

#define BASE_VOLUME  ( 256 / CHANNELS - 1 )

#define V_100  BASE_VOLUME
#define V_50   ( BASE_VOLUME / 2 )
#define V_25   ( BASE_VOLUME / 4 )

/*
uint8_t volumes[] = {
    V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100,
    V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100, V_100,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
*/

PROGMEM const uint8_t volumes[] = {
    V_100, V_100, V_100, V_100,
//    V_100
};

//#define PATTERN_LENGTH  ( ARRAY_LENGTH(notes) / CHANNELS )
#define PATTERN_LENGTH  32

#define OFFSET(x,y)  ( PATTERN_LENGTH * x + y )
#define MAX_PATTERN_COUNT  4+4+4+8


volatile uint8_t interruptCounter;

#define TIMER_OVF_vect TIMER0_OVF_vect

ISR( TIMER_OVF_vect ) {
    interruptCounter++;
}

inline void pwmOut( uint8_t pwmValue ) {
    OCR0B = pwmValue;
}

inline uint16_t getNote( uint8_t noteNr, uint8_t channelNr ) {
    // return notes[noteNr + channelNr * PATTERN_LENGTH];
    return pgm_read_dword( notes + noteNr + channelNr * PATTERN_LENGTH );
}

inline uint8_t getVolume( uint8_t noteNr, uint8_t channelNr ) {
    // return volumes[noteNr + channelNr * PATTERN_LENGTH];
    // return volumes[channelNr];
    return pgm_read_byte( volumes + noteNr + channelNr * PATTERN_LENGTH );
}

int main( void ) {
    // init & setup
    init();

    /*
    // provide UART for debugging
    uart_init();
    uart_stdio();
    printf( "\n\rUART device started: %ld %ld %d\n\r", UBRRH_VALUE, UBRRL_VALUE, USE_2X );
    */

	bit8_set( xDDR(PWM_PORT), BV(PWM_PORT_BIT) ); // set PWM pin as output

    bit8_set( TCCR0A, BV(WGM01) | BV(WGM00) ); // Fast PWM, full 8-bit
    bit8_set( TCCR0A, BV(COM0B1) ); // enable OCR0B Fast PWM: Clear on Compare Match, set at TOP
	bit8_set( TCCR0B, BV(CS00) ); // full speed
    bit8_set( TIMSK0, BV(TOIE0) ); // PWM/Timer overflow ("tick") interrupt enable

    // synth variables
    uint8_t noteNr = 0;
    uint16_t timer = 0;
    uint8_t patternCnt = 0;

    uint16_t channelCounter[CHANNELS];
    uint16_t channelToneCycles[CHANNELS];
    uint16_t channelVolume[CHANNELS];

    for( uint8_t i = 0, offset = 0; i < CHANNELS; i++, offset += PATTERN_LENGTH ) {
        channelToneCycles[i] = getNote( offset, 0 );
        //channelVolume[i] = getVolume( offset, 0 );
        channelVolume[i] = getVolume( i, 0 );
    }

    sei();
    while(1) {
        uint8_t pwmValue = 0;
        for( int8_t i = 0; i < CHANNELS; i++ ) {
            channelCounter[i] += interruptCounter;
            interruptCounter = 0;
            if ( channelCounter[i] > channelToneCycles[i] / 2 ) {
                if ( channelCounter[i] >= channelToneCycles[i] ) {
                    channelCounter[i] = 0;
                }
                pwmValue += channelVolume[i];
            }
        }
        pwmOut( pwmValue );

        timer += interruptCounter;
        if ( timer <= NOTE_TIMER_COUNT ) {
            continue;
        }
        timer = 0;

        noteNr++;
        if ( noteNr == PATTERN_LENGTH ) {
            patternCnt++;
            if ( patternCnt == MAX_PATTERN_COUNT ) {
                patternCnt = 0;
            }
            noteNr = 0;
        }

        for( uint8_t i = 0, noteOffset = 0; i < CHANNELS; i++, noteOffset += PATTERN_LENGTH ) {
            uint16_t note = getNote( noteNr + noteOffset, 0 ); // TODO use ptr maybe?

            if ( note == RST ) {
                channelVolume[i] = 0;        
            } else {
                if ( note != HLD ) {
                     channelToneCycles[i] = note;
                }
                //channelVolume[i] = getVolume( noteNr + noteOffset, 0 ); // TODO use ptr maybe?
                //channelVolume[i] = getVolume( i, 0 );
                channelVolume[i] = ( i <= patternCnt / 4 ) ? getVolume( i, 0 ) : 0;
            }
        }
    } // while(1)
}
