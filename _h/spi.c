//
#include "spi.h"

void spi_preinit( void ) {
    bit8_set( xDDR(SPI_SCK_LINE), SPI_SCK );
    bit8_clear( xDDR(SPI_MISO_LINE), SPI_MISO );
    bit8_set( xDDR(SPI_MOSI_LINE), SPI_MOSI );
#ifdef SPI_SS_LINE
    bit8_set( xDDR(SPI_SS_LINE), SPI_SS );
#endif
    //assert_SS();
}

void spi_init( uint8_t speed ) {
    SPCR = BV(MSTR); // set Master
    bit8_set( SPCR, BV(SPE) ); // SPI Enable
    bit8_set( SPCR, bit8_and( speed, 0b0011 ) );
    if ( bit8_and( speed, 0b0100 ) ) {
        bit8_set( SPSR, BV(SPI2X) );
    } else {
        bit8_clear( SPSR, BV(SPI2X) );
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
    while ( !bit8_and( SPSR, BV(SPIF) ) ) { /* wait for SPIF flag in SPSR */ }
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
