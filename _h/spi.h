#ifndef	_SPI_H
#define _SPI_H  1
// SPI helpers
#if !defined(SPI_SCK_LINE) || !defined(SPI_SCK_PIN_NR) \
 || !defined(SPI_MISO_LINE) || !defined(SPI_MISO_PIN_NR) \
 || !defined(SPI_MOSI_LINE) || !defined(SPI_MOSI_PIN_NR)
#error "missing one or more needed definitions: SPI_SCK_LINE, SPI_SCK_PIN_NR, SPI_MISO_LINE, SPI_MISO_PIN_NR, SPI_MOSI_LINE, SPI_MOSI_PIN_NR"
#else
#include <avr/io.h>
#include "macro.h"

#define SPI_SCK          BV(SPI_SCK_PIN_NR)
#define SPI_MISO         BV(SPI_MISO_PIN_NR)
#define SPI_MOSI         BV(SPI_MOSI_PIN_NR)
#define SPI_SS           BV(SPI_SS_PIN_NR)

#ifdef SPI_SS_LINE
#define spi_assert_SS()    bit8_clear( xPORT(SPI_SS_LINE), SPI_SS )
#define spi_deassert_SS()  bit8_set( xPORT(SPI_SS_LINE), SPI_SS )
#else
#define spi_assert_SS()    EMPTY_STATEMENT
#define spi_deassert_SS()  EMPTY_STATEMENT
#endif

#define SPI_EMPTY_BYTE   0xFF

#define SPI_FREQ_DIV2    0b0100
#define SPI_FREQ_DIV4    0b0000
#define SPI_FREQ_DIV8    0b0101
#define SPI_FREQ_DIV16   0b0001
#define SPI_FREQ_DIV32   0b0110
#define SPI_FREQ_DIV64   0b0010
#define SPI_FREQ_DIV64x  0b0111
#define SPI_FREQ_DIV128  0b0011

void spi_preinit( void );
void spi_init( uint8_t speed );
uint8_t spi_send( uint8_t out );
#ifdef SPI_RECV_MACRO
#define spi_recv()  spi_send( SPI_EMPTY_BYTE )
#else
uint8_t spi_recv( void );
#endif

#ifdef COMPILE_SINGLE_FILE
#include "spi.c"
#endif

#endif // req. macros
#endif // _SPI_H
