//
#include "mux_output.h"

bool mux_add( FILE* output ) {
    if ( mux_cnt == MUX_MAX ) {
        return false;
    }
    mux_outputs[mux_cnt] = output;
    mux_cnt++;
    return true;
}

void mux_putchar( char c ) {
    for( int8_t i = mux_cnt - 1; i >= 0; i-- ) {
        putc( c, mux_outputs[i] );
    }
}

int _mux_putchar_FDEV( char c, FILE *stream ) {
    mux_putchar( c );
    return 0;
}
