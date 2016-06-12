#define F_CPU  20000000UL
#define BAUD  115200
#include <util/setbaud.h>

#define QUOTE0(...)  #__VA_ARGS__
#define QUOTE(...)  QUOTE0(__VA_ARGS__)

int main( void ) {
#pragma message QUOTE(UBRRH_VALUE)
#pragma message QUOTE(UBRRL_VALUE)
#pragma message QUOTE(USE_2X)

for(;;) {}
}