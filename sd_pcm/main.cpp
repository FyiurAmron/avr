/*
  ATmega @ 8MHz
  Plays an 8bit PCM audio on OC1A output
*/

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "pcm_data.h"

const uint8_t SAMPLE_RATE_DIVISOR = 3;
const uint16_t SAMPLE_RATE = 31250 / SAMPLE_RATE_DIVISOR; // about 10416 Hz
const uint16_t PCM_LENGTH = sizeof(PCM_DATA) / sizeof(unsigned char);

void pwm_init(void) {
  // set /1 system clock prescaler instead of default /8
  CLKPR = _BV(CLKPCE);
  CLKPR = 0;
  
  // setup PWM
  DDRD = _BV(PD5); // set OC1A pin as output

  TCCR1A = _BV(COM1A1) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
   
  TCCR0B |= _BV(CS00);  // enable timer 0
  TIMSK0 |= _BV(TOIE0); // enable timer 0 interrupt
  
  // little tweak
  DDRA  = 0b00000000;
  PORTA = 0b11111111;
    
  sei();
}

uint8_t timer_count = SAMPLE_RATE_DIVISOR;
uint16_t sample = PCM_LENGTH / 2;
uint8_t pass = 0;
uint8_t out = 0;
uint8_t mplier = 1;

void play_sample( void ) {
  mplier = 1;
  timer_count = SAMPLE_RATE_DIVISOR;            
  OCR1A = pgm_read_byte(&PCM_DATA[sample]);
  if ( sample != PCM_LENGTH - 1 ) {
    sample++;
    return;
  }
  if ( pass == 2 ) {
    sample = 0;
    pass = 0;
  } else {
    sample = PCM_LENGTH / 2;
    pass++;
  }
}
 
void play_fx( void ) {
  sample = 0;
  timer_count = SAMPLE_RATE_DIVISOR * mplier;
  mplier++;
  out = ( out == 0 ) ? 255 : 0 ;
            
  OCR1A = out;
}

ISR( TIMER0_OVF_vect ) {
  timer_count--;
  if ( timer_count != 0 )
    return;

  if ( PINA & 0b00000001 )
    play_sample();
  else
    play_fx();
}

int main(void) {
  pwm_init();
  while( true )
   ;
}
