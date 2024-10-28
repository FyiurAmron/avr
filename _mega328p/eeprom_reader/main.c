// EEPROM reader/writer
#define F_CPU  8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define COMPILE_SINGLE_FILE
#include "vax/misc.h"
#define b(byteno, n)  ( (uint8_t)( n >> ( 8 * byteno ) ) ) // FIXME
#include "vax/bits.h"

#define BAUD  9600
#include "vax/uart.h"

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SS    PB2
#define MOSI  PB3
#define MISO  PB4
#define SCK   PB5

// note: AT25xxx allows the Bit3 (4th from right) to be both 1 and 0
// although the cross-compatible standard is to pass only 0
#define EEPROM_READ   0b00000011 // Read Data from Memory Array
#define EEPROM_WRITE  0b00000010 // Write Data to Memory Array
#define EEPROM_WREN   0b00000110 // Set Write Enable Latch
#define EEPROM_WRDI   0b00000100 // Reset Write Enable Latch
#define EEPROM_RDSR   0b00000101 // Read Status Register
#define EEPROM_WRSR   0b00000001 // Write Status Register

// the below are not supported by AT25xxx, but are often found in the wild

#define EEPROM_PE     0b01000010  // Page   Erase
#define EEPROM_SE     0b11011000  // Sector Erase
#define EEPROM_CE     0b11000111  // Chip   Erase
#define EEPROM_DPD    0b10111001  // Deep Power-Down
#define EEPROM_RDID   0b10101011  // DPD Release

//

void spi_init_master( void ) { // spi_init_master
    // Set SS, MOSI and SCK are output for master
    SPI_DDR = BV(SS) | BV(MOSI) | BV(SCK);

    // Enable SPI, Master, set clock rate fck/16
    SPCR = BV(SPE) | BV(MSTR) | BV(SPR0);

    // not needed due to synchronous mode?? Clear SPI Interrupt Flag by reading SPSR and SPDR ??
}

inline void spi_wait( void ) {
    while( !(SPSR & BV(SPIF) ) ) { } // synchronous wait for EOT
}

inline void spi_cs( bool b ) {
    if ( b ) {
        cbi( SPI_PORT, SS ); // CS enable
    } else {
        sbi( SPI_PORT, SS ); // CS disable, prepare for enable
    }
}

void spi_putchar( uint8_t c ) {
    SPDR = c;
    spi_wait();
}

uint8_t spi_getchar( void ) {
    SPDR = 0x80; // arbitrary value, forces transfer
    spi_wait();
    return SPDR;
}

char eeprom_read( uint16_t addr ) {
    spi_cs( 1 );
    spi_putchar( EEPROM_READ );
    spi_putchar( b(1, addr) );
    spi_putchar( b(0, addr) );
    uint8_t Rx_data = spi_getchar();
    spi_cs( 0 );
    return Rx_data;
}

#define EEPROM_SIZE  1024
#define GRACE_PERIOD_SEC  5

int main( void ) {
    MCUSR = 0; wdt_disable();
    DDRB = 0; DDRC = 0; DDRD = 0;
    PORTB = 0; PORTC = 0; PORTD = 0;

    uart_init();
    uart_stdio();

    printf( "\n\r[*] UART device started: %ld %ld %d\n\r", UBRRH_VALUE, UBRRL_VALUE, USE_2X );

    printf( "\n\r[*] grace period (%ds): \n\r", GRACE_PERIOD_SEC );
    for( uint8_t i = 0; i < GRACE_PERIOD_SEC; i++ ) {
        _delay_ms( 1000 );
        printf( "." );
    }

    spi_init_master();

    printf( "\n\r[*] SPI initialized (uC is Master), not CS yet\n\r" );

    for(;;) {
        printf( "\n\r[*] waiting for confirmation - press any key: " );

        uint8_t c = getchar();

        printf( "\n\r[*] got %c [%d], OK\n\r", c, c );

        spi_cs( 1 );
        spi_putchar( EEPROM_READ );
        spi_putchar( 0 );
        spi_putchar( 0 );
        for (int i = 0; i < EEPROM_SIZE; i++) {
            uart_putchar( spi_getchar() );
        }
        spi_cs( 0 );
    }

} // main()
