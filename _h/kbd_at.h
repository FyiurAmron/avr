#ifndef	_KBD_AT_H
#define _KBD_AT_H  1
// AT keyboard handler
#include "macro.h"

#if !defined(KBD_CLK_LINE) || !defined(KBD_CLK_PIN_NR) || !defined(KBD_DATA_LINE) || !defined(KBD_DATA_PIN_NR)
#error "missing one or more needed definitions: KBD_CLK_LINE, KBD_CLK_PIN_NR, KBD_DATA_LINE, KBD_DATA_PIN_NR"
#else

#define KBD_CLK       ( 1 << KBD_CLK_PIN_NR )
#define KBD_DATA      ( 1 << KBD_DATA_PIN_NR )
#define KBD_CLK_PIN   ( xPIN(KBD_CLK_LINE)  & KBD_CLK )
#define KBD_DATA_PIN  ( xPIN(KBD_DATA_LINE) & KBD_DATA )
#define KBD_DATA_BIT  ( KBD_DATA >> KBD_DATA_PIN_NR )

#define KBD_TAB_LENGTH   4
#define KBD_ERROR  0xFF

#ifndef KBD_INT
#define KBD_INT       INT0
#define KBD_INT_VECT  INT0_vect
#endif

const uint8_t KBD_RX_WAITING = 11;
const uint8_t KBD_RX_DONE = 0;

uint8_t getKeyPressed( void );
void kbd_init( void );
uint8_t kbd_sendCommand( uint8_t cmd, uint8_t data );

#ifdef KBD_USE_INT
typedef struct {
    volatile uint8_t curBitNr, keyCode;
    volatile uint8_t parityBit, parityCnt, startBit, stopBit;
} kbd_t;

kbd_t kbd;

ISR(KBD_INT_VECT) {
    uint8_t bit = KBD_DATA_BIT;
    switch ( kbd.curBitNr ) {
        case 11:
            kbd.curBitNr = 0; // TODO buffering?
        case 0:
            kbd.keyCode = 0;
            kbd.parityCnt = 0;
            kbd.startBit = bit;
            break;
        case 9:
            kbd.parityBit = bit;
            break;
        case 10:
            kbd.stopBit = bit;
            break;
        default:
            kbd.parityCnt += bit;
            kbd.keyCode |= (uint8_t)( bit << ( kbd.curBitNr - 1 ) );
            break;
    }
    kbd.curBitNr++;
}

void kbd_init( void ) {
    xDDR(KBD_DATA_LINE) &=~ KBD_DATA;
    xDDR(KBD_CLK_LINE)  &=~ KBD_CLK;
    xPORT(KBD_DATA_LINE) |= KBD_DATA; // open-collector needs pull-ups
    xPORT(KBD_DATA_LINE) |= KBD_CLK;  // open-collector needs pull-ups

    //EICRA |= 0b00; // on low
    EICRA |= 0b10; // on falling slope
    EIMSK |= (1 << KBD_INT);

    kbd.curBitNr = 0;

    sei(); // set enable interrupts
}

uint8_t kbd_waitForKey( void ) {
    while ( kbd.curBitNr != KBD_RX_WAITING ) {}
    if ( kbd.startBit ) {
        printf( "valid start bit not present" );
        return KBD_ERROR;
    }
    if ( !kbd.stopBit ) {
        printf( "valid stop bit not present" );
        return KBD_ERROR;
    }
    if ( kbd.parityBit == ( kbd.parityCnt % 2 ) ) {
        printf( "parity bit invalid" );
        return KBD_ERROR;
    }
    kbd.curBitNr = KBD_RX_DONE;
    kbd.startBit = 1; // marks current state as invalid
    return kbd.keyCode;
}

#else // !KBD_USE_INT

void kbd_init( void ) {
}

uint8_t kbd_waitForKey( void ) {
    uint8_t bit, keyCode = 0, parityCalc = 0;
    while ( KBD_CLK_PIN ) {}
    if ( KBD_DATA_PIN ) {
        //printf( "ERROR: KBD_DATA START not present\n\r" );
        return KBD_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    for( uint8_t i = 0; i < 8; i++ ) {
        while ( KBD_CLK_PIN ) {}
        bit = KBD_DATA_BIT;
        keyCode |= bit << i;
        parityCalc += bit;
        while ( !KBD_CLK_PIN ) {}
    }
    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT;
    if ( (parityCalc % 2) == bit ) {
        //printf( "ERROR: parity wrong\n\r" );
        return KBD_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    while ( KBD_CLK_PIN ) {}
    if ( !KBD_DATA_PIN ) {
        //printf( "ERROR: KBD_DATA STOP not present\n\r" );
        return KBD_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    return keyCode;
}

#endif // KBD_USE_INT

uint8_t kbd_sendCommand( uint8_t cmd, uint8_t data ) {
    cli(); // clear interrupts

    xDDR(KBD_DATA_LINE) |= KBD_DATA;
    xDDR(KBD_CLK_LINE)  |= KBD_CLK;
    xPORT(KBD_DATA_LINE) |= KBD_DATA;
    xPORT(KBD_DATA_LINE) &=~KBD_CLK;  // 1. force CLK low
    _delay_us(100);
    xPORT(KBD_DATA_LINE) &=~KBD_DATA; // 2. force DATA low
    _delay_us(50);
    xDDR(KBD_CLK_LINE)   &=~KBD_CLK;  // 3. release CLK
    xPORT(KBD_DATA_LINE) |= KBD_CLK;

    uint8_t bit, parity = 0; // 4. transmit DATA byte (LSB)
    for( int8_t i = 8; i > 0; i-- ) {
        while ( KBD_CLK_PIN ) {}
        bit = (cmd & 1);
        if ( bit ) {
            xPORT(KBD_DATA_LINE) |= KBD_DATA;
        } else {
            xPORT(KBD_DATA_LINE) &=~KBD_DATA;
        }
        parity += bit;
        cmd >>= 1;
        while ( !KBD_CLK_PIN ) {}
    }
    while ( KBD_CLK_PIN ) {}
    if ( parity % 2 ) {
        xPORT(KBD_DATA_LINE) &=~KBD_DATA;
    } else {
        xPORT(KBD_DATA_LINE) |= KBD_DATA;
    }
    while ( !KBD_CLK_PIN ) {}

    while ( KBD_CLK_PIN ) {}
    xDDR(KBD_DATA_LINE) &=~ KBD_DATA;
    xPORT(KBD_DATA_LINE) |= KBD_DATA; // 5. release DATA
    while ( !KBD_CLK_PIN ) {}

    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT; // 6. get short device ACK
    while ( !KBD_CLK_PIN ) {}

    printf( bit ? "ACK!" : "NACK..." );

    sei(); // set enable interrupts

    return bit;
}

#endif // req. macros
#endif // _KBD_AT_H