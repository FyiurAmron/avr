#ifndef _MUX_OUTPUT_H
#define _MUX_OUTPUT_H  1
// output mux
#include <stdbool.h>
#ifndef MUX_MAX
#define MUX_MAX  4
#endif

FILE* mux_outputs[MUX_MAX];
uint8_t mux_cnt;

bool mux_add( FILE* output );
void mux_putchar( char c );
int _mux_putchar_FDEV( char c, FILE *stream ); // internal

FILE mux_output = FDEV_SETUP_STREAM( _mux_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );

#ifdef SINGLE_FILE
#include "mux_output.c"
#endif

#endif // _MUX_OUTPUT_H