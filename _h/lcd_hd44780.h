#ifndef	_LCD_HD44780_H
#define _LCD_HD44780_H  1
/*
 *   LCD lib
 */

#if !defined(LCD_DATA_LINE) || !defined(LCD_CTRL_LINE) || !defined (LCD_RS) || !defined LCD_RW || !defined LCD_E
#error "missing one or more needed definitions: LCD_DATA_LINE, LCD_CTRL_LINE, LCD_RS, LCD_RW, LCD_E"
#else

#include <stdbool.h>
#include "macro.h"

// define LCD_RW == LCD_RS if RW is not connected;
// code deals with it by overwriting RW address with RS values

// NOTE: in 4bit mode, LCD receives data on high nibble;
// this code sends it out on AVR port's low nibble instead

const uint8_t LCD_CMD_CLEAR             = 0b00000001;

const uint8_t LCD_CMD_HOME              = 0b00000010;

const uint8_t LCD_CMD_SET_ENTRY_MODE    = 0b00000100;
const uint8_t LCD_CMD_EM_SHIFT_CURSOR   = 0;
const uint8_t LCD_CMD_EM_SHIFT_DISPLAY	= 0b00000001;
const uint8_t LCD_CMD_EM_DECREMENT		= 0;
const uint8_t LCD_CMD_EM_INCREMENT		= 0b00000010;

const uint8_t LCD_CMD_SET_DISPLAY       = 0b00001000;
const uint8_t LCD_CMD_DISPLAY_OFF		= 0;
const uint8_t LCD_CMD_DISPLAY_ON		= 0b00000100;
const uint8_t LCD_CMD_CURSOR_OFF		= 0;
const uint8_t LCD_CMD_CURSOR_ON			= 0b00000010;
const uint8_t LCD_CMD_CURSOR_BLINK_OFF	= 0;
const uint8_t LCD_CMD_CURSOR_BLINK_ON	= 0b00000001;

const uint8_t LCD_CMD_SHIFT             = 0b00010000;
const uint8_t LCD_CMD_SHIFT_CURSOR		= 0;
const uint8_t LCD_CMD_SHIFT_DISPLAY		= 0b00001000;
const uint8_t LCD_CMD_SHIFT_LEFT	    = 0;
const uint8_t LCD_CMD_SHIFT_RIGHT       = 0b00000100;

const uint8_t LCD_CMD_SET_FUNCTION		= 0b00100000;
const uint8_t LCD_CMD_FONT_REGULAR		= 0;
const uint8_t LCD_CMD_FONT_LARGE		= 0b00000100;
const uint8_t LCD_CMD_1_LINE			= 0;
const uint8_t LCD_CMD_2_LINES			= 0b00001000;
const uint8_t LCD_CMD_4_BIT				= 0;
const uint8_t LCD_CMD_8_BIT				= 0b00010000;

const uint8_t LCD_CMD_SET_CGRAM			= 0b01000000;

const uint8_t LCD_CMD_SET_DDRAM			= 0b10000000;

const uint8_t LCD_BUSY_BIT			    = (1<< 7);

const uint8_t LCD_LINE_1_OFFSET         = 0x00;
const uint8_t LCD_LINE_2_OFFSET         = 0x40;
const uint8_t LCD_LINE_3_OFFSET         = 0x14;
const uint8_t LCD_LINE_4_OFFSET         = 0x54;

void LCD_preinit( void );
void LCD_init4bit( void );

void LCD_signalEnable( void );
void LCD_setDataWrite( void );
void LCD_setDataRead( void );
uint8_t LCD_getBusyAndAddressDDRAM( void );
void _LCD_command( uint8_t cmd, bool useBusyFlag ); // internal
void LCD_command( uint8_t cmd );
void LCD_setDisplay( bool displayOn, bool displayCursor, bool cursorBlink );
void LCD_setFunction( bool use8bits, bool use2lines, bool useLargeFont );
void LCD_setEntryMode( bool shiftDisplay, bool increment );
void LCD_shiftCursor( int8_t offset );
void LCD_shiftDisplay( int8_t offset );

void _LCD_write_4bit( uint8_t bit8 ); // internal
void _LCD_write_8bit( uint8_t bit8 ); // internal
void _LCD_write( uint8_t byte ); // internal
void LCD_putchar( uint8_t byte );
void LCD_printL( const char * str, uint8_t len );
void LCD_print( const char * str );

void LCD_waitShort( void );
void LCD_waitLong( void );
uint8_t LCD_read( void );
uint8_t LCD_getAddressDDRAM( void );
bool LCD_isBusy( void );
void LCD_clear( void );

void LCD_setCharacter( uint8_t charCode, const uint8_t* data );
void LCD_setAddressCGRAM( uint8_t address );
void LCD_setAddressDDRAM( uint8_t address );

void LCD_setPos( uint8_t x, uint8_t y );
void LCD_setPos1( uint8_t x );
void LCD_setPos2( uint8_t x );
void LCD_setPos3( uint8_t x );
void LCD_setPos4( uint8_t x );
void LCD_gotoLine1( void );
void LCD_gotoLine2( void );
void LCD_gotoLine3( void );
void LCD_gotoLine4( void );

#ifdef SINGLE_FILE
#include "lcd_hd44780.c"
#endif

#endif // req. macros
#endif // _LCD_HD44780_H