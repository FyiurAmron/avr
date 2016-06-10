#define F_CPU  8000000UL
//#define F_CPU 1000000UL
#define COMPILE_SINGLE_FILE
#include "vax/misc.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "vax/uart.h"

int main( void ) {
    init();

    uart_init();
    uart_stdio();

    for(;;) {
        //printf( "\n\rDevice started!\n\r" );
        uart_putchar( '~' );
        _delay_ms(1000);
    }
} // main()
