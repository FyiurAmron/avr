#define F_CPU 1000000UL
//#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#define BAUD 9600
#include <util/setbaud.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>

#include "vax/misc.h"

int main( void ) {
    uint8_t i;
    while(1){

        for(i = 10; i >= 0; i-- ) {
            ASM_NOP();
        } 

/*
        for(i = 0; i < 11; i++) {
            ASM_NOP();
        }
*/
/*
        i = 10;
        do {
            ASM_NOP();
        } while ( i-- );
*/
/*
        i = 11;
        while(i--> 0) {
            ASM_NOP();
        } 
*/
    }
}
