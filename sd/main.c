//
#include "main.h"

// sd.c /.h

// @return  response returned from command, or SPI_EMPTY_BYTE (0xFF) if failed
// note 1: this code assumes a regular 512-byte block since SDHC allows only this
// note 2: this function is somewhat slower than sd_readSingleBlockHC
uint8_t sd_readPartialBlockHC( uint8_t addr3, uint8_t addr2, uint8_t addr1, uint8_t addr0, uint8_t* buf, uint16_t offset, uint16_t len ) {
    uint8_t ret = sd_command( SD_READ_SINGLE_BLOCK, addr3, addr2, addr1, addr0 );
    if ( ret != SD_RET_READY ) {
        sd_debug_printf( "RET: not READY; [0x%02hhx] received\n\r", ret );
        return SPI_EMPTY_BYTE;
    }
    spi_assert_SS();
    for( uint8_t i = SD_SPI_RESPONSE_WAIT_MAX; i > 0; i-- ) {
        buf[0] = spi_send( SPI_EMPTY_BYTE );
        if ( buf[0] == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( buf[0] != SD_DATA_TOKEN ) {
            spi_deassert_SS();
            sd_debug_printf( "RET: invalid token received\n\r" );
            return SPI_EMPTY_BYTE;
        }
        uint16_t i = 0, end = offset + len;
        for( ; i < offset; i++ ) {
            spi_send( SPI_EMPTY_BYTE );
        }
        for( ; i < end; i++ ) {
            *(buf++) = spi_send( SPI_EMPTY_BYTE );
        }
        for( ; i < SD_HC_BLOCK_LENGTH; i++ ) {
                spi_send( SPI_EMPTY_BYTE );
        }
        uint8_t crc1 = spi_send( SPI_EMPTY_BYTE );
        uint8_t crc2 = spi_send( SPI_EMPTY_BYTE );
        sd_debug_printf( "CRC: 0x%02hhx %02hhx\n\r", crc1, crc2 );
        spi_send( SPI_EMPTY_BYTE ); // required 8 clock cooldown
        return ret;
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: busy\n\r" );
    return SPI_EMPTY_BYTE;
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
        buf[0] = spi_send( SPI_EMPTY_BYTE );
        if ( buf[0] == SPI_EMPTY_BYTE  ) {
            continue;
        }
        if ( buf[0] != SD_DATA_TOKEN ) {
            spi_deassert_SS();
            sd_debug_printf( "RET: invalid token received\n\r" );
            return SPI_EMPTY_BYTE;
        }
        for( uint8_t i = 0; i != 255; i++ ) {
            buf[i] = spi_send( SPI_EMPTY_BYTE );
        }
        buf += 255;
        for( uint8_t i = 0; i != 255; i++ ) {
            buf[i] = spi_send( SPI_EMPTY_BYTE );
        }
        buf += 255;
        buf[0] = spi_send( SPI_EMPTY_BYTE );
        buf[1] = spi_send( SPI_EMPTY_BYTE );

        uint8_t crc1 = spi_send( SPI_EMPTY_BYTE );
        uint8_t crc2 = spi_send( SPI_EMPTY_BYTE );
        sd_debug_printf( "CRC: 0x%02hhx %02hhx\n\r", crc1, crc2 );
        spi_send( SPI_EMPTY_BYTE ); // required 8 clock cooldown
        return ret;
    }
    spi_deassert_SS();
    sd_debug_printf( "RET: busy\n\r" );
    return SPI_EMPTY_BYTE;
}

int main( void ) {
    init();
    //while(1) {} // to quickly disable uC code
    _delay_ms(100);

    uart_init();
    uart_stdio();

    printf( "\n\r*** Device initializing:\n\r" );
    printf( "\n\r" );
    printf( "* UART: OK\n\r" );
    for(;;) {
        printf( "press any key to continue...\n\r" );
        getchar();
        printf( "* SPI: " );
        spi_preinit();
        spi_init( SPI_FREQ_DIV2 );
        printf( "OK\n\r" );

        printf( "* SD: " );
        sd_preinit();
        uint8_t sd_init_ret = sd_init();
        printf( sd_init_ret == SD_INIT_NO_ERROR ? "OK" : "failed");
        printf( " [%hhu] \n\r\n\r", sd_init_ret );

        uint8_t block[512];
        // MBR is @ addr 0
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block, MBR_SIZE - MBR_EEM_SIZE - 4 * MBR_PE_SIZE, MBR_PE_SIZE );
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block + MBR_PE_SIZE, MBR_SIZE - MBR_EEM_SIZE, MBR_EEM_SIZE );
        //debug_hexDump( block, 512 ); // dump MBR
        fs_PartitionEntry* pe = (fs_PartitionEntry*) block;
        fs_BootSectorEnd* eem = (fs_BootSectorEnd*) (block + MBR_PE_SIZE);
        printf( "Type code: 0x%02hhx\n\r", pe->typeCode );
        printf( "End marker: 0x%02hhx %02hhx\n\r", eem->byte1, eem->byte2 );
        printf( "FAT Boot Sector:\n\r" );
        uint32_t baseAddr = UINT8x4_TO_UINT32( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3 );
        sd_readSingleBlockHC( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3, block );
        fat_32* f32 = (fat_32*) block;
        printf( "volLabel: %.11s\n\r", f32->volLabel );
        printf( "fsName: %.8s\n\r", f32->fsName );
        //debug_hexDump( block, 512 ); // dump FAT boot sector
        uint8_t secPerClust = f32->secPerClust;
        baseAddr += UINT8x2LE_TO_UINT16( f32->reservedSecCnt );
        baseAddr += f32->numFATs * UINT8x4LE_TO_UINT32( f32->fatSize32 );
        uint32_t addr = baseAddr;
        // addr == root dir ATM
        for( uint8_t sec = secPerClust; sec > 0; sec--, addr++ ) {
            uint8_t rootBlock[512];
            sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr), rootBlock );
            //debug_hexDump( rootBlock, 512 ); // dump root dir
            fat_FileEntry* fe = (fat_FileEntry*) rootBlock;
            uint32_t addr2;
            for( uint8_t i = SD_HC_BLOCK_LENGTH / sizeof(fat_FileEntry); i > 0; i--, fe++ ) {
                if ( fe->name[0] == 0x00 ) {
                    goto endDirScan;
                }
                if ( fe->name[0] < FAT_FILE_ENTRY_MIN_NORMAL_VALUE
                  || fe->name[0] == FAT_FILE_ENTRY_DELETED
                  || fe->attrBits & ( FAT_ATTR_VOLUME_ID | FAT_ATTR_DIRECTORY ) ) {
                    continue;
                }
                printf( "file: [%.8s.%.3s]\n\r", fe->name, fe->ext );
                uint32_t clust = ( UINT8x4_TO_UINT32(
                  fe->clustHigh[1], fe->clustHigh[0], fe->clustLow[1], fe->clustLow[0] )
                  - FAT_CLUSTER_OFFSET );
                printf( "clust: 0x%8lx (+2)\n\r", clust );
                addr2 = baseAddr + secPerClust * clust;
                printf( "addr2: 0x%8lx\n\r1st sector:\n\r", addr2 );
                sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr2), block );
                debug_hexDump( block, 512 );
            }
        }

        uint8_t arg0, arg1, arg2, arg3;
      endDirScan:
	    while(1) {
	        scanf( "%hhx %hhx %hhx %hhx", &arg0, &arg1, &arg2, &arg3 );
            printf( "\n\r" );
            sd_readSingleBlockHC( arg0, arg1, arg2, arg3, block );
            debug_hexDump( block, 512 );
	    }	
        
    } // while(1)
} // main()
