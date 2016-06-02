#ifndef _UART_H
#define _UART_H  1
// UART code

#define uart_printf(...)  fprintf( &uart_output, __VA_ARGS__ )

void uart_init( void );
void uart_as_stdio( void );
char uart_getchar( void );
void uart_putchar( char c );
int _uart_getchar_FDEV( FILE *stream ); // internal
int _uart_putchar_FDEV( char c, FILE *stream ); // internal

FILE uart_input  = FDEV_SETUP_STREAM( NULL, _uart_getchar_FDEV, _FDEV_SETUP_READ  );
FILE uart_output = FDEV_SETUP_STREAM( _uart_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );

#ifdef SINGLE_FILE
#include "uart.c"
#endif

#endif // _UART_H