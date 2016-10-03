
#include "dist_hc_sr04.h"
//
uint8_t dist_init( void ) {
    bit8_clear( xDDR(DIST_ECHO_LINE), DIST_ECHO );
    bit8_clear( xPORT(DIST_ECHO_LINE), DIST_ECHO ); // input for ECHO
    if ( bit8_and( xPIN( DIST_ECHO_LINE), DIST_ECHO ) ) { // assert that ECHO is low
        return DIST_SENSE_HANG_UP;
    }
    bit8_set( xDDR(DIST_TRIG_LINE), DIST_TRIG );
    bit8_set( xPORT(DIST_TRIG_LINE), DIST_TRIG ); // high output for TRIG
    _delay_us(10); // 10us TRIG pulse
    bit8_clear( xPORT(DIST_TRIG_LINE), DIST_TRIG ); // low output for TRIG
    _delay_us(1); // wait with pull down before releasing
    bit8_clear( xDDR(DIST_ECHO_LINE), DIST_ECHO ); // input for ECHO
}

uint8_t dist_ping( void ) {
    dist_init();

    for( uint8_t delay = 0; !bit8_and( xPIN(DIST_ECHO_LINE), DIST_ECHO ); delay++, _delay_us(10) ) {
        if( delay > DIST_DELAY_MAX ) {
            bit8_set( xDDR(DIST_ECHO_LINE), DIST_ECHO ); // set to output (low currently) to reset
            return DIST_SENSE_HANG_UP;
        }
    } // wait for ECHO high; usually 43 loops (430 us)

    for( uint8_t cnt = 0; cnt < DIST_DELAY_MAX; cnt++, _delay_us(58) ) {
        if ( !bit8_and( xPIN(DIST_ECHO_LINE), DIST_ECHO ) ) { // wait for ECHO low
            return cnt;
        }
    }
    return DIST_SENSE_OUT_OF_RANGE;
}

uint16_t dist_pingHQ( void ) {
    dist_init();

    for( uint16_t delay = 0; !bit8_and( xPIN(DIST_ECHO_LINE), DIST_ECHO ); delay++, _delay_us(1) ) {
        if( delay > DIST_DELAY_MAX_HQ ) {
            bit8_set( xDDR(DIST_ECHO_LINE), DIST_ECHO ); // set to output (low currently) to reset
            return DIST_SENSE_HANG_UP;
        }
    } // wait for ECHO high; usually 430 loops (430 us)

    for( uint16_t cnt = 0; cnt < DIST_DELAY_MAX_HQ; cnt++, _delay_us(6) ) {
        if ( !bit8_and( xPIN(DIST_ECHO_LINE), DIST_ECHO ) ) { // wait for ECHO low
            return cnt;
        }
    }
    return DIST_SENSE_OUT_OF_RANGE;
}
