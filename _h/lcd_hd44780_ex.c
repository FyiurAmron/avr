//
#include "lcd_hd44780_ex.h"

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
    //LCD_setPosEx( lcdPos );
    memset( lcdTextBuffer, 0, sizeof(char) * LCD_TOTAL_SIZE );
    LCD_clear();
}

void LCD_putcharEx( uint8_t c ) {
    LCD_putchar( c );
    lcdTextBuffer[lcdPos] = c;
    LCD_setPosEx( ++lcdPos );
}

int _LCD_putcharEx_FDEV( char c, FILE *stream ) {
    LCD_putcharEx( c );
    return 0;
}
