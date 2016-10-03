//
#include "kbd_at.h"

bool kbd_intEnabled = false;
uint8_t kbd_LEDs = 0;

uint8_t kbd_waitForKey_noInt( void ) {
    uint8_t bit, keyCode = 0, parityCalc = 0;
    while ( KBD_CLK_PIN ) {}
    if ( KBD_DATA_PIN ) {
        kbd_debug_printf( "ERROR: KBD_DATA START not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    for( uint8_t i = 0; i < 8; i++ ) {
        while ( KBD_CLK_PIN ) {}
        bit = KBD_DATA_BIT;
        bit8_set( keyCode, bit << i );
        parityCalc += bit;
        while ( !KBD_CLK_PIN ) {}
    }
    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT;
    if ( bit8_lsb(parityCalc) == bit ) {
        kbd_debug_printf( "ERROR: parity wrong\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    while ( KBD_CLK_PIN ) {}
    if ( !KBD_DATA_PIN ) {
        kbd_debug_printf( "ERROR: KBD_DATA STOP not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    while ( !KBD_CLK_PIN ) {}
    return keyCode;
}

#ifdef KBD_USE_INT
ISR(KBD_INT_VECT) {
    uint8_t bit = KBD_DATA_BIT;
    switch ( kbd_state.curBitNr ) {
        case 11:
            kbd_state.curBitNr = 0; // TODO buffering?
        case 0:
            kbd_state.keyCode = 0;
            kbd_state.parityCnt = 0;
            kbd_state.startBit = 0;
            break;
        case 9:
            kbd_state.parityBit = bit;
            break;
        case 10:
            kbd_state.stopBit = bit;
            break;
        default:
            kbd_state.parityCnt += bit;
            bit8_set( kbd_state.keyCode, ( bit << (uint8_t)( kbd_state.curBitNr - 1 ) ) );
            break;
    }
    kbd_state.curBitNr++;
}

void kbd_initInt( void ) {
    bit8_clear( EICRA, BV(0) );
    bit8_set( EICRA, BV(1) );
    // 0bxxxxxx10 == on falling slope
    bit8_set( EIMSK, BV(KBD_INT) );

    kbd_state.curBitNr = KBD_RX_DONE;
    sei(); // set enable interrupts
    kbd_intEnabled = true;
    kbd_state.curBitNr = KBD_RX_DONE;
}

void kbd_disableInt( void ) {
    cli();
    kbd_intEnabled = false;
}

void kbd_init( void ) {
    _kbd_initPorts();
    kbd_initInt();
}

bool kbd_reset( void ) {
    _kbd_init_ports();
    if ( !_kbd_reset() ) {
        return false;
    }
    kbd_LEDs = 0;
    kbd_initInt();
    return true;
}

uint8_t kbd_waitForKey( void ) {
    if ( !kbd_intEnabled ) {
        return kbd_waitForKey_noInt();
    }
    while ( kbd_state.curBitNr != KBD_RX_WAITING ) {}
    if ( kbd_state.startBit ) {
        kbd_debug_printf( " valid start bit not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    if ( !kbd_state.stopBit ) {
        kbd_debug_printf( " valid stop bit not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    if ( kbd_state.parityBit == bit8_lsb( kbd_state.parityCnt ) ) {
        kbd_debug_printf( " parity bit invalid\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    kbd_state.curBitNr = KBD_RX_DONE;
    kbd_state.startBit = 1; // marks current state as invalid
    return kbd_state.keyCode;
}

#else // !KBD_USE_INT

#define kbd_reset       _kbd_reset
#define kbd_init        _kbd_initPorts
#define kbd_waitForKey  kbd_waitForKey_noInt
void kbd_init_int( void ) {}
void kbd_disable_int( void ) {}
#endif // KBD_USE_INT

void _kbd_initPorts( void ) {
    bit8_clear( xDDR(KBD_DATA_LINE), KBD_DATA );
    bit8_clear( xDDR(KBD_CLK_LINE), KBD_CLK );
    bit8_set( xPORT(KBD_DATA_LINE), KBD_DATA ); // open-collector needs pull-ups
    bit8_set( xPORT(KBD_CLK_LINE), KBD_CLK );  // open-collector needs pull-ups
}

bool kbd_testEcho( void ) {
    bool ret = kbd_sendCommand( KBD2(ECHO) ) && ( kbd_waitForKey() == KBD2(ECHO) );
    kbd_init_int();
    return ret;
}

uint16_t kbd_getId( void ) {
    if ( !kbd_sendCommand( KBD2(GET_ID) ) || kbd_waitForKey() != KBD2(ACK) ) {
        kbd_init_int();
        return 0xFFFF;
    }
    uint16_t ret = kbd_waitForKey();
    ret <<= 8;
    bit8_set( ret, kbd_waitForKey() );
    kbd_init_int();
    return ret;
}

bool _kbd_setLEDs( uint8_t mask ) {
    bool ret = kbd_sendCommand( KBD2(SET_LEDS) ) && ( kbd_waitForKey() == KBD2(ACK) );
    if ( ret ) {
        ret = kbd_sendCommand( mask ) && ( kbd_waitForKey() == KBD2(ACK) );
    }
    kbd_init_int();
    return ret;
}

bool kbd_setLEDs( bool LED_scrollLock, bool LED_numLock, bool LED_capsLock ) {
    kbd_LEDs = 0;
    if ( LED_scrollLock ) {
        bit8_set( kbd_LEDs, KBD_LED_SCROLL_LOCK );
    }
    if ( LED_numLock ) {
        bit8_set( kbd_LEDs, KBD_LED_NUM_LOCK );
    }
    if ( LED_capsLock ) {
        bit8_set( kbd_LEDs, KBD_LED_CAPS_LOCK );
    }
    return _kbd_setLEDs( kbd_LEDs );
}

bool kbd_updateLEDs() {
    return _kbd_setLEDs( kbd_LEDs );
}

bool _kbd_reset( void ) {
    // expect ACK bit, full ACK & OK; short-circuit
    bool ret = kbd_sendCommand( KBD2(RESET) )
            && ( kbd_waitForKey() == KBD2(ACK) )
            && ( kbd_waitForKey() == KBD2(RESET_OK) );
    kbd_init_int();
    return ret;
}

bool kbd_sendCommand( uint8_t cmd ) {
    kbd_disable_int();
    bit8_set( xDDR(KBD_DATA_LINE), KBD_DATA );
    bit8_set( xDDR(KBD_CLK_LINE), KBD_CLK );
    bit8_set( xPORT(KBD_DATA_LINE), KBD_DATA );
    bit8_clear( xPORT(KBD_CLK_LINE), KBD_CLK );  // 1. force CLK low
    _delay_us(100);
    bit8_clear( xPORT(KBD_DATA_LINE), KBD_DATA ); // 2. force DATA low
    //_delay_us(50);
    bit8_set( xPORT(KBD_CLK_LINE), KBD_CLK );  // & 
    bit8_clear( xDDR(KBD_CLK_LINE), KBD_CLK );  // 3. release CLK

    uint8_t bit, parity = 0; // 4. transmit DATA byte (LSB)
    for( int8_t i = 8; i > 0; i-- ) {
        bit = bit8_lsb(cmd);
        parity += bit;
        cmd >>= 1;
        while ( KBD_CLK_PIN ) {}
        bit8_setTo( xPORT(KBD_DATA_LINE), KBD_DATA, bit );
        while ( !KBD_CLK_PIN ) {}
    }
    while ( KBD_CLK_PIN ) {}
    bit8_setTo( xPORT(KBD_DATA_LINE), KBD_DATA, !bit8_lsb( parity & 1 ) );
    while ( !KBD_CLK_PIN ) {}

    while ( KBD_CLK_PIN ) {}
    bit8_set( xPORT(KBD_DATA_LINE), KBD_DATA ); // STOP
    while ( !KBD_CLK_PIN ) {}

    bit8_clear( xDDR(KBD_DATA_LINE), KBD_DATA ); // 5. release DATA

    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT; // 6. get short device ACK
    while ( !KBD_CLK_PIN ) {}

    return !bit;
}
