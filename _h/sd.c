// SD card routines
#include "sd.h"

void sd_preinit( void ) {
    spi_assert_SS();
	for( uint8_t i = SD_INIT_IDLE_CYCLES; i > 0; i-- ) {
        sd_debug_printf( "~" );
		spi_send( SPI_EMPTY_BYTE );
    }
    sd_debug_printf( "|\n\r" );
    spi_deassert_SS();
}

uint8_t sd_commandEx( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t crc, uint8_t* retBuf ) {
    sd_debug_printf( "CMD: %hhu ARG: 0x%02hhx %02hhx %02hhx %02hhx CRC: 0x%02hhx\n\r",
                  cmd, arg0, arg1, arg2, arg3, crc );
    spi_assert_SS();
	spi_send( cmd | 0x40 );
	spi_send( arg0 );
	spi_send( arg1 );
	spi_send( arg2 );
	spi_send( arg3 );
	spi_send( crc );

    for( uint8_t i = SD_SPI_RESPONSE_WAIT_MAX; i > 0; i-- ) {
        retBuf[0] = spi_send( SPI_EMPTY_BYTE );
        if ( retBuf[0] == SPI_EMPTY_BYTE  ) {
            continue;
        }
        for( uint8_t j = 1; j < SD_RET_LEN_MAX; j++ ) {
            retBuf[j] = spi_send( SPI_EMPTY_BYTE );
        }
        spi_send( SPI_EMPTY_BYTE );
	    spi_deassert_SS();
        sd_debug_printf( "RET: 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx\n\r",
                      retBuf[0], retBuf[1], retBuf[2], retBuf[3], retBuf[4] );
        return retBuf[0];
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: error: no response\n\r" );
    return SPI_EMPTY_BYTE;
}

uint8_t _sd_legacy_cmd1_init( uint8_t* retBuf ) {
    for( uint8_t i = SD_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_SEND_OP_COND;
        }
        sd_commandEx( SD_SEND_OP_COND, 0, 0, 0, 0, 0xF9, retBuf ); // i.e. CMD1
        if ( retBuf[0] == SD_RET_READY ) {
            return SD_INIT_NO_ERROR;
        }
        if ( retBuf[0] != SD_RET_IDLE ) {
            return SD_SEND_OP_COND;
        }
    }
}

// returns which CMD triggered error, or 0xFF if no error
uint8_t sd_initEx( uint8_t* retBuf, bool useHcs ) {
    for( uint8_t i = SD_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_GO_IDLE_STATE;
        }
        sd_commandEx( SD_GO_IDLE_STATE, 0, 0, 0, 0, 0x95, retBuf ); // i.e. CMD0
        if ( retBuf[0] != SPI_EMPTY_BYTE ) {
            break; // accept strange but non-0xFF responses
        }
    }
    sd_commandEx( SD_SEND_IF_COND, 0, 0, 1, 0xAA, 0x87, retBuf ); // i.e. CMD8
    if ( !( retBuf[0] & SD_RET_IDLE ) || ( retBuf[0] & SD_RET_ZERO_START_BIT ) ) {
        return SD_SEND_IF_COND;
    }
    if ( retBuf[0] & SD_RET_ILLEGAL_CMD ) {
        return _sd_legacy_cmd1_init( retBuf );
    }
    if ( retBuf[0] != SD_RET_IDLE ) {
        return SD_SEND_IF_COND;
    }
    uint8_t acmd41_arg0;
    uint8_t acmd41_crc;
    if ( useHcs ) {
        acmd41_arg0 = 0x40;
        acmd41_crc = 0x77;
    } else {
        acmd41_arg0 = 0x00;
        acmd41_crc = 0xE5;
    }
    for( uint8_t i = SD_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_SEND_OP_COND;
        }
        sd_commandEx( SD_APP_CMD, 0, 0, 0, 0, 0x65, retBuf ); // i.e. CMD55
        if ( !( retBuf[0] & SD_RET_IDLE ) || ( retBuf[0] & SD_RET_ZERO_START_BIT ) ) {
            return SD_APP_CMD;
        }
        if ( retBuf[0] & SD_RET_ILLEGAL_CMD ) {
            return _sd_legacy_cmd1_init( retBuf );
        }
        if ( retBuf[0] != SD_RET_IDLE ) {
            return SD_APP_CMD;
        }
        sd_commandEx( SD_APP_OP_COND, acmd41_arg0, 0, 0, 0, acmd41_crc, retBuf ); // i.e. ACMD41
        if ( retBuf[0] == SD_RET_READY ) {
            return SD_INIT_NO_ERROR;
        }
        if ( !( retBuf[0] & SD_RET_IDLE ) || ( retBuf[0] & SD_RET_ZERO_START_BIT ) ) {
            return SD_APP_OP_COND;
        }
        if ( retBuf[0] & SD_RET_ILLEGAL_CMD ) {
            return _sd_legacy_cmd1_init( retBuf );
        }
    }
}

#ifndef SD_NO_DEFAULT_BUFFER
uint8_t sd_retBuf[SD_RET_LEN_MAX];

uint8_t sd_command( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3 ) {
    return sd_commandCRC( cmd, arg0, arg1, arg2, arg3, 0x95 /* CMD 0 CRC */ );
}

uint8_t sd_commandCRC( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t crc ) {
    return sd_commandEx( cmd, arg0, arg1, arg2, arg3, crc, sd_retBuf );
}

uint8_t sd_init( void ) {
    return sd_initEx( sd_retBuf, true );
}
#endif
