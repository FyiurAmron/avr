#ifndef _UART_H
#define _UART_H  1
// UART code
#include <stdio.h>
#include <avr/io.h>

#ifndef BAUD
#define BAUD  115200
#endif

#ifndef UBRRL_VALUE
#include <util/setbaud.h>
#endif

void uart_init( void );
void uart_stdio( void );
uint8_t uart_getchar( void );
uint8_t uart_getcharEcho( void );
void uart_putchar( uint8_t c );

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
