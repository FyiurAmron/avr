//
#include "uart.h"

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

#ifdef USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &=~(_BV(U2X0));
#endif

    // default settings: async, 8-N-1 ; override by setting UCSR0C/UCSR0B
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

uint8_t uart_getchar( void ) {
    loop_until_bit_is_set( UCSR0A, RXC0 ); // until RX buffer ready (nonempty)
    return UDR0;
}

uint8_t uart_getcharEcho( void ) {
    uint8_t ret = uart_getchar();
    uart_putchar( ret );
    return ret;
}

void uart_putchar( uint8_t c ) {
    loop_until_bit_is_set( UCSR0A, UDRE0 ); // until TX buffer ready (has space for input)
    UDR0 = c;
}
