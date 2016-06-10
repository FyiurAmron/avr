#ifndef _MBR_H
#define _MBR_H  1
// Master Boot Record data
typedef struct {
    uint8_t bootFlag;
    uint8_t chsBegin[3];
    uint8_t typeCode;
    uint8_t chsEnd[3];
    uint8_t lbaStart3; // little-endian
    uint8_t lbaStart2;
    uint8_t lbaStart1;
    uint8_t lbaStart0;
    uint8_t sectorCnt[4]; // uint32_t, little-endian
} fs_PartitionEntry;

typedef struct {
    uint8_t byte1; // 0x55
    uint8_t byte2; // 0xAA
} fs_BootSectorEnd;

typedef struct {
    uint8_t bootstrap[446];
    fs_PartitionEntry partitionEntry[4];
    fs_BootSectorEnd bootSectorEnd; // 0x55 0xAA
} fs_MBR;

#define MBR_SIZE      sizeof(fs_MBR)
#define MBR_PE_SIZE   sizeof(fs_PartitionEntry)
#define MBR_EEM_SIZE  sizeof(fs_BootSectorEnd)

#endif // _MBR_H
