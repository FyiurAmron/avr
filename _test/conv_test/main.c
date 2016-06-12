#define F_CPU  8000000UL
//#define F_CPU 1000000UL
#define COMPILE_SINGLE_FILE
#include "vax/misc.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "vax/uart.h"

uint8_t z1(void) { return 0x20; }
uint8_t z2(void) { return 0x60; }
uint8_t z3(void) { return 0xA0; }
uint8_t z4(void) { return 0xE0; }

void test( void ) {
    uint8_t y1 = z1();
    volatile uint8_t y2 = y1 << 3;
    uint8_t y3 = z3();
    y3 <<= 5;
    printf( "%d %d %d %d\n\r", sizeof(y1 << 3), sizeof( y3 <<= 5 ), sizeof( y3 ^ 0b101 ), sizeof ( ~y3) );
    do {
        y2++;
    } while ( !(uint8_t)(y2 << 6) );
    printf( "%d\n\r", y2 );
}

int main( void ) {
    init();

    uart_init();
    uart_stdio();

    printf( "\n\rDevice started.\n\r" );

    //uint8_t a = z1(), b = z2(), c = z3(), d = z4();

    //printf( "0x%lX 0x%X 0x%X 0x%X\n\r", (uint32_t)(a<<24), (a<<24), (a<<24)|(b<<16)|(c<<8)|d, (c<<8)|d );
    //printf( "%d %d %d %d\n\r", sizeof((uint32_t)(a<<24)), sizeof(a<<24), sizeof((a<<24)|(b<<16)|(c<<8)|d), sizeof((c<<8)|d) );
    //printf( "0x%lX\n\r", ((((((uint32_t)(a)<<8)|b)<<8)|c)<<8)|d );
    //printf( "\x7F""Ü""\x7F""ÜÜ\n\r" );
    //printf( "%d\n\r", sizeof((uint32_t)(a)<<8 );
/*
    ASM_ADD(d,d);
    ASM_ADC(c,c);
    ASM_ADC(b,b);
    ASM_ADC(a,a);

    printf( "0x%lX\n\r", ((((((uint32_t)(a)<<8)|b)<<8)|c)<<8)|d );
*/
    test();
    while(1) {
        //_delay_ms(1000);
    }
} // main()
