//

#include "main.h"

int main( void ) {
    init();
    //while(1) {} // to quickly disable uC code

    uart_init();
    uart_stdio();

    uart_printf( "\n\r" );
    uart_printf( "* UART: OK\n\r" );

    while(1) {
        uint16_t dist = dist_pingHQ();
        switch( dist ) {
            case DIST_SENSE_OUT_OF_RANGE:
                printf( "\n\r dist out of range\n\r" );
                break;
            case DIST_SENSE_HANG_UP:
                printf( "\n\r wait for sensor reset...\n\r" );
                break;
            default:
                printf( "\n\r ping: %d cm\n\r", dist );
                break;
        }
        _delay_ms(500);
    } // while(1)
} // main()
