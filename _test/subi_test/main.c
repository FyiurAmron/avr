#include <avr/io.h>

extern uint8_t external( void );

uint8_t zz( void ) {
    return external();
}

int main( void ) {
    uint8_t x1 = zz();
    x1++;
    if ( x1 == 0 ) {
        external();
    }
    for(;;) {
    }
}