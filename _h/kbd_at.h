#ifndef	_KBD_AT_H
#define _KBD_AT_H  1
// AT keyboard handler
#if !defined(KBD_CLK_LINE) || !defined(KBD_CLK_PIN_NR) || !defined(KBD_DATA_LINE) || !defined(KBD_DATA_PIN_NR)
#error "missing one or more needed definitions: KBD_CLK_LINE, KBD_CLK_PIN_NR, KBD_DATA_LINE, KBD_DATA_PIN_NR"
#else

#include "macro.h"
#include "kbd_at_set2.h"

#define KBD_CLK       ( 1 << KBD_CLK_PIN_NR )
#define KBD_DATA      ( 1 << KBD_DATA_PIN_NR )
#define KBD_CLK_PIN   ( xPIN(KBD_CLK_LINE)  & KBD_CLK )
#define KBD_DATA_PIN  ( xPIN(KBD_DATA_LINE) & KBD_DATA )
#define KBD_DATA_BIT  ( KBD_DATA_PIN >> KBD_DATA_PIN_NR )

#define KBD_LED_SCROLL_LOCK  0x01
#define KBD_LED_NUM_LOCK     0x02
#define KBD_LED_CAPS_LOCK    0x04

#ifndef KBD_INT
#define KBD_INT       INT0
#define KBD_INT_VECT  INT0_vect
#endif

#ifndef KBD_RETURN_ON_ERROR
//#define KBD_RETURN_ON_ERROR  KBD2(ERROR)
#define KBD_RETURN_ON_ERROR  kbd.keyCode
#endif
#ifndef kbd_debug_printf
#define kbd_debug_printf(...)  NOTHING
#endif

#define KBD_RX_WAITING  11
#define KBD_RX_DONE  0

extern bool kbd_intEnabled;
extern uint8_t kbd_LEDs;

uint8_t kbd_waitForKey( void );
void kbd_init( void );
void kbd_initInt( void );
void _kbd_initPorts( void ); // internal
bool _kbd_sendCommand( uint8_t cmd ); // internal
bool kbd_sendCommand( uint8_t cmd );
bool kbd_reset( void );
bool _kbd_reset( void ); // internal
uint8_t kbd_waitForKey_noInt( void ); // internal
uint16_t kbd_getId( void );
bool _kbd_setLEDs( uint8_t mask );
bool kbd_setLEDs( bool LED_scrollLock, bool LED_numLock, bool LED_capsLock );
bool kbd_updateLEDs( void );

#ifdef KBD_USE_INT
typedef struct {
    volatile uint8_t curBitNr, keyCode;
    volatile uint8_t parityBit, parityCnt, startBit, stopBit;
} kbd_t;

kbd_t kbd;
#endif

#ifdef COMPILE_SINGLE_FILE
#include "kbd_at.c"
#endif

#endif // req. macros
#endif // _KBD_AT_H