#define F_CPU  20000000UL
//#define F_CPU 1000000UL
#define COMPILE_SINGLE_FILE
#include "vax/misc.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define BAUD  9600
#include "vax/uart.h"

int main( void ) {
    init();

    uart_init();
    uart_stdio();
    printf( "\n\rUART device started: %ld %ld %d\n\r", UBRRH_VALUE, UBRRL_VALUE, USE_2X );

    for(;;) {
        uart_putchar( '*' );
        uart_putchar( ' ' );
        _delay_ms(1000);
    }
} // main()
