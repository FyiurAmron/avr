#ifndef	_LCD_HD44780_H
#define _LCD_HD44780_H  1
/*
 *   LCD lib
 */

#if !defined(LCD_DATA_LINE) || !defined(LCD_CTRL_LINE) || !defined (LCD_RS) || !defined LCD_RW || !defined LCD_E
#error "missing one or more needed definitions: LCD_DATA_LINE, LCD_CTRL_LINE, LCD_RS, LCD_RW, LCD_E"
#else

#include "macro.h"

// define LCD_RW == LCD_RS if RW is not connected;
// code deals with it by overwriting RW address with RS values

// NOTE: in 4bit mode, LCD receives data on high nibble;
// this code sends it out on AVR port's low nibble instead

#define LCD_CMD_CLEAR              0b00000001

#define LCD_CMD_HOME               0b00000010

#define LCD_CMD_SET_ENTRY_MODE     0b00000100
#define LCD_CMD_EM_SHIFT_CURSOR    0
#define LCD_CMD_EM_SHIFT_DISPLAY   0b00000001
#define LCD_CMD_EM_DECREMENT       0
#define LCD_CMD_EM_INCREMENT       0b00000010

#define LCD_CMD_SET_DISPLAY        0b00001000
#define LCD_CMD_DISPLAY_OFF        0
#define LCD_CMD_DISPLAY_ON         0b00000100
#define LCD_CMD_CURSOR_OFF         0
#define LCD_CMD_CURSOR_ON          0b00000010
#define LCD_CMD_CURSOR_BLINK_OFF   0
#define LCD_CMD_CURSOR_BLINK_ON    0b00000001

#define LCD_CMD_SHIFT              0b00010000
#define LCD_CMD_SHIFT_CURSOR       0
#define LCD_CMD_SHIFT_DISPLAY      0b00001000
#define LCD_CMD_SHIFT_LEFT         0
#define LCD_CMD_SHIFT_RIGHT        0b00000100

#define LCD_CMD_SET_FUNCTION TAB   0b00100000
#define LCD_CMD_FONT_REGULAR TAB   0
#define LCD_CMD_FONT_LARGE         0b00000100
#define LCD_CMD_1_LINE             0
#define LCD_CMD_2_LINES            0b00001000
#define LCD_CMD_4_BIT              0
#define LCD_CMD_8_BIT              0b00010000

#define LCD_CMD_SET_CGRAM          0b01000000

#define LCD_CMD_SET_DDRAM          0b10000000

#define LCD_BUSY_BIT               (1<< 7)

#define LCD_LINE_1_OFFSET          0x00
#define LCD_LINE_2_OFFSET          0x40
#define LCD_LINE_3_OFFSET          0x14
#define LCD_LINE_4_OFFSET          0x54

void lcd_preinit( void );
void lcd_init4bit( void );

void lcd_signalEnable( void );
void lcd_setDataWrite( void );
void lcd_setDataRead( void );
uint8_t lcd_getBusyAndAddressDDRAM( void );
void _lcd_command( uint8_t cmd, bool useBusyFlag ); // internal
void lcd_command( uint8_t cmd );
void lcd_setDisplay( bool displayOn, bool displayCursor, bool cursorBlink );
void lcd_setFunction( bool use8bits, bool use2lines, bool useLargeFont );
void lcd_setEntryMode( bool shiftDisplay, bool increment );
void lcd_shiftCursor( int8_t offset );
void lcd_shiftDisplay( int8_t offset );

void _lcd_write_4bit( uint8_t bit8 ); // internal
void _lcd_write_8bit( uint8_t bit8 ); // internal
void _lcd_write( uint8_t byte ); // internal
void lcd_putchar( uint8_t byte );
void lcd_printL( const char * str, uint8_t len );
void lcd_print( const char * str );

void lcd_waitShort( void );
void lcd_waitLong( void );
uint8_t lcd_read( void );
uint8_t lcd_getAddressDDRAM( void );
bool lcd_isBusy( void );
void lcd_clear( void );

void lcd_setCharacter( uint8_t charCode, const uint8_t* data );
void lcd_setAddressCGRAM( uint8_t address );
void lcd_setAddressDDRAM( uint8_t address );

void lcd_setPos( uint8_t x, uint8_t y );
void lcd_setPos1( uint8_t x );
void lcd_setPos2( uint8_t x );
void lcd_setPos3( uint8_t x );
void lcd_setPos4( uint8_t x );
void lcd_gotoLine1( void );
void lcd_gotoLine2( void );
void lcd_gotoLine3( void );
void lcd_gotoLine4( void );

#ifdef COMPILE_SINGLE_FILE
#include "lcd_hd44780.c"
#endif

#endif // req. macros
#endif // _LCD_HD44780_H