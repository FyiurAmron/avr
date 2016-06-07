//
#include "spi.h"

void spi_preinit( void ) {
    xDDR(SPI_SCK_LINE)  |= SPI_SCK;
    xDDR(SPI_MISO_LINE) &=~SPI_MISO;
    xDDR(SPI_MOSI_LINE) |= SPI_MOSI;
#ifdef SPI_SS_LINE
    xDDR(SPI_SS_LINE)   |= SPI_SS;
#endif
    //assert_SS();
}

void spi_init( uint8_t speed ) {
    SPCR = _BV(SPE) | _BV(MSTR); // SPI Enable, Master
    SPCR |= ( speed & 0b0011 );
    if ( speed & 0b0100 ) {
        SPSR |= _BV(SPI2X);
    } else {
        SPSR &=~_BV(SPI2X);
    }
}

/*
uint8_t spi_sync( void ) {
    spi_assert_SS();
    for( uint8_t i = 10; i > 0; i-- ) { // idle for 10 bytes / 80 clocks
        spi_send( SPI_EMPTY_BYTE );
    }
    spi_deassert_SS();
}
*/

uint8_t spi_send( uint8_t out ) {
    SPDR = out;
    while( !(SPSR & _BV(SPIF)) ) {} // wait for SPIF flag in SPSR
    return SPDR;
}

#ifdef SPI_STREAMS
FILE _spi_input  = FDEV_SETUP_STREAM( NULL, _spi_getchar_FDEV, _FDEV_SETUP_READ  );
FILE _spi_output = FDEV_SETUP_STREAM( _spi_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );
FILE* const spi_input  = &_spi_input;
FILE* const spi_output = &_spi_output;

void spi_stdio( void ) {
    stdin  = spi_input;
    stdout = spi_output;
}

int _uart_getchar_FDEV( FILE *stream ) {
    do {
    uint8_t ret = spi_send( SPI_EMPTY_BYTE );
    } while( ret == SPI_EMPTY_BYTE );
    return ret;
}

int _uart_putchar_FDEV( char c, FILE *stream ) {
    spi_send(c);
    return c;
}
#endif
