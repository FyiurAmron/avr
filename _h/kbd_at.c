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
        keyCode |= bit << i;
        parityCalc += bit;
        while ( !KBD_CLK_PIN ) {}
    }
    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT;
    if ( (parityCalc % 2) == bit ) {
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
    switch ( kbd.curBitNr ) {
        case 11:
            kbd.curBitNr = 0; // TODO buffering?
        case 0:
            kbd.keyCode = 0;
            kbd.parityCnt = 0;
            kbd.startBit = 0;
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

void kbd_initInt( void ) {
   //EICRA |= 0b00; // on low
    EICRA |= 0b10; // on falling slope
    EIMSK |= (1 << KBD_INT);

    kbd.curBitNr = KBD_RX_DONE;
    sei(); // set enable interrupts
    kbd_intEnabled = true;
    kbd.curBitNr = KBD_RX_DONE;
}

void kbd_disableInt( void ) {
    cli();
    kbd_intEnabled = false;
}

void kbd_init( void ) {
    _kbd_init_ports();
    kbd_init_int();
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
    while ( kbd.curBitNr != KBD_RX_WAITING ) {}
    if ( kbd.startBit ) {
        kbd_debug_printf( " valid start bit not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    if ( !kbd.stopBit ) {
        kbd_debug_printf( " valid stop bit not present\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    if ( kbd.parityBit == ( kbd.parityCnt % 2 ) ) {
        kbd_debug_printf( " parity bit invalid\n\r" );
        return KBD_RETURN_ON_ERROR;
    }
    kbd.curBitNr = KBD_RX_DONE;
    kbd.startBit = 1; // marks current state as invalid
    return kbd.keyCode;
}

#else // !KBD_USE_INT

#define kbd_reset       _kbd_reset
#define kbd_init        _kbd_initPorts
#define kbd_waitForKey  kbd_waitForKey_noInt
void kbd_init_int( void ) {}
void kbd_disable_int( void ) {}
#endif // KBD_USE_INT

void _kbd_initPorts( void ) {
    xDDR(KBD_DATA_LINE)  &=~KBD_DATA;
    xDDR(KBD_CLK_LINE)   &=~KBD_CLK;
    xPORT(KBD_DATA_LINE) |= KBD_DATA; // open-collector needs pull-ups
    xPORT(KBD_CLK_LINE)  |= KBD_CLK;  // open-collector needs pull-ups
}

bool kbd_testEcho( void ) {
    bool ret = kbd_sendCommand( KBD2(ECHO) ) && kbd_waitForKey() == KBD2(ECHO);
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
    ret |= kbd_waitForKey();
    kbd_init_int();
    return ret;
}

bool _kbd_setLEDs( uint8_t mask ) {
    bool ret = kbd_sendCommand( KBD2(SET_LEDS) ) && kbd_waitForKey() == KBD2(ACK);
    if ( ret ) {
        ret = kbd_sendCommand( mask ) && kbd_waitForKey() == KBD2(ACK);
    }
    kbd_init_int();
    return ret;
}

bool kbd_setLEDs( bool LED_scrollLock, bool LED_numLock, bool LED_capsLock ) {
    kbd_LEDs = 0;
    if ( LED_scrollLock ) {
        kbd_LEDs |= KBD_LED_SCROLL_LOCK;
    }
    if ( LED_numLock ) {
        kbd_LEDs |= KBD_LED_NUM_LOCK;
    }
    if ( LED_capsLock ) {
        kbd_LEDs |= KBD_LED_CAPS_LOCK;
    }
    return _kbd_setLEDs( kbd_LEDs );
}

bool kbd_updateLEDs() {
    return _kbd_setLEDs( kbd_LEDs );
}

bool _kbd_reset( void ) {
    // expect ACK bit, full ACK & OK; short-circuit
    bool ret = kbd_sendCommand( KBD2(RESET) )
            && kbd_waitForKey() == KBD2(ACK)
            && kbd_waitForKey() == KBD2(RESET_OK);
    kbd_init_int();
    return ret;
}

bool kbd_sendCommand( uint8_t cmd ) {
    kbd_disable_int();
    xDDR(KBD_DATA_LINE) |= KBD_DATA;
    xDDR(KBD_CLK_LINE)  |= KBD_CLK;
    xPORT(KBD_DATA_LINE) |= KBD_DATA;
    xPORT(KBD_CLK_LINE)  &=~KBD_CLK;  // 1. force CLK low
    _delay_us(100);
    xPORT(KBD_DATA_LINE) &=~KBD_DATA; // 2. force DATA low
    //_delay_us(50);
    xPORT(KBD_CLK_LINE)  |= KBD_CLK;  // & 
    xDDR(KBD_CLK_LINE)   &=~KBD_CLK;  // 3. release CLK

    uint8_t bit, parity = 0; // 4. transmit DATA byte (LSB)
    for( int8_t i = 8; i > 0; i-- ) {
        bit = (cmd & 1);
        parity += bit;
        cmd >>= 1;
        while ( KBD_CLK_PIN ) {}
        if ( bit ) {
            xPORT(KBD_DATA_LINE) |= KBD_DATA;
        } else {
            xPORT(KBD_DATA_LINE) &=~KBD_DATA;
        }
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
    xPORT(KBD_DATA_LINE) |= KBD_DATA; // STOP
    //xDDR(KBD_DATA_LINE)  &=~KBD_DATA; // 5. release DATA
    while ( !KBD_CLK_PIN ) {}

    xDDR(KBD_DATA_LINE)  &=~KBD_DATA; // 5. release DATA
    //xPORT(KBD_DATA_LINE) |= KBD_DATA; 

    while ( KBD_CLK_PIN ) {}
    bit = KBD_DATA_BIT; // 6. get short device ACK
    while ( !KBD_CLK_PIN ) {}

    return !bit;
}
