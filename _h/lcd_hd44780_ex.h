#ifndef _LCD_HD44780_EX_H
#define _LCD_HD44780_EX_H  1
// ext HD44780 functions
#ifndef LCD_BLOCK_SIZE
#define LCD_BLOCK_SIZE  8
#endif
#ifndef LCD_TOTAL_SIZE
#define LCD_TOTAL_SIZE  LCD_BLOCK_SIZE * 2
#endif

char lcdTextBuffer[LCD_TOTAL_SIZE];
uint8_t lcdPos = 0;

int _LCD_putchar_FDEV( char c, FILE *stream );
FILE LCD_output = FDEV_SETUP_STREAM( _LCD_putchar_FDEV, NULL, _FDEV_SETUP_WRITE );

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

int _LCD_putchar_FDEV( char c, FILE *stream ) {
    LCD_putcharEx( c );
    return 0;
}
#endif // _LCD_HD44780_EX_H