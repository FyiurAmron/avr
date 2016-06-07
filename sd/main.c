//
#include "main.h"

int main( void ) {
    init();
    //while(1) {} // to quickly disable uC code
    _delay_ms(100);

    uart_init();
    uart_stdio();

    uart_printf( "\n\r" );
    uart_printf( "* UART: OK\n\r" );
    while(1) {
    uart_printf( "press any key to continue...\n\r" );
    uart_getchar();
    uart_printf( "* SPI: " );
    spi_preinit();
    spi_init( SPI_FREQ_DIV2 );
    uart_printf( "OK\n\r" );

    uart_printf( "* SD: " );
    sd_preinit();
    uint8_t sd_init_ret = sd_init();
    uart_printf( sd_init_ret == SD_INIT_NO_ERROR ? "OK" : "failed");
    uart_printf( " [%hhu] \n\r", sd_init_ret );
    } // while(1)
} // main()
