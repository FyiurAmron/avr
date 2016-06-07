//
#include "mux_output.h"

FILE* mux_outputs[MUX_MAX];
uint8_t mux_cnt;

FILE _mux_output = FDEV_SETUP_STREAM( _mux_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );
FILE* const mux_output = &_mux_output;

bool mux_add( FILE* output ) {
    if ( mux_cnt == MUX_MAX ) {
        return false;
    }
    mux_outputs[mux_cnt] = output;
    mux_cnt++;
    return true;
}

void mux_putchar( uint8_t c ) {
    for( int8_t i = mux_cnt - 1; i >= 0; i-- ) {
        putc( c, mux_outputs[i] );
    }
}

int _mux_putchar_FDEV( char c, FILE *stream ) {
    mux_putchar( c );
    return c;
}
