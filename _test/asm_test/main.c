typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

extern void foo( uint8_t z );
extern void foo2();
extern uint8_t spi_send( uint8_t z );
extern uint8_t spi_recv( void );

#define SPI_EMPTY_BYTE 0xFF

#include "vax/asm.h"

__attribute__ ((noinline))
void loop2i( uint8_t* buf) {
        uint8_t i = 0;
        do {
            ASM_ST_INC( buf, spi_recv() );
            ASM_ST_INC( buf, spi_recv() );
        } while ( ++i );
}

__attribute__ ((noinline))
void loop2j( uint8_t* buf) {
uint8_t* buf2 = buf;
        uint8_t i = 0;
        do {
 *buf2 = spi_recv();
buf2++;
 *buf2 = spi_recv();
buf2++;
        } while ( ++i );
}

void loop3(void) {
   uint8_t z1 = 0x20, z2 = 0xc0, y1 = 0x10, y2 = 0x88;
   ASM_ADD( z2, y2 );
   ASM_ADC( z1, y1 );
   foo(z1);
}

extern void proc( uint8_t* buf );
void proc( uint8_t* buf );

int main( void ) {
uint8_t buf[512];
    loop2i( buf );
    loop2j( buf );
    proc( buf );
for(;;) {}
}