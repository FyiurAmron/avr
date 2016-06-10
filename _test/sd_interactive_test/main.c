#define SD_DEBUG
#include "main.h"

int main(int argc, char *argv[]) {
	uart_init();
    uart_stdio();
    _delay_ms( 500 );
	printf( "\n\rinit\n\r" );
	spi_preinit();	
    spi_init( SPI_FREQ_DIV128 );

    uint8_t cmd, arg0, arg1, arg2, arg3;
	for(;;) {
	    scanf( "%hhu %hhx %hhx %hhx %hhx", &cmd, &arg0, &arg1, &arg2, &arg3 );
	    printf( "\n\rCMD: %02d %#02x %02x %02x %02x\n\r", cmd, arg0, arg1, arg2, arg3 );
	    sd_commandCRC( cmd, arg0, arg1, arg2, arg3, cmd == 8 ? 0x87 : 0x95 );
	    printf( "RET: "BYTE2BIN_FORMAT" %02x %02x %02x %02x\n\r\n\r",
                BYTE2BIN_ARG(sd_retBuf[0]), sd_retBuf[1], sd_retBuf[2], sd_retBuf[3], sd_retBuf[4] );
	}	
}
