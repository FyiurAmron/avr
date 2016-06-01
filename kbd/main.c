//#define F_CPU  1000000UL
//#define F_CPU  8000000UL
#include "../_h/cpu.h"

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../_h/misc.h"

//#define KEYPAD C /*A*/
//#include "../_h/keypad.h"

#define BAUD  9600
#include <util/setbaud.h>
#undef USE_2X
#include "../_h/uart.h"

#define FRAME_HEIGHT  8
#define FRAME_WIDTH   4
#define FRAME_LEN     (FRAME_HEIGHT * FRAME_WIDTH)

#define LCD_DATA_LINE  A /*C*/
#define LCD_CTRL_LINE  D
#define LCD_RS  (1<< 6)
#define LCD_RW  (1<< 5)
#define LCD_E   (1<< 4)

//#define LCD_USE_BUSY_FLAG
#define LCD_4BIT
#include "../_h/lcd_hd44780.h"
#include "../_h/lcd_hd44780_ex.h"

#define KBD_DATA_LINE    D
#define KBD_CLK_LINE     D
#define KBD_DATA_PIN_NR  3
#define KBD_CLK_PIN_NR   2

#define KBD_USE_INT
#include "../_h/kbd_at.h"
#include "../_h/kbd_at_set2.h"

#include "../_miniapp/lis/lis.h"

#define debug_printf(...)  printf(__VA_ARGS__)
//#define debug_printf(...)  do {} while(0)
//#define debug_printf(...)  NOTHING

void LCD_setDefaults( void );

void LCD_setDefaults( void ) {
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_ON, LCD_CMD_CURSOR_BLINK_ON );
    LCD_setEntryMode( LCD_CMD_EM_SHIFT_CURSOR, LCD_CMD_EM_INCREMENT );
    LCD_clearEx();
}

int main( void ) {
    init();
  // set /1 system clock prescaler instead of default /8
  //CLKPR = _BV( CLKPCE );
  //CLKPR = 0;

    //while(1) {} // to quickly disable uC code

    power_adc_disable();
    //etc.
    //ADCSRA = 0;
    //xDDR(B) |= (1<< 0 ); // keypad status
    //PORTB = 0;

    kbd_init();

    uart_init();
    uart_as_stdio();

    // LCD control
    LCD_preinit();
    _delay_ms(50);
    LCD_init4bit();
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    //LCD_setFunction( LCD_CMD_8_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    //_LCD_command( )
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );  
    LCD_setDefaults();
    fputs( "\n\rDevice started...\r", &LCD_output );
    LCD_setDefaults();
    puts( "\n\rDevice started...\r" );

    bool isShift = false;
    uint8_t keyCode;

    while(1) {
        keyCode = kbd_waitForKey();
        if ( keyCode == 0xE0 ) { // extended code
            keyCode = kbd_waitForKey();
            if ( keyCode == 0xF0 ) { // extended code
                keyCode = kbd_waitForKey();
                debug_printf( "\n\r break ext %x\n\r", keyCode );
            } else {
                if ( keyCode == 0x7E ) {
                    printf( "\n\r Ctrl+Break released - soft reset!\n\r" );
                    soft_reset(); // actually halts this code
                }
                debug_printf( "\n\r press ext %x\n\r", keyCode );
            }
        } else if ( keyCode == 0xE1 ) { // Pause
            for( uint8_t i = 7; i > 0; i-- ) { // e1 14 77 e1 f0 14 f0 77
                kbd_waitForKey();
            } // don't bother checking them
            // H2D comm test (LEDs)
            kbd_sendCommand( 0xEE, 0xEE );
        } else if ( keyCode == 0xF0 ) {
            keyCode = kbd_waitForKey();
            switch ( keyCode ) {
                case 0x12:
                case 0x59:
                    isShift = false;
                    break;
            }
            debug_printf( "\n\r break %x\n\r", keyCode );
        } else { // regular code
            debug_printf( "\n\r press %x\n\r", keyCode );
            switch ( keyCode ) {
                case 0x12:
                case 0x59:
                    isShift = true;
                    break;
                case 0x76: 
                    printf( "\n\r Esc - LCD state reset!\n\r" );
                    LCD_setDefaults();
                    break;
                default: {
                    uint8_t key = (isShift ? KBD_CODE_MAP_SHIFT : KBD_CODE_MAP)[keyCode];
                    switch ( key ) {
                        case KBD_ERROR:
                            break;
                        case '`':
                            _delay_ms( 200 );
                            lisuj( &(kbd.startBit) );
                        case '\t':
                            putchar( key );
                            fputs( "    ", &LCD_output );
                            break;
                        case '\b':
                            fputs( "\b \b", stdout );
                            LCD_setPosEx( --lcdPos );
                            LCD_putcharEx( ' ' );
                            LCD_setPosEx( --lcdPos );
                            break;
                        case '\n':
                            fputs( "\n\r", stdout );
                            lcdTextBuffer[lcdPos] = 0;
                            //printf("command issued: %s\n\r", lcdTextBuffer );
                            bool dupa;
                            if( !strcmp( lcdTextBuffer, "dupa" ) ) {
                                dupa = true;
                            }
                            LCD_clearEx();
                            if ( dupa ) {
                                fputs( "dupa", &LCD_output );
                            }
                            break;
                        default:
                            putchar( key );
                            //LCD_writeEx( key );
                            //LCD_setPosEx( ++lcdPos );
                            LCD_putcharEx( key );
                            break;
                    }
                    break;
                }
            }
        }
    } // while(1)
} // main()
