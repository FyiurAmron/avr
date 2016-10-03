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

//#define BAUD  9600
#include "vax/uart.h"

#define LCD_DATA_LINE  A /*C*/
#define LCD_CTRL_LINE  D
#define LCD_RS_PIN_NR  6
#define LCD_RW_PIN_NR  5
#define LCD_E_PIN_NR   4

//#define LCD_USE_BUSY_FLAG
#define LCD_4BIT
#include "vax/lcd_hd44780.h"

#define FRAME_HEIGHT  8
#define FRAME_WIDTH   4
#define FRAME_LEN     (FRAME_HEIGHT * FRAME_WIDTH)

#include "lis.h"

#define SCREEN_LEN  16
void LCD_setPosEx( uint8_t pos ) {
    pos %= SCREEN_LEN;
    if ( pos >= 8 ) {
        pos += (LCD_LINE_2_OFFSET - 8);
    }
    LCD_setAddressDDRAM( pos );
}

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
    uart_stdio();

    // LCD control
    lcd_preinit();
    _delay_ms(50);
    lcd_init4bit();
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    //_LCD_command( )
    lcd_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );    
    lcd_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    lcd_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLINK_OFF );
    lcd_setEntryMode( LCD_CMD_EM_SHIFT_CURSOR, LCD_CMD_EM_INCREMENT );
    lcd_clear();

    puts("\n\rDevice started...\r");

    //const uint8_t * charPtr   = charFox;
    //const uint8_t * charLimit = charPtr + 8;
    uint8_t curPos = 0, curSubPos;

    uint8_t frameCnt = sizeof(foxChars) / sizeof(foxChars[0]) / FRAME_LEN;
    uint8_t frame = 0;
    uint8_t key, keyCode;

    while(1) {
        keyCode = getKeyPressed();
        key = ( keyCode == KEYPAD_NO_KEY_PRESSED ) ? '5' : KEYPAD_KEYCODES3[keyCode];

        if ( key == '*' ) {
            continue;
        }
        curSubPos = curPos;
        lcd_setPosEx( curSubPos );
        lcd_write( ' ' );
        for( uint8_t i = 0; i < FRAME_WIDTH; i++ ) {
            lcd_setPosEx( ++curSubPos );    
            lcd_write( i );
        }

        //printf("frame: %d\n\r", frame % frameCnt );
        rotateChars4( foxChars + FRAME_LEN * (frame % frameCnt) );

        for( uint8_t i = 0; i < 4; i++ ) {
            lcd_setCharacter( i, charOut4[i] );
        }
        frame++;
        curPos++;
        //uart_putchar('*');
        //printf( "%c ", key );
        if ( key >= '0' && key <= '9' ) {
            _delay_ms( 10 * ( key - '0' ) );
        } else if ( key >= 'A' && key <= 'D' ) {
            _delay_ms( 100 * ( key - 'A' + 1 ) );
        }
    }
} // main()
