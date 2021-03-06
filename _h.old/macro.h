#ifndef _MACRO_H
#define _MACRO_H  1

#define NOTHING

#define CONCAT2(x,y)  x ## y
#define CONCAT(x,y)   CONCAT2(x,y)

#define xPORT(x)  CONCAT2(PORT,x)
#define xDDR(x)   CONCAT2(DDR,x)
#define xPIN(x)   CONCAT2(PIN,x)

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)
#define _delay_ticks(x) for( volatile uint8_t _delayer = x; _delayer > 0; _delayer-- )

#define BYTE2BIN_FORMAT "%c%c%c%c%c%c%c%c"
#define BYTE2BIN_ARG(byte)   \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#endif // _MACRO_H