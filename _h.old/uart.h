#ifndef _UART_H
#define _UART_H  1
// UART code

void uart_init( void );
void uart_as_stdio( void );
char uart_getchar( void );
void uart_putchar( char c );
int _uart_getchar_FDEV( FILE *stream ); // internal
int _uart_putchar_FDEV( char c, FILE *stream ); // internal

FILE uart_input  = FDEV_SETUP_STREAM( NULL, _uart_getchar_FDEV, _FDEV_SETUP_READ  );
FILE uart_output = FDEV_SETUP_STREAM( _uart_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );

void uart_init( void ) {
// UBRRH/L_VALUE and USE_2X is set by setbaud.h
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &=~(_BV(U2X0));
#endif

    // default settings: async, 8-N-1 ; override by setting UCSR0C/UCSR0B
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */    
}

void uart_as_stdio( void ) {
    stdin  = &uart_input;
    stdout = &uart_output;
}

char uart_getchar( void ) {
    loop_until_bit_is_set( UCSR0A, RXC0 ); // until RX buffer ready (nonempty)
    return UDR0;
}

void uart_putchar( char c ) {
    loop_until_bit_is_set( UCSR0A, UDRE0 ); // until TX buffer ready (has space for input)
    UDR0 = c;
}

int _uart_getchar_FDEV( FILE *stream ) {
    return uart_getchar();
}

int _uart_putchar_FDEV( char c, FILE *stream ) {
    uart_putchar(c);
    return 0;
}

#endif // _UART_H