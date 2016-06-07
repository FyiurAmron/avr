#define COMPILE_SINGLE_FILE
//#define DEBUG

#include "../_h/cpu.h"
//#define F_CPU  1000000UL
//#define F_CPU  8000000UL

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
//#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../_h/misc.h"

#define BAUD  9600
#include <util/setbaud.h>
#undef USE_2X
#include "../_h/uart.h"

#define SPI_SCK_LINE     B
#define SPI_SCK_PIN_NR   7
#define SPI_MISO_LINE    B
#define SPI_MISO_PIN_NR  6
#define SPI_MOSI_LINE    B
#define SPI_MOSI_PIN_NR  5
#define SPI_SS_LINE      B
#define SPI_SS_PIN_NR    4

#define SD_DEBUG
#include "../_h/sd.h"
