// debug
#include "debug.h"

void debug_hexDump( uint8_t* data, uint16_t len ) {
    uint8_t* ptr = data, *limitPtr = data + len;
    len = 0; // reuse
    for( ; ptr < limitPtr; ptr += DEBUG_HEX_DUMP_LINE_LENGTH, len += DEBUG_HEX_DUMP_LINE_LENGTH ) {
        printf( " 0x%04hx |", len );
        for( uint8_t j = 0; j < DEBUG_HEX_DUMP_LINE_LENGTH; j++ ) {
            printf( " %02hhx", ptr[j] );
        }
        printf( " | " );
        for( uint8_t j = 0; j < DEBUG_HEX_DUMP_LINE_LENGTH; j++ ) {
            putchar( ( DEBUG_HEX_DUMP_COND1( ptr[j] ) || DEBUG_HEX_DUMP_COND2( ptr[j] ) ) ? '.' : ptr[j] );
        }
        printf( "\n\r" );
    }
}
