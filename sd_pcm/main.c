//
#include "main.h"

volatile uint8_t tick = 0;
volatile uint16_t pos = 0;
volatile uint8_t *pwmBlockCur, *pwmBlockNext;
volatile bool pwmNeedUpdate = false;
#define SIZE  512

ISR( TIMER0_OVF_vect ) { // overflow, 1/256 clocks i.e. for 20 MHz CPU f_OVF = 78125 Hz
    tick++;
    if ( tick != 2 ) {
        return;
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

void pwm_init( void ) {
  bit8_set( DDRD, BV(PD5) ); // set OC1A pin as output

  bit8_set( TCCR1A, BV(COM1A1) | BV(WGM10) );
  bit8_set( TCCR1B, BV(WGM12) | BV(CS10) );
   
  bit8_set( TCCR0B, BV(CS00) );  // enable timer 0
  bit8_set( TIMSK0, BV(TOIE0) ); // enable timer 0 interrupt
  
  OCR1A = 0x80;
}

void pwm_disable( void ) {
    cli();

    OCR1A = 0x80;

    TCCR0A = 0;
    TCCR0B = 0;
    TCCR1A = 0;
    TCCR1B = 0;
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
    //printf( "* PWM: OK\n\r" );

    bit8_set( xPORT(B), BV(PB0) );

    uint8_t block1[SIZE] = {0}, block2[SIZE] = {0};
    for(;;) {
      init:
        //printf( "press any key to continue... " );
        //getchar();
        //_delay_ms(100);
        while( !bit8_and( xPIN(B), BV(PB0) ) ) {}
        printf( "press START (PB0) to continue" );
        while( bit8_and( xPIN(B), BV(PB0) ) ) {
            putchar( '.' );
            _delay_ms(500);
        }
        _delay_ms(10);
        printf( "\n\r* SPI: " );
        spi_preinit();
        spi_init( SPI_FREQ_DIV128 );
        printf( "OK\n\r" );

        printf( "* SD: " );
        sd_preinit();
        uint8_t sd_init_ret = sd_init();
        printf( sd_init_ret == SD_INIT_NO_ERROR ? "OK" : "failed" );
        printf( " [%hhu] \n\r\n\r", sd_init_ret );

        // MBR is @ addr 0
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block1, MBR_SIZE - MBR_EEM_SIZE - 4 * MBR_PE_SIZE, MBR_PE_SIZE );
/*
        sd_readPartialBlockHC( 0, 0, 0, 0,
            block1 + MBR_PE_SIZE, MBR_SIZE - MBR_EEM_SIZE, MBR_EEM_SIZE );
*/
        //debug_hexDump( block1, 512 ); // dump MBR
        fs_PartitionEntry* pe = (fs_PartitionEntry*) block1;
        //fs_BootSectorEnd* eem = (fs_BootSectorEnd*) (block1 + MBR_PE_SIZE);
        //printf( "Type code: 0x%02hhx\n\r", pe->typeCode );
        //printf( "End marker: 0x%02hhx %02hhx\n\r", eem->byte1, eem->byte2 );
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
            //uint8_t rootBlock[512];
            sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr), block2 );
            //debug_hexDump( rootBlock, 512 ); // dump root dir
            fat_FileEntry* fe = (fat_FileEntry*) block2;
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
                //debug_hexDump( block1, 512 );

                spi_init( SPI_FREQ_DIV2 );
                for(;;) {
                    uint32_t addr3 = addr2;
                    addr3++;
                    sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block2 );
                    pwmBlockCur = block1;
                    pwmBlockNext = block2;
                    pos = 0x2C; // skip entire header
                    pwm_init();
                    sei();
                    for( uint16_t sec2 = (13 * secPerClust - 1) / 2; sec2 > 0; sec2-- ) {
                        while ( !pwmNeedUpdate ) {
                            if ( !bit8_and( xPIN(B), BV(PB0) ) ) {
                                //_delay_ms(1000);
                                goto restart;
                            }
                        }
                        addr3++;
                        pwmBlockNext = block1; // note: ptr swap done *before* read to reduce delay
                        if ( sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block1 ) == SD_COMMAND_ERROR ) {
                            goto restart;
                        }
                        //pwmBlockNext = block1;
                        pwmNeedUpdate = false;
                        while ( !pwmNeedUpdate ) {
                            if ( !bit8_and( xPIN(B), BV(PB0) ) ) {
                                //_delay_ms(1000);
                                goto restart;
                            }
                        }
                        addr3++;
                        pwmBlockNext = block2; // note: ptr swap done *before* read to reduce delay
                        if ( sd_readSingleBlockHC( UINT32_TO_UINT8x4(addr3), block2 ) == SD_COMMAND_ERROR ) {
                            goto restart;
                        }
                        //pwmBlockNext = block2;
                        pwmNeedUpdate = false;
                    }
                }
                cli();
                printf(" : PWM ended.\n\r");
            }
        }
      endDirScan:
	    while(1) {
/*
            uint8_t arg0, arg1, arg2, arg3;
	        scanf( "%hhx %hhx %hhx %hhx", &arg0, &arg1, &arg2, &arg3 );
            printf( "\n\r" );
            sd_readSingleBlockHC( arg0, arg1, arg2, arg3, block1 );
            debug_hexDump( block1, 512 );
*/
	    }	
      restart:
        pwm_disable();
        goto init;
    } // while(1)
} // main()
