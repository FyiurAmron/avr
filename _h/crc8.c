//
#include "crc8.h"

uint8_t CRC_bytes( uint8_t* data, uint8_t len ) {
    uint8_t CRC = 0;
    for( int i = 0; i < len; i++ ) {
        CRC = CRC_byte( CRC, data[i] );
    }
    return CRC;
}

uint8_t CRC_byte( uint8_t oldCRC, uint8_t data ) {
    return pgm_read_byte( CRC_table + ((uint8_t)( oldCRC << 1 ) ^ data) );
}
