
void loop2a( uint8_t* buf) {
    for( uint16_t i = 0; i < 512; i++ ) {
        buf[i] = spi_recv();
    }
}

void loop2b( uint8_t* buf) {
    uint8_t i = 0;
    do {
        *buf++ = spi_recv();
        *buf++ = spi_recv();
    } while ( ++i );
}

void loop2c( uint8_t* buf) {
    uint8_t i = 0;
    do {
        ASM_ST_INC( buf, spi_recv() );
        ASM_ST_INC( buf, spi_recv() );
    } while ( ++i );
}

// emitted code
#if 0
////
loop2a:
        push r16
        push r17
        push r28
        push r29
/**/
        mov r16,r24
        mov r17,r25
        ldi r28,lo8(0)
        ldi r29,hi8(0)
.L8:
        rcall spi_recv
        mov r30,r16
        mov r31,r17
        st Z+,r24
        mov r16,r30
        mov r17,r31
        adiw r28,1
        ldi r31,hi8(512)
        cpi r28,lo8(512)
        cpc r29,r31
        brne .L8
// 512 * 11 = 5632 clocks 
        pop r29
        pop r28
        pop r17
        pop r16
        ret
////
loop2b:
        push r17
        push r28
        push r29
/**/
        mov r28,r24
        mov r29,r25
        ldi r17,lo8(0)
.L5:
        rcall spi_recv
        st Y,r24
        rcall spi_recv
        std Y+1,r24
        adiw r28,2
        subi r17,lo8(-(-1))
        brne .L5
// 256 * 7 = 1792 clocks
        pop r29
        pop r28
        pop r17
        ret
////
loop2c:
        push r17
        push r28
        push r29
/**/
        mov r28,r24
        mov r29,r25
        ldi r17,lo8(0)
.L2:
        rcall spi_recv
        st Y+,r24
        rcall spi_recv
        st Y+,r24
        subi r17,lo8(-(-1))
        brne .L2
// 256 * 6 = 1536 clocks
        pop r29
        pop r28
        pop r17
        ret
////
#endif