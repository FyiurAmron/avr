//
#include "uart.h"
#include "macro.h"

#ifndef UART_NO_STREAMS
FILE _uart_input  = FDEV_SETUP_STREAM( NULL, _uart_getchar_FDEV, _FDEV_SETUP_READ  );
FILE _uart_output = FDEV_SETUP_STREAM( _uart_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );
FILE* const uart_input  = &_uart_input;
FILE* const uart_output = &_uart_output;

void uart_stdio( void ) {
    stdin  = uart_input;
    stdout = uart_output;
}

int _uart_getchar_FDEV( FILE *stream ) {
#ifdef UART_INPUT_ECHO
    return uart_getcharEcho();
#else
    return uart_getchar();
#endif
}

int _uart_putchar_FDEV( char c, FILE *stream ) {
    uart_putchar(c);
    return c;
}
#endif

void uart_init( void ) {
// UBRRH/L_VALUE and USE_2X is set by setbaud.h
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X == 1
    bit8_set( UCSR0A, BV(U2X0) );
#elif USE_2X == 0
    bit8_clear( UCSR0A, BV(U2X0) );
#else
#error "Unknown USE_2X value"
#endif

    // default settings: async, 8-N-1 ; override by setting UCSR0C/UCSR0B
    UCSR0B  = BV(RXEN0);
    bit8_set( UCSR0B, BV(TXEN0) );
}

uint8_t uart_getchar( void ) {
    while ( !bit8_and( UCSR0A, BV(RXC0) ) ) { /* until RX buffer ready (nonempty) */ }
    return UDR0;
}

uint8_t uart_getcharEcho( void ) {
    uint8_t ret = uart_getchar();
    uart_putchar( ret );
    return ret;
}

void uart_putchar( uint8_t c ) {
    while ( !bit8_and( UCSR0A, BV(UDRE0) ) ) { /* until TX buffer ready (has space for input) */ }
    UDR0 = c;
}
