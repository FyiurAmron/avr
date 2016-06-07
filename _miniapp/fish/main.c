//#define F_CPU  1000000UL
//#define F_CPU  8000000UL
#include "vax/cpu.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>

#define BAUD  9600
#include <util/setbaud.h>
//#undef USE2X
// FIXME UART errors?
#include "vax/uart.h"

#include "vax/misc.h"

#define LCD_DATA_LINE  A /*C*/
#define LCD_CTRL_LINE  D

#define LCD_RS  (1<< 6)
#define LCD_RW  (1<< 5)
#define LCD_E   (1<< 4)

#define LCD_4BIT
#include "vax/lcd_hd44780.h"

static const uint8_t charTri[] = {
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b11111,
    0b00100,
    0b01110,
    0b10101,
};

static const uint8_t charTriX2[] = {
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b11111,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b11111,
    0b00100,
    0b01110,
    0b10101,
};

static const uint8_t charFish[] = {
    0b10101,
    0b01110,
    0b11101,
    0b11111,
    0b11111,
    0b01110,
    0b11111,
    0b11011,
};

static const uint8_t charFishX2[] = {
    0b10101,
    0b01110,
    0b11101,
    0b11111,
    0b11111,
    0b01110,
    0b11111,
    0b11011,
    0b10101,
    0b01110,
    0b11101,
    0b11111,
    0b11111,
    0b01110,
    0b11111,
    0b11011,
};

static const uint8_t charEmpty[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
};

int main( void ) {
    init();
    power_adc_disable();
    //etc.

    uart_init();
    uart_as_stdio();

    // LCD control
    LCD_preinit();
    _delay_ms(50);
    LCD_init4bit();
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );    
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLINK_OFF );
    LCD_setEntryMode( LCD_CMD_EM_SHIFT_CURSOR, LCD_CMD_EM_INCREMENT );
    LCD_clear();

    puts("\n\rDevice started...\r");

    for( uint8_t i = 0; i < 8; i++ ) {
        LCD_putchar( i );
    }
    LCD_gotoLine2();
    for( uint8_t i = 0; i < 8; i++ ) {
        LCD_putchar( i );
    }

    const uint8_t * charPtr   = charFishX2;
    const uint8_t * charLimit = charPtr + 8;

    while(1) {
        for( uint8_t i = 0; i < 8; i++ ) {
            LCD_setCharacter( i, charPtr++ );
            if ( charPtr >= charLimit ) {
                charPtr -= 8;
            }
        }
        charPtr++;
        //uart_putchar('*');
        //while ( getKeyPressed() == NO_KEY_PRESSED ) {}
        _delay_ms(50);
    }
} // main()
