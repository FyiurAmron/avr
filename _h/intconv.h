#ifndef _INTCONV_H
#define _INTCONV_H  1

#define UINT32_TO_VALx4(x,y)        (y)((x) >> 24), (y)((x) >> 16), (y)((x) >> 8), (y)(x)
#define UINT32_TO_UINT8x4(x)        UINT32_TO_VALx4(x,uint8_t)
#define UINT8x4_TO_UINT32(a,b,c,d)  ( ( ( ( ( ( (uint32_t)(a) << 8 ) | (b) ) << 8 ) | (c) ) << 8 ) | (d) )
#define UINT8x4LE_TO_UINT32(b)      UINT8x4_TO_UINT32(b[3],b[2],b[1],b[0])
#define UINT8x4BE_TO_UINT32(b)      UINT8x4_TO_UINT32(b[0],b[1],b[2],b[3])
#define UINT8x2_TO_UINT16(a,b)      ( ( (uint16_t)(a) << 8 ) | (b) )
#define UINT8x2LE_TO_UINT16(b)      UINT8x2_TO_UINT16(b[1],b[0])
#define UINT8x2BE_TO_UINT16(b)      UINT8x2_TO_UINT16(b[0],b[1])

#endif // _INTCONV_H