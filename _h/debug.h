#ifndef _DEBUG_H
#define _DEBUG_H  1

#define DEBUG_HEX_DUMP_LINE_LENGTH  16
#define DEBUG_HEX_DUMP_COND1(x)     ( (x) < 0x20 )
#define DEBUG_HEX_DUMP_COND2(x)     ( (x) == 0x7E )

#include <stdint.h>

void debug_hexDump( uint8_t* data, uint16_t len );

#ifdef COMPILE_SINGLE_FILE
#include "debug.c"
#endif

#endif // _DEBUG_H