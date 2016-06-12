/*
  ATmega @ 8MHz
  Plays an 8bit PCM audio on OC1A output
*/

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "pcm_data.h"

//const uint8_t SAMPLE_RATE_DIVISOR = 3;
//const uint16_t SAMPLE_RATE = 31250 / SAMPLE_RATE_DIVISOR; // about 10416 Hz
const uint16_t PCM_LENGTH = sizeof(PCM_DATA) / sizeof(unsigned char);

volatile uint8_t tick = 0;

void pwm_init(void) {
  DDRD = _BV(PD5); // set OC1A pin as output

  TCCR1A = _BV(COM1A1) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
   
  TCCR0B |= _BV(CS00);  // enable timer 0
  TIMSK0 |= _BV(TOIE0); // enable timer 0 interrupt
  
  OCR1A = 0x80;
}

ISR( TIMER0_OVF_vect ) { // overflow, 1/256 clocks i.e. 78125/sec
    tick++;
}

int main(void) {
  const uint8_t *sample = PCM_DATA;
  const uint8_t *sampleLimit = sample + PCM_LENGTH - 1;
  pwm_init();
  sei();
  for(;;) {
    if ( tick == 4 ) {
        tick = 0;
        OCR1A = pgm_read_byte(sample);
        if ( sample == sampleLimit ) {
          sample = PCM_DATA;
        } else {
          sample++;
        }
    }
  }
}
