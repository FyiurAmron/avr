#ifndef _LCD_HD44780_EX_H
#define _LCD_HD44780_EX_H  1
// ext HD44780 functions
#include "lcd_hd44780.h"

#ifndef LCD_BLOCK_SIZE
#define LCD_BLOCK_SIZE  8
#endif
#ifndef LCD_TOTAL_SIZE
#define LCD_TOTAL_SIZE  LCD_BLOCK_SIZE * 2
#endif

char lcdTextBuffer[LCD_TOTAL_SIZE];
uint8_t lcdPos = 0;

void LCD_setPosEx( uint8_t pos );
void LCD_clearEx( void );
int _LCD_putcharEx_FDEV( char c, FILE *stream );
void LCD_putcharEx( uint8_t c );

FILE LCD_output = FDEV_SETUP_STREAM( _LCD_putcharEx_FDEV, NULL, _FDEV_SETUP_WRITE );

#define LCD_printf(...)  fprintf( &LCD_output, __VA_ARGS__ )

#ifdef SINGLE_FILE
#include "lcd_hd44780_ex.c"
#endif

#endif // _LCD_HD44780_EX_H