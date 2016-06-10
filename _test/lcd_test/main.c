#define F_CPU  1000000UL
//#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>

#include "vax/bits.h"
#include "vax/misc.h"

#define KEYPAD C /*A*/
#include "vax/keypad.h"

#include "vax/uart.h"

#define LCD_DATA  A /*C*/
#define LCD_CTRL  D

#define LCD_RS  BV(6)
#define LCD_RW  BV(5)
#define LCD_E   BV(4)

#include "vax/lcd_hd44780.h"

int main( void ) {
    init();
    power_adc_disable();
    //etc.
    //while(1) {} // to quickly disable uC code

    LCD_init();
    uart_init();
    uart_as_stdio();

    puts("\n\rDevice started...\r");

    // LCD control
    _delay_ms(50);
    //LCD_initalize();
    //LCD_Initalize();
    LCD_print( "test1" );
    //LCD_print("test1");
    //printf( "%x ", LCD_ReadStatus() );
    printf( "%x ", LCD_getAddressDDRAM() );

/*
#define CODE  0
for( uint8_t i = 0; i < 8; i++ ) {
    LCD_setAddressCGRAM( (CODE << 3) | i );
    LCD_writeChar( 0b10101 );
}
    LCD_setAddressDDRAM( 0 );
    LCD_writeChar( CODE );
#define CODE  1
for( uint8_t i = 0; i < 8; i++ ) {
    LCD_setAddressCGRAM( (CODE << 3) | i );
    LCD_write( i % 2 ? 0b10101 : 0b01010 );
}
    LCD_setAddressDDRAM( 1 );
    LCD_writeChar( CODE );
#define CODE  2
for( uint8_t i = 0; i < 8; i++ ) {
    LCD_setAddressCGRAM( (CODE << 3) | i );
    LCD_writeChar( i % 2 ? 0b11111 : 0 );
}
    LCD_setAddressDDRAM( 2 );
    LCD_writeChar( CODE );
*/

    while(1) {
    } // while(1)
} // main()
