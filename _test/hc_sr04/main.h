#define COMPILE_SINGLE_FILE
//#define DEBUG

#include "vax/cpu.h"
//#define F_CPU  1000000UL
//#define F_CPU  8000000UL

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "vax/misc.h"

//#define KEYPAD C
//#include "vax/keypad.h"

#define BAUD  9600
#include <util/setbaud.h>
#undef USE_2X
#include "vax/uart.h"

#define DIST_LINE    D
#define DIST_PIN_NR  PD4
#include "vax/dist_hc_sr04.h"


