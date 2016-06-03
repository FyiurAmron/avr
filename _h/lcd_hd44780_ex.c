//
#include "lcd_hd44780_ex.h"

char lcdTextBuffer[LCD_TOTAL_SIZE];
uint8_t lcdPos = 0;

void LCD_setPosEx( uint8_t pos ) {
    pos %= LCD_TOTAL_SIZE;
    lcdPos = pos;
    if ( pos >= LCD_BLOCK_SIZE ) {
        pos += (LCD_LINE_2_OFFSET - LCD_BLOCK_SIZE);
    }
    LCD_setAddressDDRAM( pos );
}

void LCD_clearEx( void ) {
    lcdPos = 0;
    memset( lcdTextBuffer, 0, sizeof(char) * LCD_TOTAL_SIZE );
    LCD_clear();
}

void LCD_putcharEx_raw( uint8_t c ) {
    LCD_putchar( c );
    lcdTextBuffer[lcdPos] = c;
    LCD_setPosEx( ++lcdPos );
}

int _LCD_putcharEx_raw_FDEV( char c, FILE *stream ) {
    LCD_putcharEx_raw( c );
    return 0;
}

#ifndef LCD_PUTCHAR_SAFE_NEWLINE_CHAR
#define LCD_PUTCHAR_SAFE_NEWLINE_CHAR  '>'
#endif

void LCD_putcharEx_safe( uint8_t c ) {
    if ( c == '\n' ) {
#ifdef LCD_PUTCHAR_SAFE_CLEAR_ON_NEWLINE
        LCD_clearEx();
#else
        //LCD_setPosEx( 0 );
        LCD_putcharEx_raw( LCD_PUTCHAR_SAFE_NEWLINE_CHAR );
#endif
        return;
    }
    if ( c > 0x07 && c < ' ' ) {
        return;
    }
    LCD_putcharEx_raw( c );
}

int _LCD_putcharEx_safe_FDEV( char c, FILE *stream ) {
    LCD_putcharEx_safe( c );
    return 0;
}


#ifdef LCD_PUTCHAR_RAW
FILE _LCD_output_raw = FDEV_SETUP_STREAM( _LCD_putcharEx_raw_FDEV, NULL, _FDEV_SETUP_WRITE );
#endif

#ifdef LCD_PUTCHAR_SAFE
FILE _LCD_output_safe = FDEV_SETUP_STREAM( _LCD_putcharEx_safe_FDEV, NULL, _FDEV_SETUP_WRITE );
FILE* const LCD_output = &_LCD_output_safe;
#else
#  ifdef LCD_PUTCHAR_RAW
FILE* const LCD_output = &_LCD_output_raw;
#  endif
#endif