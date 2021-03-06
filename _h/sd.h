#ifndef _SD_H
#define _SD_H  1

#include <stdint.h>
#include "spi.h"
#include "asm.h"

#define SD_HC_BLOCK_LENGTH  512

#ifndef SD_INIT_IDLE_CYCLES
#define SD_INIT_IDLE_CYCLES  10
// idle for 10 bytes => 80 clocks (74 required by spec)
#endif

#ifndef SD_INIT_IF_COND_ECHO // CMD8 echoes last arg
#define SD_INIT_IF_COND_ECHO  0xAA
#endif

#ifndef SD_INIT_IF_COND_VOLTAGE // CMD8 echoes voltage if supported
#define SD_INIT_IF_COND_VOLTAGE  0x01
#endif

#ifdef SD_DEBUG
#define sd_debug_printf(...)  printf(__VA_ARGS__)
#else
#define sd_debug_printf(...)  EMPTY_STATEMENT
#endif

#ifdef SD_DEBUG_VERBOSE
#define sd_debug_verbose_printf(...)  printf(__VA_ARGS__)
#else
#define sd_debug_verbose_printf(...)  EMPTY_STATEMENT
#endif

#ifndef SD_INIT_RETRIES_MAX
#define SD_INIT_RETRIES_MAX            250  // note the uint8_t's limit (255)
#endif

#ifndef SD_SPI_RESPONSE_WAIT_MAX // Ncr (command response time) is defined as 8 bytes max BTW
#define SD_SPI_RESPONSE_WAIT_MAX  8
#endif

#define SD_RET_LEN_MAX         5

#define SD_RET_READY           0
#define SD_R1_READY            SD_RET_READY
#define SD_R1_IN_IDLE_STATE    BV(0)
#define SD_R1_ERASE_RESET      BV(1)
#define SD_R1_ILLEGAL_CMD      BV(2)
#define SD_R1_CRC_ERROR        BV(3)
#define SD_R1_ERASE_SEQ_ERR    BV(4)
#define SD_R1_ADDRESS_ERR      BV(5)
#define SD_R1_PARAM_ERR        BV(6)
#define SD_R1_ZERO_START_BIT   BV(7)

#define SD_R2_READY            SD_RET_READY
#define SD_R2_CARD_LOCKED      BV(0)
#define SD_R2_WP_ERASE_SKIP    BV(1)
#define SD_R2_LOCK_UNLOCK_FAILED  SD_R2_WP_ERASE_SKIP
#define SD_R2_ERROR            BV(2)
#define SD_R2_INTERNAL_ERROR   SD_R2_ERROR
#define SD_R2_CC_ERROR         BV(3)
#define SD_R2_CARD_ECC_FAILED  BV(4)
#define SD_R2_WP_VIOLATION     BV(5)
#define SD_R2_ERASE_PARAM      BV(6)
#define SD_R2_OUT_OF_RANGE     BV(7)
#define SD_R2_CSD_OVERWRITE    SD_R2_OUT_OF_RANGE

#define SD_INIT_NO_ERROR       0xFF

#define SD_COMMAND_ERROR       0xFF

//// SD CMD
#define SD_CMD0                0
#define SD_GO_IDLE_STATE       SD_CMD0
#define SD_CMD1                1
#define SD_SEND_OP_COND        SD_CMD1
#define SD_CMD2                2
#define SD_ALL_SEND_CID        SD_CMD2
#define SD_CMD3                3
#define SD_SEND_RELATIVE_ADDR  SD_CMD3
#define SD_CMD4                4
#define SD_SET_DSR             SD_CMD4
#define SD_CMD5                5
#define SD_IO_SEND_OP_COND     SD_CMD5
#define SD_CMD6                6
#define SD_SWITCH_FUNC         SD_CMD6
#define SD_CMD7                7
#define SD_SELECT_DESELECT_CARD  SD_CMD7
#define SD_CMD8                8
#define SD_SEND_IF_COND        SD_CMD8
#define SD_CMD9                9
#define SD_SEND_CSD            SD_CMD9
#define SD_CMD10               10
#define SD_SEND_CID            SD_CMD10
// --- none
//#define SD_CMD11               11
#define SD_CMD12               12
#define SD_STOP_TRANSMISSION   SD_CMD12
#define SD_CMD13               13
#define SD_SEND_STATUS         SD_CMD13
// --- reserved
//#define SD_CMD14               14
#define SD_CMD15               15
#define SD_GO_INACTIVE_STATE   SD_CMD15
#define SD_CMD16               16
#define SD_SET_BLOCKLEN        SD_CMD16
#define SD_CMD17               17
#define SD_READ_SINGLE_BLOCK   SD_CMD17
#define SD_CMD18               18
#define SD_READ_MULTIPLE_BLOCK  SD_CMD18
// CMD19-CMD23 reserved
// ---
#define SD_CMD24               24
#define SD_WRITE_BLOCK         SD_CMD24
#define SD_CMD25               25
#define SD_WRITE_MULTIPLE_BLOCK  SD_CMD25
// --- none
//#define SD_CMD26               26
#define SD_CMD27               27
#define SD_PROGRAM_CSD         SD_CMD28
#define SD_CMD28               28
#define SD_SET_WRITE_PROT      SD_CMD28
#define SD_CMD29               29
#define SD_CLR_WRITE_PROT      SD_CMD29
#define SD_CMD30               30
#define SD_SEND_WRITE_PROT     SD_CMD30
//#define SD_CMD31               31
// --- reserved
#define SD_CMD32               32
#define SD_ERASE_WR_BLK_START  SD_CMD32
#define SD_CMD33               33
#define SD_ERASE_WR_BLK_END    SD_CMD33
// ---
// CMD34-CMD37 reserved
#define SD_CMD38               38
#define SD_ERASE               SD_CMD38
// ---
// CMD39-CMD41 reserved
#define SD_CMD42               42
#define SD_LOCK_UNLOCK         SD_CMD42
// ---
// CMD43-51 reserved
#define SD_CMD52               52
#define SD_IO_RW_DIRECT        SD_CMD52
#define SD_CMD53               53
#define SD_IO_RW_EXTENDED      SD_CMD53
//#define SD_CMD54               54
// --- reserved
#define SD_CMD55               55
#define SD_APP_CMD             SD_CMD55
#define SD_CMD56               56
#define SD_GEN_CMD             SD_CMD56
//#define SD_CMD57               57
// --- reserved
#define SD_CMD58               58
#define SD_READ_OCR            SD_CMD58
#define SD_CMD59               59
#define SD_CRC_ON_OFF          SD_CMD59
// ---
// CMD60-53 reserved

//// SD ACMD
#define SD_ACMD6               6
#define SD_SET_BUS_WIDTH       SD_ACMD6
#define SD_ACMD13              13
#define SD_SD_STATUS           SD_ACMD13
#define SD_STATUS              SD_ACMD13
#define SD_ACMD22              22
#define SD_SEND_NUM_WR_BLOCKS  SD_ACMD22
#define SD_ACMD23              23
#define SD_SET_WR_BLK_ERASE_COUNT  SD_ACMD23
#define SD_ACMD41              41
#define SD_SD_APP_OP_COND      SD_ACMD41
#define SD_APP_OP_COND         SD_ACMD41
#define SD_ACMD42              42
#define SD_SET_CLR_CARD_DETECT  SD_ACMD42
#define SD_ACMD51              51
#define SD_SEND_SCR            SD_ACMD51

//// SD Security ACMD
#ifdef SD_SECURITY
#define SD_ACMD18              18
#define SD_ACMD25              25
#define SD_ACMD26              26
#define SD_ACMD38              38
#define SD_ACMD43              43
#define SD_ACMD44              44
#define SD_ACMD45              45
#define SD_ACMD46              46
#define SD_ACMD47              47
#define SD_ACMD48              48
#define SD_ACMD49              49
#endif

#define SD_DATA_TOKEN              0b11111110
#define SD_DATA_TOKEN_WRITE_MULTI  0b11111110
#define SD_STOP_TRAN_TOKEN         0b11111101

// common commands with CRCs:
//
// 40 00 00 00 00 95 (CMD 0)
///41 00 00 00 00 F9 (CMD 1)
// 48 00 00 01 AA 87 (CMD 8)
// 77 00 00 00 00 65 (CMD 55)
///69 00 00 00 00 E5 (ACMD 41, no HCS)
// 69 40 00 00 00 77 (ACMD 41, HCS)

void sd_preinit( void );
uint8_t sd_commandEx( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t crc, uint8_t* retBuf );
uint8_t sd_initEx( uint8_t* retBuf, bool useHcs );
uint8_t _sd_legacy_cmd1_init( uint8_t* retBuf );
void _spi_recv_block( uint8_t* buf ); // internal
uint8_t sd_readSingleBlockHC( uint8_t addr3, uint8_t addr2, uint8_t addr1, uint8_t addr0, uint8_t* buf );
uint8_t sd_readPartialBlockHC( uint8_t addr3, uint8_t addr2, uint8_t addr1, uint8_t addr0, uint8_t* buf, uint16_t offset, uint16_t len );

#ifndef SD_NO_DEFAULT_BUFFER
uint8_t sd_command( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3 );
uint8_t sd_commandCRC( uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t crc );
uint8_t sd_init( void );

extern uint8_t sd_retBuf[SD_RET_LEN_MAX];
#endif

#ifdef COMPILE_SINGLE_FILE
#include "sd.c"
#endif

#endif // _SD_H