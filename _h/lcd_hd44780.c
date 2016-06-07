/*
 *   LCD lib code
 */
#include "lcd_hd44780.h"
#include "lcd_hd44780_consts.c"

void lcd_preinit( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xDDR(LCD_CTRL_LINE) |= LCD_E;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
}

void lcd_init4bit( void ) {
    xDDR(LCD_CTRL_LINE) |= LCD_RW;
    xDDR(LCD_CTRL_LINE) |= LCD_RS;
    xPORT(LCD_CTRL_LINE) &=~LCD_RW;
    xPORT(LCD_CTRL_LINE) &=~LCD_RS;
    xDDR(LCD_DATA_LINE) |= 0x0F;
    xPORT(LCD_DATA_LINE) &= 0b11110000;
    xPORT(LCD_DATA_LINE) |= 0b00000011;
    lcd_signalEnable();
    _delay_ms(50);
}

void lcd_signalEnable( void ) {
    xDDR(LCD_CTRL_LINE)  |= LCD_E;
    xPORT(LCD_CTRL_LINE) |= LCD_E;
    _delay_us(50);
    xPORT(LCD_CTRL_LINE) &=~LCD_E;
}

void lcd_setDataWrite( void ) {
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

void _lcd_setCommandMode( void ) {
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

void lcd_clear( void ) {
    lcd_command( LCD_CMD_CLEAR );
}

void lcd_setCharacter( uint8_t charCode, const uint8_t* data ) {
    uint8_t oldAddr = LCD_getAddressDDRAM();
    uint8_t cmd = LCD_CMD_SET_CGRAM | ( (charCode << 3) & 0b00111000 );
    for( uint8_t i = 0; i < 8; i++ ) {
        lcd_command( cmd | i );
        lcd_putchar( *(data++) );
    }

    lcd_command( LCD_CMD_SET_DDRAM | oldAddr );
}

void lcd_setAddressCGRAM( uint8_t address ) {
    lcd_command( LCD_CMD_SET_CGRAM | ( address & 0b00111111 ) );
}

void lcd_setAddressDDRAM( uint8_t address ) {
    lcd_command( LCD_CMD_SET_DDRAM | address );
}

void lcd_setPos1( uint8_t x ) {
    lcd_setAddressDDRAM( x + LCD_LINE_1_OFFSET );
}

void lcd_setPos2( uint8_t x ) {
    lcd_setAddressDDRAM( x + LCD_LINE_2_OFFSET );
}

void lcd_setPos3( uint8_t x ) {
    lcd_setAddressDDRAM( x + LCD_LINE_3_OFFSET );
}

void lcd_setPos4( uint8_t x ) {
    lcd_setAddressDDRAM( x + LCD_LINE_4_OFFSET );
}

void lcd_gotoLine1( void ) {
    lcd_setAddressDDRAM( LCD_LINE_1_OFFSET );
}

void lcd_gotoLine2( void ) {
    lcd_setAddressDDRAM( LCD_LINE_2_OFFSET );
}

void lcd_gotoLine3( void ) {
    lcd_setAddressDDRAM( LCD_LINE_3_OFFSET );
}

void lcd_gotoLine4( void ) {
    lcd_setAddressDDRAM( LCD_LINE_4_OFFSET );
}

#ifdef LCD_USE_BUSY_FLAG
void lcd_waitShort( void ) {
    while( LCD_isBusy() ) {};
}

void lcd_waitLong( void ) {
    while( LCD_isBusy() ) {};
}
#else // LCD_USE_DELAY
void lcd_waitShort( void ) {
    _delay_us(50);
}

void lcd_waitLong( void ) {
    _delay_ms(2);
}
#endif // LCD_USE_BUSY_FLAG

void lcd_putchar( uint8_t byte ) {
    lcd_setDataWrite();
    _lcd_write( byte );
    lcd_waitShort();
}

void _lcd_write( uint8_t byte ) {
#ifdef LCD_4BIT
    _lcd_write_4bit( byte );
#else // LCD_8BIT
    _lcd_write_8bit( byte );
#endif // LCD_4BIT
}

void _lcd_write_4bit( uint8_t byte ) {
    xPORT(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_DATA_LINE) |= (byte >> 4);
    lcd_signalEnable();
    xPORT(LCD_DATA_LINE) &= 0xF0;
    xPORT(LCD_DATA_LINE) |= (byte & 0x0F);
    lcd_signalEnable();
}

void _lcd_write_8bit( uint8_t byte ) {
    xPORT(LCD_DATA_LINE) = byte;
    lcd_signalEnable();
}

void lcd_printL( const char * str, uint8_t len ) {
    LCD_setDataWrite();
    while( len-- ) {
        lcd_putchar( *(str++) );
    }
}

void lcd_print( const char * str ) {
    LCD_setDataWrite();
    while( *str ) {
        lcd_putchar( *(str++) );
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
    lcd_waitShort();
    return ret;
}

uint8_t LCD_getAddressDDRAM( void ) {
    return lcd_getBusyAndAddressDDRAM() & ~LCD_BUSY_BIT;
}

bool LCD_isBusy( void ) {
    return lcd_getBusyAndAddressDDRAM() & LCD_BUSY_BIT;
}

void _lcd_command( uint8_t cmd, bool useBusyFlag ) {
    _lcd_setCommandMode();
    _lcd_write( cmd );
    if ( useBusyFlag ) {
        while( lcd_isBusy() ) {}
    } else {
        if ( cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME ) {
            lcd_waitLong();
        } else {
            lcd_waitShort();
        }
    }
}

void lcd_command( uint8_t cmd ) {
#ifdef LCD_USE_BUSY_FLAG
    _lcd_command( cmd, true );
#else // LCD_USE_DELAY
    _lcd_command( cmd, false );
#endif // LCD_USE_BUSY_FLAG
}

void lcd_setDisplay( bool displayOn, bool displayCursor, bool cursorBlink ) {
    uint8_t cmd = cursorBlink ? ( LCD_CMD_SET_DISPLAY | LCD_CMD_CURSOR_BLINK_ON )
                              :   LCD_CMD_SET_DISPLAY;
    if ( displayCursor ) {
        cmd |= LCD_CMD_CURSOR_ON;
    }
    if ( displayOn ) {
        cmd |= LCD_CMD_DISPLAY_ON;
    }
    lcd_command( cmd );
}

void lcd_setFunction( bool use8bits, bool use2lines, bool useLargeFont ) {
    uint8_t cmd = use8bits ? ( LCD_CMD_SET_FUNCTION | LCD_CMD_8_BIT )
                           :   LCD_CMD_SET_FUNCTION;
    if ( use2lines ) {
        cmd |= LCD_CMD_2_LINES;
    }
    if ( useLargeFont ) {
        cmd |= LCD_CMD_FONT_LARGE;
    }
    lcd_command( cmd );
}

void lcd_setEntryMode( bool shiftDisplay, bool increment ) {
    uint8_t cmd = shiftDisplay ? ( LCD_CMD_SET_ENTRY_MODE | LCD_CMD_SHIFT_DISPLAY )
                               :   LCD_CMD_SET_ENTRY_MODE;
    if ( increment ) {
        cmd |= LCD_CMD_EM_INCREMENT;
    }
    lcd_command( cmd );
}

void lcd_shift( int8_t offset, bool shiftDisplay ) {
    if ( offset == 0 ) {
        return;
    }
    _lcd_setCommandMode();
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
        _lcd_write_4bit( cmd );
        lcd_waitShort();
    }
#else // LCD_8BIT
    xPORT(LCD_DATA_LINE) = cmd;
    for( ; offset > 0; offset-- ) {
        lcd_signalEnable();
        lcd_waitShort();
    }
#endif // LCD_4BIT
}

void lcd_shiftCursor( int8_t offset ) {
    lcd_shift( offset, LCD_CMD_SHIFT_DISPLAY );
}

void lcd_shiftDisplay( int8_t offset ) {
    lcd_shift( offset, LCD_CMD_SHIFT_CURSOR );
}

void lcd_setPos( uint8_t x, uint8_t y ) {
    switch( y ) {
      case 1:
        lcd_setPos1( x ); break;
      case 2:
        lcd_setPos2( x ); break;
      case 3:
        lcd_setPos3( x ); break;
      case 4:
        lcd_setPos4( x ); break;
      default:
        lcd_setPos1( x ); break;
    }
}
