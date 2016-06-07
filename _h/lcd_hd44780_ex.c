//
#include "lcd_hd44780_ex.h"

char lcdTextBuffer[LCD_TOTAL_SIZE];
uint8_t lcdPos = 0;

void lcd_setPosEx( uint8_t pos ) {
    pos %= LCD_TOTAL_SIZE;
    lcdPos = pos;
    if ( pos >= LCD_BLOCK_SIZE ) {
        pos += (LCD_LINE_2_OFFSET - LCD_BLOCK_SIZE);
    }
    lcd_setAddressDDRAM( pos );
}

void LCD_clearEx( void ) {
    lcdPos = 0;
    memset( lcdTextBuffer, 0, sizeof(char) * LCD_TOTAL_SIZE );
    lcd_clear();
}

void LCD_putcharEx_raw( uint8_t c ) {
    lcd_putchar( c );
    lcdTextBuffer[lcdPos] = c;
    lcd_setPosEx( ++lcdPos );
}

int _lcd_putcharEx_raw_FDEV( char c, FILE *stream ) {
    lcd_putcharEx_raw( c );
    return c;
}

#ifndef LCD_PUTCHAR_SAFE_NEWLINE_CHAR
#define LCD_PUTCHAR_SAFE_NEWLINE_CHAR  '>'
#endif

void lcd_putcharEx_safe( uint8_t c ) {
    if ( c == '\n' ) {
#ifdef LCD_PUTCHAR_SAFE_CLEAR_ON_NEWLINE
        lcd_clearEx();
#else
        //LCD_setPosEx( 0 );
        lcd_putcharEx_raw( LCD_PUTCHAR_SAFE_NEWLINE_CHAR );
#endif
        return;
    }
    if ( c > 0x07 && c < ' ' ) {
        return;
    }
    lcd_putcharEx_raw( c );
}

int _LCD_putcharEx_safe_FDEV( char c, FILE *stream ) {
    lcd_putcharEx_safe( c );
    return c;
}


#ifdef LCD_PUTCHAR_RAW
FILE _lcd_output_raw = FDEV_SETUP_STREAM( _LCD_putcharEx_raw_FDEV, NULL, _FDEV_SETUP_WRITE );
#endif

#ifdef LCD_PUTCHAR_SAFE
FILE _lcd_output_safe = FDEV_SETUP_STREAM( _LCD_putcharEx_safe_FDEV, NULL, _FDEV_SETUP_WRITE );
FILE* const lcd_output = &_lcd_output_safe;
#else
#  ifdef LCD_PUTCHAR_RAW
FILE* const lcd_output = &_lcd_output_raw;
#  endif
#endif