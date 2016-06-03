#ifndef _MUX_OUTPUT_H
#define _MUX_OUTPUT_H  1
// output mux
#include <stdbool.h>
#ifndef MUX_MAX
#define MUX_MAX  4
#endif

bool mux_add( FILE* output );
void mux_putchar( char c );
int _mux_putchar_FDEV( char c, FILE *stream ); // internal

extern FILE* mux_outputs[MUX_MAX];
extern uint8_t mux_cnt;

extern FILE _mux_output;
extern FILE* const mux_output;

#ifdef COMPILE_SINGLE_FILE
#include "mux_output.c"
#endif

#endif // _MUX_OUTPUT_H