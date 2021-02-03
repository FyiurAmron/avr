#define F_CPU  20000000UL
//#define F_CPU  8000000UL
//#define F_CPU 1000000UL
#define COMPILE_SINGLE_FILE
#define BITS_SHORT_NAMES
#include "vax/misc.h"
#include "vax/bits.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BAUD  9600
#include "vax/uart.h"

// LCD common defines

#define LCD_CMD_DISPLAY_ON            0b00111001
#define LCD_CMD_DISPLAY_OFF           0b00111000

#define LCD_CMD_MODE_UP               0b00111011
#define LCD_CMD_MODE_DOWN             0b00111010

#define LCD_CMD_DISPLAY_START_PAGE_A  0b00111110
#define LCD_CMD_DISPLAY_START_PAGE_B  0b01111110
#define LCD_CMD_DISPLAY_START_PAGE_C  0b10111110
#define LCD_CMD_DISPLAY_START_PAGE_D  0b11111110

#define STROBE_DELAY_US  10

// LCD user defines

#define PORTDB  PORTA
#define DDRDB   DDRA
#define PINDB   PINA

#define PORTCMD  PORTC
#define DDRCMD   DDRC

#define RW  0
#define E   1
#define DI  2

#define CSEL1  3
#define CSEL2  4
#define CSEL3  5
#define CSEL4  6

#define LCD_COLUMNS  50
#define LCD_ROWS     4

#define LCD_MAX_X  LCD_COLUMNS
#define LCD_MAX_Y  LCD_ROWS

#define LCD_MAX_BANK  4

// LIB CODE

void lcd_strobeE( void ) {
    PORTCMD |= _BV(E);
    _delay_us(STROBE_DELAY_US);
    PORTCMD &= ~_BV(E);
    _delay_us(STROBE_DELAY_US);
}

void lcd_init( void ) {
    DDRCMD |= _BV(RW);
    DDRCMD |= _BV(E);
    DDRCMD |= _BV(DI);
    DDRCMD |= _BV(CSEL1);
    DDRCMD |= _BV(CSEL2);
    DDRCMD |= _BV(CSEL3);
    DDRCMD |= _BV(CSEL4);

    // TODO
}

//TODO void lcd_setup( uint8_t chuj );

void lcd_cls( void ) {
// FIXME
    PORTCMD &= ~_BV(RW);
    PORTCMD |= _BV(CSEL1);
    PORTCMD |= _BV(CSEL2);
    PORTCMD |= _BV(CSEL3);
    PORTCMD |= _BV(CSEL4);

    DDRDB  = 0b11111111;
    PORTDB = 0;

    for( uint8_t y = 0; y < LCD_ROWS; y++ ) {
        PORTCMD &= ~_BV(DI);
        PORTDB = y << 6;
        lcd_strobeE();
        PORTCMD |= _BV(DI);
        for( uint8_t x = 0; x < LCD_COLUMNS; x++ ) {
            lcd_strobeE();
        }
    }
}

//TODO void sendCmd( uint8_t cmd, uint8_t chuj );

// TODO busy check instead of delays

void lcd_displayImage( const __flash uint8_t* image ) {
    DDRDB = 0b11111111;
    const __flash uint8_t* img = image;
    for( int8_t y = 0; y < LCD_MAX_Y; y++ ) { // iterate backwards on flipped
        PORTCMD = 0b01111000;
        PORTDB = y << 6; // | 49; on flipped
        lcd_strobeE();
        for( int8_t bank = 0; bank < LCD_MAX_BANK; bank++ ) { // iterate backwards on flipped
            PORTCMD = 1 << ( bank + CSEL1 );
            PORTCMD |= _BV(DI);
            for( uint8_t x = 0; x < LCD_MAX_X; x++ ) {
                PORTDB = *img++;
                //PORTDB = x + ( 3 - bank ) * 50;
                lcd_strobeE();
            }
        }
    }
}

// END OF LIB CODE

#include "contra_logo.h"

void scrollScreen( void ) {
    for( uint8_t i = 0; i < 4; i++ ) {
        _delay_ms(250);
        PORTCMD = 0b01111000;
        PORTDB  = ( i << 6 ) | 0b00111110;
        lcd_strobeE();
    }
}

int main( void ) {
    init();

    uart_init();
    uart_stdio();
    printf( "\n\rUART device started: %ld %ld %d\n\r", UBRRH_VALUE, UBRRL_VALUE, USE_2X );

	lcd_init();

    DDRDB   = 0b11111111;
    _delay_ms(20);
    PORTCMD = 0b01111000;
    PORTDB = LCD_CMD_DISPLAY_ON; // display on
    lcd_strobeE();
    // PORTDB = LCD_CMD_MODE_DOWN; // e.g. for top-down flipped displays
    PORTDB = LCD_CMD_MODE_UP;
    lcd_strobeE();
    PORTDB = 0x00; // pos 0/0
    lcd_strobeE();
    //PORTDB = LCD_CMD_DISPLAY_START_PAGE_A;
    lcd_strobeE();
    _delay_ms(20);
    //lcd_cls();
    //_delay_ms(20);

    lcd_displayImage( contra_logo_min );

    while(1) {
        scrollScreen();
    }
} // main()
