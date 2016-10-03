static const uint8_t foxChars[] = {
    0b00000, 0b00000, 0b00000, 0b00000,
    0b00000, 0b10000, 0b00000, 0b00100,
    0b01111, 0b11000, 0b00000, 0b11111,
    0b11111, 0b11111, 0b11111, 0b11100,
    0b00111, 0b10011, 0b11111, 0b11000,
    0b00000, 0b00111, 0b00111, 0b10000,
    0b00000, 0b01000, 0b00000, 0b01111,
    0b00001, 0b10000, 0b00000, 0b00000,

    0b00000, 0b00000, 0b00000, 0b00000,
    0b01111, 0b10000, 0b00000, 0b00000,
    0b11111, 0b11000, 0b00000, 0b11100,
    0b00111, 0b11111, 0b11111, 0b11111,
    0b00000, 0b00011, 0b11111, 0b11100,
    0b00000, 0b00111, 0b00111, 0b10000,
    0b00000, 0b11000, 0b00000, 0b01000,
    0b00001, 0b00000, 0b00000, 0b00111,

    0b11000, 0b00000, 0b00000, 0b00000,
    0b11111, 0b10000, 0b00000, 0b00000,
    0b01111, 0b11000, 0b00000, 0b11100,
    0b00111, 0b11111, 0b11111, 0b11111,
    0b00000, 0b00011, 0b11111, 0b11110,
    0b00000, 0b00111, 0b00111, 0b10000,
    0b00001, 0b11000, 0b00000, 0b01000,
    0b00000, 0b00000, 0b00000, 0b00110,

    0b11000, 0b00000, 0b00000, 0b00000,
    0b11111, 0b10000, 0b00000, 0b00000,
    0b11111, 0b11000, 0b00000, 0b11100,
    0b00111, 0b11111, 0b11111, 0b11111,
    0b00000, 0b00011, 0b11111, 0b11110,
    0b00000, 0b00011, 0b00111, 0b10000,
    0b00000, 0b00010, 0b00000, 0b01000,
    0b00000, 0b00110, 0b00000, 0b00110,

    0b00000, 0b00000, 0b00000, 0b00000,
    0b01111, 0b10000, 0b00000, 0b00000,
    0b11111, 0b11000, 0b00000, 0b01100,
    0b01111, 0b11111, 0b11111, 0b11111,
    0b00111, 0b10011, 0b11111, 0b11110,
    0b00000, 0b00011, 0b10111, 0b10000,
    0b00000, 0b00010, 0b00001, 0b00000,
    0b00000, 0b00001, 0b10010, 0b00000,

    0b00000, 0b00000, 0b00000, 0b00000,
    0b00000, 0b10000, 0b00000, 0b00100,
    0b00111, 0b11000, 0b00000, 0b11111,
    0b11111, 0b11111, 0b11111, 0b11100,
    0b01111, 0b10011, 0b11111, 0b11000,
    0b00000, 0b00111, 0b00111, 0b10000,
    0b00000, 0b01000, 0b00000, 0b01000,
    0b00000, 0b10000, 0b00000, 0b01000,
};

static uint8_t charOut4[FRAME_WIDTH][FRAME_HEIGHT];

void rotateChars4( const uint8_t* carr );

void rotateChars4( const uint8_t* carr ) {
    for( int8_t i = FRAME_LEN - 1; i >= 0; i-- ) {
        charOut4[i%FRAME_WIDTH][i/FRAME_WIDTH] = carr[i];
    }
}

void lisuj( bool* condition ) {
    uint8_t frameMax = sizeof(foxChars) / sizeof(foxChars[0]) / FRAME_LEN - 1;
    uint8_t frame = 0;
    uint8_t lcdSubPos = 0;

    lcd_setDisplay( LCD_CMD_DISPLAY_OFF, LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLINK_OFF );
    lcd_setPosEx( 0 );
    for( uint8_t i = 0; i < FRAME_WIDTH; i++ ) {
        LCD_putcharEx( i );
    }

    *condition = true;
    while( *condition ) {
        lcd_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLINK_OFF );
        _delay_ms( 50 );
        lcd_setDisplay( LCD_CMD_DISPLAY_OFF, LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLINK_OFF );
        rotateChars4( foxChars + FRAME_LEN * frame );
        for( uint8_t i = 0; i < 4; i++ ) {
            LCD_setCharacter( i, charOut4[i] );
        }

        lcd_setPosEx( lcdSubPos );
        lcd_putcharEx( ' ' );
        for( uint8_t i = 0; i < FRAME_WIDTH; i++ ) {
            lcd_putcharEx( i );
        }
        //printf("frame: %d\n\r", frame % frameCnt );

        if ( frame == frameMax ) {
            frame = 0;
        } else {
            frame++;
        }
        if ( lcdSubPos == LCD_TOTAL_SIZE - 1) {
            lcdSubPos = 0;
        } else {
            lcdSubPos++;
        }
    }
    lcd_setDisplay( LCD_CMD_DISPLAY_ON, LCD_CMD_CURSOR_ON, LCD_CMD_CURSOR_BLINK_ON );
    lcd_clear();
}
