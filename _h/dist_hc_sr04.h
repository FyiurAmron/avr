#ifndef	_DIST_HC_SR04
#define _DIST_HC_SR04  1
/*
 *   HC-SR04 hi-freq soundwave distance sensor (SRF04-compatible)
 */
#include "macro.h"

#ifdef DIST_LINE
#define DIST_TRIG_LINE    DIST_LINE
#define DIST_ECHO_LINE    DIST_LINE
#define DIST_TRIG_PIN_NR  DIST_PIN_NR
#define DIST_ECHO_PIN_NR  DIST_PIN_NR
#endif

#if !defined(DIST_TRIG_LINE) || !defined(DIST_TRIG_PIN_NR) || !defined (DIST_ECHO_LINE) || !defined DIST_ECHO_PIN_NR
#error "missing one or more needed definitions: DIST_[TRIG_|ECHO_|]LINE, DIST_[TRIG_|ECHO_|]PIN_NR"
#else

#ifndef DIST_DELAY_MAX
#define DIST_DELAY_MAX  250
#endif

#ifndef DIST_DELAY_MAX_HQ
#define DIST_DELAY_MAX_HQ  5000
#endif

#define DIST_TRIG        (1<< DIST_TRIG_PIN_NR)
#define DIST_ECHO        (1<< DIST_ECHO_PIN_NR)

#define DIST_SENSE_OUT_OF_RANGE  0xFF
#define DIST_SENSE_HANG_UP       0xFE

#define DIST_SENSE_OUT_OF_RANGE_HQ  DIST_SENSE_OUT_OF_RANGE ## DIST_SENSE_OUT_OF_RANGE
#define DIST_SENSE_HANG_UP_HQ       DIST_SENSE_HANG_UP ## DIST_SENSE_HANG_UP

uint8_t dist_ping( void );
uint16_t dist_pingHQ( void );

#ifdef COMPILE_SINGLE_FILE
#include "dist_hc_sr04.c"
#endif

#endif // req. macros
#endif // _DIST_HC_SR04