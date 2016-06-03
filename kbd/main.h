#define COMPILE_SINGLE_FILE
//#define DEBUG

#include "../_h/cpu.h"
//#define F_CPU  1000000UL
//#define F_CPU  8000000UL

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../_h/misc.h"

//#define KEYPAD C
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
#define LCD_PUTCHAR_RAW
#define LCD_PUTCHAR_SAFE
#include "../_h/lcd_hd44780.h"
#include "../_h/lcd_hd44780_ex.h"

#define KBD_DATA_LINE    D
#define KBD_CLK_LINE     D
#define KBD_DATA_PIN_NR  3
#define KBD_CLK_PIN_NR   2

#define KBD_USE_INT
#include "../_h/kbd_at.h"
#include "../_h/kbd_at_set2ex.h"

#include "../_h/mux_output.h"

#ifdef LIS
#include "../_miniapp/lis/lis.h"
#endif

void LCD_setDefaults( void );
void LCD_fullInit( void );
void kbd_cycleLEDs( void );
void microTest( void );
