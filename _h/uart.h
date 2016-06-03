#ifndef _UART_H
#define _UART_H  1
// UART code

void uart_init( void );
void uart_as_stdio( void );
char uart_getchar( void );
void uart_putchar( char c );

#ifndef UART_NO_STREAMS
#define uart_printf(...)  fprintf( uart_output, __VA_ARGS__ )
int _uart_getchar_FDEV( FILE *stream ); // internal
int _uart_putchar_FDEV( char c, FILE *stream ); // internal

extern FILE _uart_input;
extern FILE _uart_output;
extern FILE* const uart_input;
extern FILE* const uart_output;
#endif

#ifdef COMPILE_SINGLE_FILE
#include "uart.c"
#endif

#endif // _UART_H