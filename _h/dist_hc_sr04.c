//
uint8_t dist_ping( void ) {
    xDDR(DIST_ECHO_LINE)  &=~DIST_ECHO;
    xPORT(DIST_ECHO_LINE) &=~DIST_ECHO; // input for ECHO
    if ( xPIN(DIST_ECHO_LINE) & DIST_ECHO ) { // assert that ECHO is low
        return DIST_SENSE_HANG_UP;
    }
    xDDR(DIST_TRIG_LINE)  |= DIST_TRIG;
    xPORT(DIST_TRIG_LINE) |= DIST_TRIG; // high output for TRIG
    _delay_us(10); // 10us TRIG pulse
    xPORT(DIST_TRIG_LINE) &=~DIST_TRIG; // low output for TRIG
    _delay_us(1); // wait with pull down before releasing
    xDDR(DIST_ECHO_LINE)  &=~DIST_ECHO; // input for ECHO

    for( uint8_t delay = 0; !(xPIN(DIST_ECHO_LINE) & DIST_ECHO); delay++, _delay_us(10) ) {
        if( delay > DIST_DELAY_MAX ) {
            xDDR(DIST_ECHO_LINE)  |= DIST_ECHO; // set to output (low currently) to reset
            return DIST_SENSE_HANG_UP;
        }
    } // wait for ECHO high; usually 43 loops (430 us)

    for( uint8_t cnt = 0; cnt < DIST_DELAY_MAX; cnt++, _delay_us(58) ) {
        if ( !( xPIN(DIST_ECHO_LINE) & DIST_ECHO ) ) { // wait for ECHO low
            return cnt;
        }
    }
    return DIST_SENSE_OUT_OF_RANGE;
}

uint16_t dist_pingHQ( void ) {
    xDDR(DIST_ECHO_LINE)  &=~DIST_ECHO;
    xPORT(DIST_ECHO_LINE) &=~DIST_ECHO; // input for ECHO
    if ( xPIN(DIST_ECHO_LINE) & DIST_ECHO ) { // assert that ECHO is low
        return DIST_SENSE_HANG_UP;
    }
    xDDR(DIST_TRIG_LINE)  |= DIST_TRIG;
    xPORT(DIST_TRIG_LINE) |= DIST_TRIG; // high output for TRIG
    _delay_us(10); // 10us TRIG pulse
    xPORT(DIST_TRIG_LINE) &=~DIST_TRIG; // low output for TRIG
    _delay_us(1); // wait with pull down before releasing
    xDDR(DIST_ECHO_LINE)  &=~DIST_ECHO; // input for ECHO

    for( uint16_t delay = 0; !(xPIN(DIST_ECHO_LINE) & DIST_ECHO); delay++, _delay_us(1) ) {
        if( delay > DIST_DELAY_MAX_HQ ) {
            xDDR(DIST_ECHO_LINE)  |= DIST_ECHO; // set to output (low currently) to reset
            return DIST_SENSE_HANG_UP;
        }
    } // wait for ECHO high; usually 430 loops (430 us)

    for( uint16_t cnt = 0; cnt < DIST_DELAY_MAX_HQ; cnt++, _delay_us(6) ) {
        if ( !( xPIN(DIST_ECHO_LINE) & DIST_ECHO ) ) { // wait for ECHO low
            return cnt;
        }
    }
    return DIST_SENSE_OUT_OF_RANGE;
}
