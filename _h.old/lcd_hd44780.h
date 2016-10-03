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

/*
 *   LCD lib code
 */

void LCD_preinit( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xDDR(LCD_CTRL_LINE) |= LCD_E;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
}

void LCD_init4bit( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
    xDDR(LCD_DATA_LINE) |= 0x0F;
    xPORT(LCD_DATA_LINE) &= 0b11110000;
    xPORT(LCD_DATA_LINE) |= 0b00000011;
    LCD_signalEnable();
    _delay_ms(50);
}

void LCD_signalEnable( void ) {
    xDDR(LCD_CTRL_LINE)  |= LCD_E;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
}

void LCD_setDataWrite( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) |= LCD_RS;
#ifdef LCD_4BIT
    xDDR(LCD_DATA_LINE) |= 0x0F;
#else
    xDDR(LCD_DATA_LINE) = 0xFF;
#endif
}

void _LCD_setCommandMode( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
#ifdef LCD_4BIT
    xDDR(LCD_DATA_LINE) |= 0x0F;
#else
    xDDR(LCD_DATA_LINE) = 0xFF;
#endif
}

void LCD_clear( void ) {
    LCD_command( LCD_CMD_CLEAR );
}

void LCD_setCharacter( uint8_t charCode, const uint8_t* data ) {
    uint8_t oldAddr = LCD_getAddressDDRAM();
    uint8_t cmd = LCD_CMD_SET_CGRAM | ( (charCode << 3) & 0b00111000 );
    for( uint8_t i = 0; i < 8; i++ ) {
        LCD_command( cmd | i );
        LCD_putchar( *(data++) );
    }

    LCD_command( LCD_CMD_SET_DDRAM | oldAddr );
}

void LCD_setAddressCGRAM( uint8_t address ) {
    LCD_command( LCD_CMD_SET_CGRAM | ( address & 0b00111111 ) );
}

void LCD_setAddressDDRAM( uint8_t address ) {
    LCD_command( LCD_CMD_SET_DDRAM | address );
}

void LCD_setPos1( uint8_t x ) {
    LCD_setAddressDDRAM( x + LCD_LINE_1_OFFSET );
}

void LCD_setPos2( uint8_t x ) {
    LCD_setAddressDDRAM( x + LCD_LINE_2_OFFSET );
}

void LCD_setPos3( uint8_t x ) {
    LCD_setAddressDDRAM( x + LCD_LINE_3_OFFSET );
}

void LCD_setPos4( uint8_t x ) {
    LCD_setAddressDDRAM( x + LCD_LINE_4_OFFSET );
}

void LCD_gotoLine1( void ) {
    LCD_setAddressDDRAM( LCD_LINE_1_OFFSET );
}

void LCD_gotoLine2( void ) {
    LCD_setAddressDDRAM( LCD_LINE_2_OFFSET );
}

void LCD_gotoLine3( void ) {
    LCD_setAddressDDRAM( LCD_LINE_3_OFFSET );
}

void LCD_gotoLine4( void ) {
    LCD_setAddressDDRAM( LCD_LINE_4_OFFSET );
}

#ifdef LCD_USE_BUSY_FLAG
void LCD_waitShort( void ) {
    while( LCD_isBusy() ) {};
}

void LCD_waitLong( void ) {
    while( LCD_isBusy() ) {};
}
#else // LCD_USE_DELAY
void LCD_waitShort( void ) {
    _delay_us(50);
}

void LCD_waitLong( void ) {
    _delay_ms(2);
}
#endif // LCD_USE_BUSY_FLAG

void LCD_putchar( uint8_t byte ) {
    LCD_setDataWrite();
    _LCD_write( byte );
    LCD_waitShort();
}

void _LCD_write( uint8_t byte ) {
#ifdef LCD_4BIT
    _LCD_write_4bit( byte );
#else // LCD_8BIT
    _LCD_write_8bit( byte );
#endif // LCD_4BIT
}

void _LCD_write_4bit( uint8_t byte ) {
    xPORT(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_DATA_LINE) |= (byte >> 4);
    LCD_signalEnable();
    xPORT(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_DATA_LINE) |= (byte & 0x0F);
    LCD_signalEnable();
}

void _LCD_write_8bit( uint8_t byte ) {
    xPORT(LCD_DATA_LINE) = byte;
    LCD_signalEnable();
}

void LCD_printL( const char * str, uint8_t len ) {
    LCD_setDataWrite();
    while( len-- ) {
        LCD_putchar( *(str++) );
    }
}

void LCD_print( const char * str ) {
    LCD_setDataWrite();
    while( *str ) {
        LCD_putchar( *(str++) );
    }
}

uint8_t LCD_getBusyAndAddressDDRAM( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) |= LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
    xDDR(LCD_CTRL_LINE)  |= LCD_E;
    uint8_t ret;
#ifdef LCD_4BIT
    xDDR(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret = ( xPIN(LCD_DATA_LINE) & 0x0F ) << 4;
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
    _delay_us(50);
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret |= ( xPIN(LCD_DATA_LINE) & 0x0F );
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
#else
    xDDR(LCD_DATA_LINE) = 0x00;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret = xPIN(LCD_DATA_LINE);
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
#endif // LCD_4BIT
    _delay_us(50);
    return ret;
}

uint8_t LCD_read( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) |= LCD_RW;
    xPORT(LCD_CTRL_LINE) |= LCD_RS;
    xDDR(LCD_CTRL_LINE)  |= LCD_E;
    uint8_t ret;
#ifdef LCD_4BIT
    xDDR(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret = ( xPIN(LCD_DATA_LINE) & 0x0F ) << 4;
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
    LCD_waitShort();
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret |= ( xPIN(LCD_DATA_LINE) & 0x0F );
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
#else
    xDDR(LCD_DATA_LINE) = 0x00;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    ret = xPIN(LCD_DATA_LINE);
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
#endif // LCD_4BIT
    LCD_waitShort();
    return ret;
}

uint8_t LCD_getAddressDDRAM( void ) {
    return LCD_getBusyAndAddressDDRAM() & ~LCD_BUSY_BIT;
}

bool LCD_isBusy( void ) {
    return LCD_getBusyAndAddressDDRAM() & LCD_BUSY_BIT;
}

void _LCD_command( uint8_t cmd, bool useBusyFlag ) {
    _LCD_setCommandMode();
    _LCD_write( cmd );
    if ( useBusyFlag ) {
        while( LCD_isBusy() ) {}
    } else {
        if ( cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME ) {
            LCD_waitLong();
        } else {
            LCD_waitShort();
        }
    }
}

void LCD_command( uint8_t cmd ) {
#ifdef LCD_USE_BUSY_FLAG
    _LCD_command( cmd, true );
#else // LCD_USE_DELAY
    _LCD_command( cmd, false );
#endif // LCD_USE_BUSY_FLAG
}

void LCD_setDisplay( bool displayOn, bool displayCursor, bool cursorBlink ) {
    uint8_t cmd = cursorBlink ? ( LCD_CMD_SET_DISPLAY | LCD_CMD_CURSOR_BLINK_ON )
                              :   LCD_CMD_SET_DISPLAY;
    if ( displayCursor ) {
        cmd |= LCD_CMD_CURSOR_ON;
    }
    if ( displayOn ) {
        cmd |= LCD_CMD_DISPLAY_ON;
    }
    LCD_command( cmd );
}

void LCD_setFunction( bool use8bits, bool use2lines, bool useLargeFont ) {
    uint8_t cmd = use8bits ? ( LCD_CMD_SET_FUNCTION | LCD_CMD_8_BIT )
                           :   LCD_CMD_SET_FUNCTION;
    if ( use2lines ) {
        cmd |= LCD_CMD_2_LINES;
    }
    if ( useLargeFont ) {
        cmd |= LCD_CMD_FONT_LARGE;
    }
    LCD_command( cmd );
}

void LCD_setEntryMode( bool shiftDisplay, bool increment ) {
    uint8_t cmd = shiftDisplay ? ( LCD_CMD_SET_ENTRY_MODE | LCD_CMD_SHIFT_DISPLAY )
                               :   LCD_CMD_SET_ENTRY_MODE;
    if ( increment ) {
        cmd |= LCD_CMD_EM_INCREMENT;
    }
    LCD_command( cmd );
}

void LCD_shift( int8_t offset, bool shiftDisplay ) {
    if ( offset == 0 ) {
        return;
    }
    _LCD_setCommandMode();
    uint8_t cmd = LCD_CMD_SHIFT;
    if ( shiftDisplay ) {
        cmd |= LCD_CMD_SHIFT_DISPLAY;
    }
    if ( offset > 0 ) {
        cmd |= LCD_CMD_SHIFT_RIGHT;
    } else {
        offset = -offset;
    }
#ifdef LCD_4BIT
    for( ; offset > 0; offset-- ) {
        _LCD_write_4bit( cmd );
        LCD_waitShort();
    }
#else // LCD_8BIT
    xPORT(LCD_DATA_LINE) = cmd;
    for( ; offset > 0; offset-- ) {
        LCD_signalEnable();
        LCD_waitShort();
    }
#endif // LCD_4BIT
}

void LCD_shiftCursor( int8_t offset ) {
    LCD_shift( offset, LCD_CMD_SHIFT_DISPLAY );
}

void LCD_shiftDisplay( int8_t offset ) {
    LCD_shift( offset, LCD_CMD_SHIFT_CURSOR );
}

void LCD_setPos( uint8_t x, uint8_t y ) {
    switch( y ) {
      case 1:
        LCD_setPos1( x ); break;
      case 2:
        LCD_setPos2( x ); break;
      case 3:
        LCD_setPos3( x ); break;
      case 4:
        LCD_setPos4( x ); break;
      default:
        LCD_setPos1( x ); break;
    }
}

#endif // req. macros
#endif // _LCD_HD44780_H