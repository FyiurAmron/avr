#define COMPILE_SINGLE_FILE
//#define DEBUG

#include <stdio.h>
#define QUOTE0(x)     #x
#define QUOTE(x)      QUOTE0(x)
#define CONCAT0(x,y)  x ## y
#define CONCAT(x,y)   CONCAT0(x,y)

#define PATH  std
#define H_PATH(x)  QUOTE(CONCAT(PATH,x))

#include H_PATH(arg.h)

int main() {
return 0;
}
