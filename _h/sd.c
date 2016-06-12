// SD card routines
#include "sd.h"

void sd_preinit( void ) {
    spi_assert_SS();
	for( uint8_t i = SD_INIT_IDLE_CYCLES; i > 0; i-- ) {
        sd_debug_printf( "~" );
		spi_recv();
    }
    sd_debug_printf( "|\n\r" );
    spi_deassert_SS();
}

// @return  first byte of returned response, or SPI_EMPTY_BYTE (0xFF) if failed
uint8_t sd_commandEx( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t crc, uint8_t* retBuf ) {
    sd_debug_verbose_printf( "CMD: %hhu ARG: 0x%02hhx %02hhx %02hhx %02hhx CRC: 0x%02hhx\n\r",
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
        retBuf[0] = spi_recv();
        if ( retBuf[0] == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( allowBusyByte ) { // implies r == 1
            retBuf[1] = spi_recv();
            if ( !retBuf[1] ) { // busy; don't wait, just return; caller should wait himself
                spi_deassert_SS();
                sd_debug_printf( "RET: busy\n\r" );
                return SPI_EMPTY_BYTE;
            } // else !busy - we already did the wait
        } else {
            for( uint8_t j = 1; j < r; j++ ) {
                retBuf[j] = spi_recv();
            }
        }
        spi_recv(); // req. 8 cycles wait = 1 SPI output byte
	    spi_deassert_SS();
        sd_debug_verbose_printf( "RET: 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx\n\r",
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
#ifdef SD_INIT_STEP_DELAY
        _delay_ms( SD_INIT_STEP_DELAY );
#endif
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
#ifdef SD_INIT_STEP_DELAY
        _delay_ms( SD_INIT_STEP_DELAY );
#endif
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
#ifdef SD_INIT_STEP_DELAY
        _delay_ms( SD_INIT_STEP_DELAY );
#endif
    }
}

// @return  response returned from command, or SPI_EMPTY_BYTE (0xFF) if failed
// note: this code assumes a regular 512-byte block since SDHC allows only this
// pass len == 0 to read all 512 bytes into buf
uint8_t sd_readSingleBlockHC( uint8_t addr3, uint8_t addr2, uint8_t addr1, uint8_t addr0, uint8_t* buf ) {
    uint8_t ret = sd_command( SD_READ_SINGLE_BLOCK, addr3, addr2, addr1, addr0 );
    if ( ret != SD_RET_READY ) {
        sd_debug_printf( "RET: not READY; [0x%02hhx] received\n\r", ret );
        return SPI_EMPTY_BYTE;
    }
    spi_assert_SS();
    for( uint8_t i = SD_SPI_RESPONSE_WAIT_MAX; i > 0; i-- ) {
        *buf = spi_recv();
        if ( *buf == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( *buf != SD_DATA_TOKEN ) {
            spi_deassert_SS();
            sd_debug_printf( "RET: invalid token received\n\r" );
            return SPI_EMPTY_BYTE;
        }
        _spi_recv_block( buf );
        // CRC values are ignored outside of debug mode
        uint8_t crc1 = spi_recv();
        uint8_t crc2 = spi_recv();
        sd_debug_verbose_printf( "CRC: 0x%02hhx %02hhx\n\r", crc1, crc2 );
        // do a CRC check if you're masochistic (or have a GHz-ish CPU)
        spi_recv(); // required 8 clock cooldown
        return ret;
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: busy\n\r" );
    return SPI_EMPTY_BYTE;
}

// note 1: extracted to function & hand-optimized because avr-gcc *duh*
// note 2: unroll to get even faster code:
//   256 loop itself takes 3 clocks [2 words], non-256 one takes 4 clocks [3 words],
//   inlined spi_recv() takes *at least* (no SPI wait) 5 clocks [5 words],
//   ASM_ST_INC is 2 clocks [1 word]; it follows that the loop takes (no SPI wait):
//   with 2x unrolling: (the default)
//     256 * [(5+2) *  2 + 3] = 4352 clocks; (5+1)*2+2  = 14 words
//   with 4x unrolling: (+6% for ~+25 bytes)
//     128 * [(5+2) *  4 + 4] = 4096 clocks; (5+1)*4+3  = 27 words
//   with 8x unrolling: (another +6% for ~+50 bytes)
//     64 *  [(5+2) *  8 + 4] = 3840 clocks; (5+1)*8+3  = 51 words
//   with 16x unrolling: (+3% for ~+100 bytes)
//     32 *  [(5+2) * 16 + 4] = 3712 clocks; (5+1)*16+3 = 99 words
//   further unrolling is subject to diminishing results, with maximum speed
//   at full unroll being only 3588 clocks, i.e. 3% faster than 16x unroll
//
//__attribute__ ((noinline))
void _spi_recv_block( uint8_t* buf ) {
    uint8_t j = 0;
#ifdef SD_DONT_UNROLL_RECV_BLOCK
    do {
        ASM_ST_INC( buf,  spi_recv() ); // *(buf++) = spi_recv();
        ASM_ST_INC( buf,  spi_recv() );
    } while ( ++j ); // 512 spi_recv() executions total
#else
#define SD_UNROLL_REPEAT8(x)  x; x; x; x; x; x; x; x
    do {
        SD_UNROLL_REPEAT8( ASM_ST_INC( buf,  spi_recv() ) );
    } while ( ++j < 64 ); // 512 spi_recv() executions total
#endif
}

// @return  response returned from command, or SPI_EMPTY_BYTE (0xFF) if failed
// note 1: this code assumes a regular 512-byte block since SDHC allows only this
// note 2: this function is slower than sd_readSingleBlockHC
uint8_t sd_readPartialBlockHC( uint8_t addr3, uint8_t addr2, uint8_t addr1, uint8_t addr0, uint8_t* buf, uint16_t offset, uint16_t len ) {
    uint8_t ret = sd_command( SD_READ_SINGLE_BLOCK, addr3, addr2, addr1, addr0 );
    if ( ret != SD_RET_READY ) {
        sd_debug_printf( "RET: not READY; [0x%02hhx] received\n\r", ret );
        return SPI_EMPTY_BYTE;
    }
    spi_assert_SS();
    for( uint8_t i = SD_SPI_RESPONSE_WAIT_MAX; i > 0; i-- ) {
        *buf = spi_recv();
        if ( *buf == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( *buf != SD_DATA_TOKEN ) {
            spi_deassert_SS();
            sd_debug_printf( "RET: invalid token received\n\r" );
            return SPI_EMPTY_BYTE;
        }
        uint16_t i = 0, end = offset + len;
        for( ; i < offset; i++ ) {
            spi_recv();
        }
        for( ; i < end; i++ ) {
            *(buf++) = spi_recv();
        }
        for( ; i < SD_HC_BLOCK_LENGTH; i++ ) {
            spi_recv();
        }
        uint8_t crc1 = spi_recv();
        uint8_t crc2 = spi_recv();
        sd_debug_verbose_printf( "CRC: 0x%02hhx %02hhx\n\r", crc1, crc2 );
        spi_recv(); // required 8 clock cooldown
        return ret;
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: busy\n\r" );
    return SPI_EMPTY_BYTE;
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
