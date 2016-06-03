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

void LCD_setPosEx( uint8_t pos );
void LCD_clearEx( void );
int _LCD_putcharEx_raw_FDEV( char c, FILE *stream );
int _LCD_putcharEx_safe_FDEV( char c, FILE *stream );
void _LCD_putcharEx_raw( uint8_t c );
void _LCD_putcharEx_safe( uint8_t c );

extern char lcdTextBuffer[LCD_TOTAL_SIZE];
extern uint8_t lcdPos;

#ifndef LCD_NO_STREAMS
extern FILE* const LCD_output;
#  ifdef LCD_PUTCHAR_RAW
extern FILE _LCD_output_raw;
#    ifndef LCD_PUTCHAR_SAFE
#      define LCD_putcharEx  LCD_putcharEx_raw
#    else
#      define LCD_putcharEx  LCD_putcharEx_safe
#    endif
#  else
#    ifndef LCD_PUTCHAR_SAFE
#      define LCD_PUTCHAR_SAFE
#    endif
#    define LCD_putcharEx  LCD_putcharEx_safe
#  endif
#  ifdef LCD_PUTCHAR_SAFE
extern FILE _LCD_output_safe;
#  endif
#endif

#define LCD_printf(...)  fprintf( &LCD_output, __VA_ARGS__ )

// TODO better printf (char by char?)

#ifdef COMPILE_SINGLE_FILE
#include "lcd_hd44780_ex.c"
#endif

#endif // _LCD_HD44780_EX_H