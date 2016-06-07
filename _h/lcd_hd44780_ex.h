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

void lcd_setPosEx( uint8_t pos );
void lcd_clearEx( void );
int _lcd_putcharEx_raw_FDEV( char c, FILE *stream );
int _lcd_putcharEx_safe_FDEV( char c, FILE *stream );
void _lcd_putcharEx_raw( uint8_t c );
void _lcd_putcharEx_safe( uint8_t c );

extern char lcdTextBuffer[LCD_TOTAL_SIZE];
extern uint8_t lcdPos;

#ifndef LCD_NO_STREAMS
extern FILE* const lcd_output;
#  ifdef LCD_PUTCHAR_RAW
extern FILE _lcd_output_raw;
#    ifndef LCD_PUTCHAR_SAFE
#      define lcd_putcharEx  lcd_putcharEx_raw
#    else
#      define lcd_putcharEx  lcd_putcharEx_safe
#    endif
#  else
#    ifndef LCD_PUTCHAR_SAFE
#      define LCD_PUTCHAR_SAFE
#    endif
#    define lcd_putcharEx  lcd_putcharEx_safe
#  endif
#  ifdef LCD_PUTCHAR_SAFE
extern FILE _lcd_output_safe;
#  endif
#endif

#define lcd_printf(...)  fprintf( &lcd_output, __VA_ARGS__ )

#ifdef COMPILE_SINGLE_FILE
#include "lcd_hd44780_ex.c"
#endif

#endif // _LCD_HD44780_EX_H