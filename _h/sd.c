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

// @return  first byte of returned response, or SPI_EMPTY_BYTE (0xFF) if failed
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

    uint8_t r;
    bool allowBusyByte = false;
    switch( cmd ) {
        case SD_SEND_IF_COND: // CMD8 // R7
        case SD_READ_OCR: // CMD58 // R3
            r = 5;
            break;
        case SD_SEND_STATUS: // CMD13
        //case SD_STATUS: // ACMD13 // handled by case above
            r = 2; // R2
            break;
        case SD_STOP_TRANSMISSION: // CMD12
        case SD_SET_WRITE_PROT: // CMD28
        case SD_CLR_WRITE_PROT: // CMD29
        case SD_ERASE: // CMD38
            allowBusyByte = true;  // R1b
        default: // R1
            r = 1;
            break;
    }

    for( uint8_t i = SD_SPI_RESPONSE_WAIT_MAX; i > 0; i-- ) {
        retBuf[0] = spi_send( SPI_EMPTY_BYTE );
        if ( retBuf[0] == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( allowBusyByte ) { // implies r == 1
            retBuf[1] = spi_send( SPI_EMPTY_BYTE );
            if ( !retBuf[1] ) { // busy; don't wait, just return; caller should wait himself
                spi_deassert_SS();
                sd_debug_printf( "RET: busy\n\r" );
                return SPI_EMPTY_BYTE;
            } // else !busy - we already did the wait
        } else {
            for( uint8_t j = 1; j < r; j++ ) {
                retBuf[j] = spi_send( SPI_EMPTY_BYTE );
            }
        }
        spi_send( SPI_EMPTY_BYTE ); // req. 8 cycles wait = 1 SPI output byte
	    spi_deassert_SS();
        sd_debug_printf( "RET: 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx\n\r",
                      retBuf[0],
              r > 1 ? retBuf[1] : 0xFF,
              r > 2 ? retBuf[2] : 0xFF,
              r > 3 ? retBuf[3] : 0xFF,
              r > 4 ? retBuf[4] : 0xFF );
        return retBuf[0];
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: error: response timeout\n\r" );
    return SPI_EMPTY_BYTE;
}

uint8_t _sd_legacy_cmd1_init( uint8_t* retBuf ) {
    for( uint8_t i = SD_INIT_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_SEND_OP_COND;
        }
        sd_commandEx( SD_SEND_OP_COND, 0, 0, 0, 0, 0xF9, retBuf ); // i.e. CMD1
        if ( retBuf[0] == SD_RET_READY ) {
            return SD_INIT_NO_ERROR;
        }
        if ( retBuf[0] != SD_R1_IN_IDLE_STATE ) {
            return SD_SEND_OP_COND;
        }
    }
}

// @return  CMD which triggered error, or SPI_EMPTY_BYTE (0xFF) if no error
uint8_t sd_initEx( uint8_t* retBuf, bool useHcs ) {
    for( uint8_t i = SD_INIT_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_GO_IDLE_STATE;
        }
        sd_commandEx( SD_GO_IDLE_STATE, 0, 0, 0, 0, 0x95, retBuf ); // i.e. CMD0
        if ( retBuf[0] != SPI_EMPTY_BYTE ) {
            break; // accept strange but non-0xFF responses
        }
    }
    sd_commandEx( SD_SEND_IF_COND, 0, 0, SD_INIT_IF_COND_VOLTAGE, SD_INIT_IF_COND_ECHO, 0x87, retBuf ); // i.e. CMD8
    if ( !bit8_and( retBuf[0], SD_R1_IN_IDLE_STATE ) || bit8_and( retBuf[0], SD_R1_ZERO_START_BIT ) ) {
        return SD_SEND_IF_COND; // probably transmission error
    }
    if ( bit8_and( retBuf[0], SD_R1_ILLEGAL_CMD ) ) {
        return _sd_legacy_cmd1_init( retBuf );
    }
    if ( retBuf[0] != SD_R1_IN_IDLE_STATE || retBuf[1] != 0 || retBuf[2] != 0
      || retBuf[3] != SD_INIT_IF_COND_VOLTAGE || retBuf[4] != SD_INIT_IF_COND_ECHO ) {
        return SD_SEND_IF_COND; // other errors
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
    for( uint8_t i = SD_INIT_RETRIES_MAX; ; i-- ) {
        if ( i == 0 ) {
            return SD_SEND_OP_COND;
        }
        sd_commandEx( SD_APP_CMD, 0, 0, 0, 0, 0x65, retBuf ); // i.e. CMD55
        if ( !bit8_and( retBuf[0], SD_R1_IN_IDLE_STATE ) || bit8_and( retBuf[0], SD_R1_ZERO_START_BIT ) ) {
            return SD_APP_CMD;
        }
        if ( bit8_and( retBuf[0], SD_R1_ILLEGAL_CMD ) ) {
            return _sd_legacy_cmd1_init( retBuf );
        }
        if ( retBuf[0] != SD_R1_IN_IDLE_STATE ) {
            return SD_APP_CMD;
        }
        sd_commandEx( SD_APP_OP_COND, acmd41_arg0, 0, 0, 0, acmd41_crc, retBuf ); // i.e. ACMD41
        if ( retBuf[0] == SD_RET_READY ) {
            return SD_INIT_NO_ERROR;
        }
        if ( !bit8_and( retBuf[0], SD_R1_IN_IDLE_STATE ) || bit8_and( retBuf[0], SD_R1_ZERO_START_BIT ) ) {
            return SD_APP_OP_COND;
        }
        if ( bit8_and( retBuf[0], SD_R1_ILLEGAL_CMD ) ) {
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
