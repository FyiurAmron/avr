//
#include "main.h"

volatile uint8_t tick = 0;
volatile uint16_t pos = 0;
volatile uint8_t *pwmBlockCur, *pwmBlockNext;
volatile bool pwmNeedUpdate = false;
#define SIZE  512
volatile uint8_t block1[SIZE], block2[SIZE];

ISR( TIMER0_OVF_vect ) { // overflow, 1/256 clocks i.e. for 20 MHz CPU f_OVF = 78125 Hz
    tick++;
    if ( tick != 2 ) {
        continue;
    }
    tick = 0;
    OCR1A = pwmBlockCur[pos];
    if ( pos == SIZE - 1 ) {
        pos = 0;
        pwmNeedUpdate = true;
        pwmBlockCur = pwmBlockNext;
    } else {
        pos++;
    }
}

void pwm_init(void) {
  DDRD = _BV(PD5); // set OC1A pin as output

  TCCR1A = _BV(COM1A1) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
   
  TCCR0B |= _BV(CS00);  // enable timer 0
  TIMSK0 |= _BV(TOIE0); // enable timer 0 interrupt
  
  OCR1A = 0x80;
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
printf( "* PWM: OK\n\r" );
    for(;;) {
        printf( "press any key to continue...\n\r" );
        getchar();
        printf( "* SPI: " );
        spi_preinit();
        spi_init( SPI_FREQ_DIV128 );
        printf( "OK\n\r" );

        printf( "* SD: " );
        sd_preinit();
        uint8_t sd_init_ret = sd_init();
        printf( sd_init_ret == SD_INIT_NO_ERROR ? "OK" : "failed");
        printf( " [%hhu] \n\r\n\r", sd_init_ret );

        // MBR is @ addr 0
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block1, MBR_SIZE - MBR_EEM_SIZE - 4 * MBR_PE_SIZE, MBR_PE_SIZE );
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block1 + MBR_PE_SIZE, MBR_SIZE - MBR_EEM_SIZE, MBR_EEM_SIZE );
        //debug_hexDump( block1, 512 ); // dump MBR
        fs_PartitionEntry* pe = (fs_PartitionEntry*) block1;
        fs_BootSectorEnd* eem = (fs_BootSectorEnd*) (block1 + MBR_PE_SIZE);
        printf( "Type code: 0x%02hhx\n\r", pe->typeCode );
        printf( "End marker: 0x%02hhx %02hhx\n\r", eem->byte1, eem->byte2 );
        printf( "FAT Boot Sector:\n\r" );
        uint32_t baseAddr = UINT8x4_TO_UINT32( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3 );
        sd_readSingleBlockHC( pe->lbaStart0, pe->lbaStart1, pe->lbaStart2, pe->lbaStart3, block1 );
        fat_32* f32 = (fat_32*) block1;
        printf( "volLabel: %.11s\n\r", f32->volLabel );
        printf( "fsName: %.8s\n\r", f32->fsName );
        //debug_hexDump( block1, 512 ); // dump FAT boot sector
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
                sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr2), block1 );
                if ( block1[0] != 'R' || block1[1] != 'I' || block1[2] != 'F' || block1[3] != 'F' ) {
                    continue;
                }
                debug_hexDump( block1, 512 );

                spi_init( SPI_FREQ_DIV2 );
                for(;;) {
                    uint32_t addr3 = addr2;
                    addr3++;
                    sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block1 );
                    addr3++;
                    sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block2 );
                    pwmBlockCur = block1;
                    pwmBlockNext = block2;
                    pwm_init();
                    sei();
                    //block1 += 0x2C; // skip entire header
                    // NOT: start async play from block1
                    // INSTEAD: skip entire block1 (assume it's silent)
                   for( uint16_t sec2 = (13 * secPerClust - 1) / 2; sec2 > 0; sec2-- ) {
                        while ( !pwmNeedUpdate ) {}
                        addr3++;
                        sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block1 );
                        pwmBlockNext = block1;
                        pwmNeedUpdate = false;
                        while ( !pwmNeedUpdate ) {}
                        addr3++;
                        sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block2 );
                        pwmBlockNext = block2;
                        pwmNeedUpdate = false;
                    }
                }
                cli();
                printf(" : PWM ended.\n\r");
            }
        }
        uint8_t arg0, arg1, arg2, arg3;
      endDirScan:
	    while(1) {
/*
	        scanf( "%hhx %hhx %hhx %hhx", &arg0, &arg1, &arg2, &arg3 );
            printf( "\n\r" );
            sd_readSingleBlockHC( arg0, arg1, arg2, arg3, block1 );
            debug_hexDump( block1, 512 );
*/
	    }	
        
    } // while(1)
} // main()
