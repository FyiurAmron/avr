//
#include "main.h"

#define SD_HC_BLOCK_LENGTH  512
// mbr.h
typedef struct {
    uint8_t bootFlag;
    uint8_t chsBegin[3];
    uint8_t typeCode;
    uint8_t chsEnd[3];
    uint8_t lbaStart3; // little-endian
    uint8_t lbaStart2;
    uint8_t lbaStart1;
    uint8_t lbaStart0;
    uint8_t sectorCnt[4]; // uint32_t, little-endian
} fs_PartitionEntry;

typedef struct {
    uint8_t byte1; // 0x55
    uint8_t byte2; // 0xAA
} fs_BootSectorEnd;

typedef struct {
    uint8_t bootstrap[446];
    fs_PartitionEntry partitionEntry[4];
    fs_BootSectorEnd bootSectorEnd; // 0x55 0xAA
} fs_MBR;

#define MBR_SIZE      sizeof(fs_MBR)
#define MBR_PE_SIZE   sizeof(fs_PartitionEntry)
#define MBR_EEM_SIZE  sizeof(fs_BootSectorEnd)

// fat.h

typedef struct { // all uint8_t[2] == uint16_t && uint8_t[4] == uint32_t are little-endian
    uint8_t jmpBoot[3];
    uint8_t oemName[8];
    uint8_t bytesPerSec[2]; // uint16_t
    uint8_t secPerClust;
    uint8_t reservedSecCnt[2]; // uint16_t
    uint8_t numFATs;
    uint8_t rootEntryCnt[2]; // uint16_t
    uint8_t totalSec16[2]; // uint16_t
    uint8_t media;
    uint8_t fatSize16[2]; // uint16_t
    uint8_t secPerTrack[2]; // uint16_t
    uint8_t numHeads[2]; // uint16_t
    uint8_t hiddenSecCnt[4]; // uint32_t
    uint8_t totalSec32[4]; // uint32_t
// generic FAT12/16/16B/32 part ends here; 36 bytes
// FAT32 specific:
    uint8_t fatSize32[4]; // uint32_t
    uint8_t extFlags[2]; // uint16_t
    uint8_t fsVer[2]; // uint16_t
    uint8_t rootClust[4]; // uint32_t
    uint8_t fsInfo[2]; // uint16_t
    uint8_t bkBootSec[2]; // uint16_t
    uint8_t reserved0[12];
    uint8_t drvNum;
    uint8_t reserved1;
    uint8_t bootSig;
    uint8_t volID[4]; // uint32_t
    uint8_t volLabel[11]; // char[11]
    uint8_t fsName[8]; // char[8]
// FAT32 header ends here; 90 bytes
// uint8_t filler[420];
// fs_BootSectorEnd bootSectorEnd; // 0x55 0xAA
} fat_32;

typedef struct {
   uint8_t name[8]; // char[8]
   uint8_t ext[3]; // char[3]
   uint8_t attrBits;
   uint8_t unknown1[8];
   uint8_t clustHigh[2]; // uint16_t
   uint8_t unknown2[4];
   uint8_t clustLow[2]; // uint16_t
   uint8_t size[4]; // uint32_t
} fat_FileEntry;

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

#define INT32_TO_4VAL(x,y)  (y)(x >> 24), (y)(x >> 16), (y)(x >> 8), (y)x
#define INT32_TO_4UINT8(x)  INT32_TO_4VAL(x,uint8_t)

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

// misc.c /.h

#define HEX_DUMP_LINE_LENGTH  16

void hexDump( uint8_t* data, uint16_t len ) {
    uint8_t* ptr = data, *limitPtr = data + len;
    len = 0; // reuse
    for( ; ptr < limitPtr; ptr += HEX_DUMP_LINE_LENGTH, len += HEX_DUMP_LINE_LENGTH ) {
        printf( " 0x%04hx |", len );
        for( uint8_t j = 0; j < HEX_DUMP_LINE_LENGTH; j++ ) {
            printf( " %02hhx", ptr[j] );
        }
        printf( " | " );
        for( uint8_t j = 0; j < HEX_DUMP_LINE_LENGTH; j++ ) {
            printf( "%c", ( ptr[j] < 0x20 || ptr[j] > 0x7E ) ? '.' : ptr[j] );
        }
        printf( "\n\r" );
    }
}

uint16_t uint16_from_uint8( uint8_t msb, uint8_t lsb ) {
    uint32_t ret = msb;
    ret <<= 8;
    ret |= lsb;
    return ret;
}

uint16_t uint16_fromLittleEndian( uint8_t* leBytes ) {
    return uint16_from_uint8( leBytes[1], leBytes[0] );
}

uint32_t uint32_from_uint8( uint8_t msb, uint8_t b1, uint8_t b2, uint8_t lsb ) {
    uint32_t ret = msb;
    ret <<= 8;
    ret |= b1;
    ret <<= 8;
    ret |= b2;
    ret <<= 8;
    ret |= lsb;
    return ret;
}

uint32_t uint32_fromLittleEndian( uint8_t* leBytes ) {
    return uint32_from_uint8( leBytes[3], leBytes[2], leBytes[1], leBytes[0] );
}
uint8_t dat = 2;
int main( void ) {
    init();
    //while(1) {} // to quickly disable uC code
    _delay_ms(100);

    uart_init();
    uart_stdio();

    printf( "\n\r*** Device initializing:\n\r" );
    printf( "\n\r" );
    printf( "* UART: OK\n\r" );
    while(dat==2) {
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

        fs_PartitionEntry* pe = (fs_PartitionEntry*) block;
        fs_BootSectorEnd* eem = (fs_BootSectorEnd*) (block + MBR_PE_SIZE);
        printf( "Type code: 0x%02hhx\n\r", pe->typeCode );
        printf( "End marker: 0x%02hhx %02hhx\n\r", eem->byte1, eem->byte2 );
        printf( "FAT Boot Sector:\n\r" );
#define FAT_CLUSTER_OFFSET  2
        uint32_t addr = uint32_from_uint8( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3 );
        sd_readSingleBlockHC( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3, block );
        fat_32* f32 = (fat_32*) block;
        printf( "volLabel: %11s\n\r", f32->volLabel );
        //hexDump( block, 512 );
        addr += uint16_fromLittleEndian( f32->reservedSecCnt );
        addr += f32->numFATs * uint32_fromLittleEndian( f32->fatSize32 );
        // addr == root dir ATM
        sd_readSingleBlockHC( INT32_TO_4UINT8(addr), block );
        //hexDump( block, 512 );
        fat_FileEntry* fe = (fat_FileEntry*) block;
        uint32_t addr2;

        for( uint8_t i = 4 /*SD_HC_BLOCK_LENGTH / sizeof(fat_FileEntry)*/; i > 0; i--, fe++ ) {
            if ( fe->name[0] != 0xE5 && fe->name[0] != 0x00 ) {
                printf( "file: [%8s.%3s]\n\r1st sector:\n\r", fe->name, fe->ext );
                addr2 = addr + f32->secPerClust * ( uint32_from_uint8(
                  fe->clustHigh[1], fe->clustHigh[0], fe->clustLow[1], fe->clustLow[0] )
                  - FAT_CLUSTER_OFFSET );
                sd_readSingleBlockHC( INT32_TO_4UINT8(addr2), block );
                hexDump( block, 512 );
            }
        }

        uint8_t arg0, arg1, arg2, arg3;
	    while(1) {
	        scanf( "%hhx %hhx %hhx %hhx", &arg0, &arg1, &arg2, &arg3 );
            printf( "\n\r" );
            sd_readSingleBlockHC( arg0, arg1, arg2, arg3, block );
            hexDump( block, 512 );
	    }	
        
    } // while(1)
} // main()
