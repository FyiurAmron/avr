#ifndef _FAT_H
#define _FAT_H  1
// File Allocation Table
#include "mbr.h"

typedef struct { // all uint8_t[2] == uint16_t && uint8_t[4] == uint32_t are little-endian
    uint8_t jmpBoot[3];
    uint8_t oemName[8];
    uint8_t bytesPerSec[2]; // uint16_t
    uint8_t secPerClust;
    uint8_t reservedSecCnt[2]; // uint16_t
    uint8_t numFATs;
    uint8_t rootEntryCnt[2]; // uint16_t
    uint8_t totalSec16[2]; // uint16_t
    uint8_t media;
    uint8_t fatSize16[2]; // uint16_t
    uint8_t secPerTrack[2]; // uint16_t
    uint8_t numHeads[2]; // uint16_t
    uint8_t hiddenSecCnt[4]; // uint32_t
    uint8_t totalSec32[4]; // uint32_t
// generic FAT12/16/16B/32 part ends here; 36 bytes
// FAT32 specific:
    uint8_t fatSize32[4]; // uint32_t
    uint8_t extFlags[2]; // uint16_t
    uint8_t fsVer[2]; // uint16_t
    uint8_t rootClust[4]; // uint32_t
    uint8_t fsInfo[2]; // uint16_t
    uint8_t bkBootSec[2]; // uint16_t
    uint8_t reserved0[12];
    uint8_t drvNum;
    uint8_t reserved1;
    uint8_t bootSig;
    uint8_t volID[4]; // uint32_t
    uint8_t volLabel[11]; // char[11]
    uint8_t fsName[8]; // char[8]
// FAT32 header ends here; 90 bytes
// uint8_t filler[420];
// fs_BootSectorEnd bootSectorEnd; // 0x55 0xAA
} fat_32;

typedef struct {
   uint8_t name[8]; // char[8]; name[0] can be checked using FAT_FILE_ENTRY values
   uint8_t ext[3]; // char[3]
   uint8_t attrBits;
   uint8_t ntReserved;
   uint8_t createTimeTenth;
   uint8_t createTime[2]; // uint16_t
   uint8_t createDate[2]; // uint16_t
   uint8_t lastAccessDate[2]; // uint16_t
   uint8_t clustHigh[2]; // uint16_t
   uint8_t writeTime[2]; // uint16_t
   uint8_t writeDate[2]; // uint16_t
   uint8_t clustLow[2]; // uint16_t
   uint8_t size[4]; // uint32_t
} fat_FileEntry;

#define FAT_CLUSTER_OFFSET  2

#define FAT_FILE_ENTRY_DELETED    0xE5 // non-empty may follow
#define FAT_FILE_ENTRY_EMPTY      0x00 // all other are empty
#define FAT_FILE_ENTRY_KANJI      0x05
#define FAT_FILE_ENTRY_MIN_NORMAL_VALUE  0x21

#define FAT_ATTR_READ_ONLY       BV(0)
#define FAT_ATTR_HIDDEN          BV(1)
#define FAT_ATTR_SYSTEM          BV(2)
#define FAT_ATTR_VOLUME_ID       BV(3)
#define FAT_ATTR_DIRECTORY       BV(4)
#define FAT_ATTR_ARCHIVE         BV(5)
#define FAT_ATTR_RESERVED1       BV(6)
#define FAT_ATTR_RESERVED2       BV(7)
#define FAT_ATTR_LONG_NAME       ( FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN \
                                 | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID )
#define FAT_ATTR_LONG_NAME_MASK  ( FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN \
                                 | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID \
                                 | FAT_ATTR_DIRECTORY | FAT_ATTR_ARCHIVE )

#endif // _FAT_H
