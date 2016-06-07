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

#define BAUD  9600
#include "vax/uart.h"

#define LCD_DATA  A /*C*/
#define LCD_CTRL  D

#define LCD_RS  (1<< 6)
#define LCD_RW  (1<< 5)
#define LCD_E   (1<< 4)

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
    xDDR(B) |= (1<< 0 ); // keypad status
#ifdef MEASURE_DIST
    xDDR(B) |= (1<< 1 );
    xDDR(B) &=~(1<< 2 );
    xPORT(B) &=~(1<< 1 );
#endif
    PORTB = 0;
    //while(1) {} // to quickly disable uC code

    uart_init();
    uart_as_stdio();

    // LCD control
    LCD_init();
    _delay_ms(50);
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_ON, LCD_CMD_CURSOR_BLINK_ON );
    LCD_setEntryMode( LCD_CMD_EM_SHIFT_CURSOR, LCD_CMD_EM_INCREMENT );
    LCD_clear();

    puts("\n\rDevice started...\r");

    for( uint8_t i = 0; i < 8; i++ ) {
        LCD_writeChar( i );
    }
    LCD_gotoLine2();
    for( uint8_t i = 0; i < 8; i++ ) {
        LCD_writeChar( i );
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
        uart_putchar('*');
        //while ( getKeyPressed() == NO_KEY_PRESSED ) {}
        _delay_ms(50);
    }

#ifdef MEASURE_DIST
    int32_t echoTime = -1;
#endif
    uint8_t keyNum = NO_KEY_PRESSED, keyNumLast = 0xFE;

    while(1) {
        keyNum = getKeyPressed();
if ( keyNum != NO_KEY_PRESSED ) {
        //            LCD_writeChar( KEYCODES[keyNum] );
                     uart_putchar( KEYPAD_KEYCODES3[keyNum] );
if ( KEYPAD_KEYCODES3[keyNum] == '*' ) {
    LCD_shiftCursor(1);
} else if ( KEYPAD_KEYCODES3[keyNum] == '#' ) {
    LCD_shiftCursor(-1);
}
        _delay_ms(300);
}
continue;
#ifdef MEASURE_DIST
        if ( xPIN(B) & (1<< 2 ) ) {
             if ( echoTime == -1 ) {
                 echoTime = 0;
             } else {
                 echoTime++;
                 if ( echoTime < 79 )
                     uart_putchar( '~' );
             }
        } else if ( echoTime != -1 ) {
             printf( "\n\rechoTime = %d\n\r", echoTime );
/*
             if ( echoTime < 80 ) {
                 for( ; echoTime >= 0; echoTime-- ) {
                     //uart_putchar( '~' );
                 }
                 puts( "\r" );
             } else {
                 echoTime = -1;
             }
*/
                 echoTime = -1;
        }
#endif
        if ( keyNum != NO_KEY_PRESSED ) {
            xPORT(B) |= (1<< 0 ); // keypad status
            if ( keyNumLast != keyNum ) {
                printf( "%c", KEYPAD_KEYCODES3[keyNum] );
                // LCD
                //LCD_command( keyNum );

#ifdef MEASURE_DIST
                if ( KEYPAD_KEYCODES3[keyNum] == '#' || KEYPAD_KEYCODES3[keyNum] == '*' ) {
                    xPORT(B) |= (1<< 1 );
                    echoTime = -1;
                }
#endif
                keyNumLast = keyNum;
            }
        } else {
            keyNumLast = keyNum;
            xPORT(B) &=~(1<< 0 ); // keypad status
#ifdef MEASURE_DIST
            xPORT(B) &=~(1<< 1 );
#endif
        }
    } // while(1)
} // main()
