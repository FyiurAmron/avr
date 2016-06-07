#ifndef _CRC8_H
#define _CRC8_H  1
#ifndef CRC_TABLE
#error "please define CRC_TABLE first"
#else
#include <avr/pgmspace.h>
#include <stdint.h>
#include CRC_TABLE

uint8_t CRC_bytes( uint8_t* data, uint8_t len );
uint8_t CRC_byte( uint8_t oldCRC, uint8_t data );

#ifdef COMPILE_SINGLE_FILE
#include "crc8.c"
#endif

#endif
#endif // _CRC8_H