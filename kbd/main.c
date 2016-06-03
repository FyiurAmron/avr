//

#include "main.h"

void LCD_setDefaults( void ) {
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );
    LCD_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_ON, LCD_CMD_CURSOR_BLINK_ON );
    LCD_setEntryMode( LCD_CMD_EM_SHIFT_CURSOR, LCD_CMD_EM_INCREMENT );
    LCD_clearEx();
}

void LCD_fullInit( void ) {
    // LCD control
    LCD_preinit();
    _delay_ms(50);
    LCD_init4bit();
    LCD_setFunction( LCD_CMD_4_BIT, LCD_CMD_2_LINES, LCD_CMD_FONT_REGULAR );  
    LCD_setDefaults();
    fputs( "\n\rLCD boot\n\r", &_LCD_output_raw );
    LCD_setDefaults();
}

void kbd_cycleLEDs( void ) {
    switch ( kbd_LEDs ) {
        case KBD_LED_CAPS_LOCK:
            kbd_LEDs = KBD_LED_SCROLL_LOCK;
            break;
        case KBD_LED_NUM_LOCK:
            kbd_LEDs = KBD_LED_CAPS_LOCK;
            break;
        case 0:
        case KBD_LED_SCROLL_LOCK:
        default:
            kbd_LEDs = KBD_LED_NUM_LOCK;
            break;
    }
    kbd_updateLEDs();
}

void microTest( void ) {
#ifdef LIS
    _delay_ms( 200 );
#ifdef KBD_USE_INT
    lisuj( &(kbd.startBit) );
#else
    bool foo = true;
    lisuj( &foo );
#endif
#endif
}

int main( void ) {
    init();
    //while(1) {} // to quickly disable uC code

    uart_init();
    //uart_as_stdio();
    mux_add( uart_output );
    mux_add( LCD_output );
    stdout = mux_output;

    uart_printf( "\n\r" );
    uart_printf( "* UART: OK\n\r" );
    uart_printf( "* AT keyboard reset: %s\n\r",  kbd_reset() && kbd_testEcho() ? "OK" : "FAILED" );
    LCD_fullInit();
    uart_printf( "* LCD: OK\n\r" );

    bool isShift = false;
    uint8_t keyCode;

    kbd.curBitNr = KBD_RX_DONE;
    while(1) {
        keyCode = kbd_waitForKey();
        if ( keyCode == KBD2(EXT) ) { // extended code
            keyCode = kbd_waitForKey();
            if ( keyCode == KBD2(BREAK) ) { // extended code
                keyCode = kbd_waitForKey();
                debug_printf( "\n\r break ext %x\n\r", keyCode );
            } else {
                if ( keyCode == KBD2E(PAUSE_BREAK) ) {
                    uart_printf( "\n\r Ctrl+Break :: uC reset...\n\r" );
                    soft_reset(); // actually halts this code
                }
                debug_printf( "\n\r press ext %x\n\r", keyCode );
            }
        } else if ( keyCode == KBD2(PAUSE) ) { // Pause
            for( uint8_t i = 7; i > 0; i-- ) { // e1 14 77 e1 f0 14 f0 77
                kbd_waitForKey();
            } // don't bother checking them BTW
            //kbd_cycleLEDs();
            printf( "\n\rkbd ID: %x\n\r", kbd_getId() );
        } else if ( keyCode == KBD2(BREAK) ) {
            keyCode = kbd_waitForKey();
            switch ( keyCode ) {
                case KBD2(L_SHIFT):
                case KBD2(R_SHIFT):
                    isShift = false;
                    break;
            }
            debug_printf( "\n\r break %x\n\r", keyCode );
        } else { // regular code
            debug_printf( "\n\r press %x\n\r", keyCode );
            switch ( keyCode ) {
                case KBD2(L_SHIFT):
                case KBD2(R_SHIFT):
                    isShift = true;
                    break;
                case KBD2(ESC): 
                    uart_printf( "\n\r Esc :: LCD reset...\n\r" );
                    LCD_setDefaults();
                    break;
                default: {
                    kbd_cycleLEDs();
                    uint8_t key = kbd_set2_getChar( keyCode, isShift );
                    switch ( key ) {
                        case KBD2(ERROR):
                            break; // unsupported char
                        case '`':
                            microTest();
                            break;
                        case '\t':
                            uart_putchar( key );
                            LCD_setPosEx( ( lcdPos / 8 ) ? 0 : 8 );
                            break;
                        case '\b':
                            fputs( "\b \b", uart_output );
                            LCD_setPosEx( --lcdPos );
                            LCD_putcharEx( ' ' );
                            LCD_setPosEx( --lcdPos );
                            break;
                        case '\n':
                            fputs( "\n\r", uart_output );
                            lcdTextBuffer[lcdPos] = 0;
                            //uart_printf("command issued: [%s]\n\r", lcdTextBuffer );
                            bool cls;
                            if( !strcmp( lcdTextBuffer, "cls" ) ) {
                                cls = true;
                            } else {
                                cls = false;
                            }  
                            LCD_clearEx();
                            if ( cls ) {
                                uart_printf( ESC"c" );
                            }
                            break;
                        default:
                            putchar( key );
                            break;
                    }
                    break;
                }
                break;
            }
        }
    } // while(1)
} // main()
