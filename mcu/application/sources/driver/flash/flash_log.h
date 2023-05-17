//-----------------------------------------------------------------------------
// Project   :  Flash lOG
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- Flash lOG storaged data structure
//-----------------------------------------------------------------------------

#ifndef __lOG_FLASH_H
#define __lOG_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define lOG_BUILD_RELEASE                   (0)
#define lOG_HEADER_SETTING_MAGIC_NUM        (0xFAFAFAFA)

#define SECTOR_ID_STORAGE_HEADER_INFO       (0)
#define SECTOR_ID_BEGIN_DATA_lOG            (SECTOR_ID_STORAGE_HEADER_INFO + 1)

#if (lOG_BUILD_RELEASE == 1)
#define lOG_START_BLOCK_ID                  (1)
#define lOG_END_BLOCK_ID                    (510)
#define LOG_BLOCK_DATA_SIZE                 ((uint32_t)FLASH_BLOCK_SIZE - (uint32_t)FLASH_SECTOR_SIZE)
#else
#define lOG_START_BLOCK_ID                  (1)
#define lOG_END_BLOCK_ID                    (5)
#define LOG_BLOCK_DATA_SIZE                 (45)
#endif

#define flUNEXPECTED_VAL                    (-1)
#define MAGIC_ADDR_START_READ_lOG           (GET_FLASH_BLOCK_END_ADDR(lOG_END_BLOCK_ID) + 0x0000000A)

#define lOG_GET_HEADER_ADDR(b)              GET_FLASH_SECTOR_START_ADDR(b, SECTOR_ID_STORAGE_HEADER_INFO)
#define lOG_GET_START_DATA_ADDR(b)          GET_FLASH_SECTOR_START_ADDR(b, SECTOR_ID_BEGIN_DATA_lOG)

/* This macro function will be called when entry to iterator code segment */
#define flFUNC_REFRESH_CALLBACK()       \
    do                                  \
    {                                   \
        watchdogRst();                  \
        /* ... */                       \
    } while(0)


/* Enumarics -----------------------------------------------------------------*/
enum elOGErrorCode {
    lOG_NOERROR = 0x00,
    lOG_READ_FAILED,
    lOG_WRITE_FAILED,
    lOG_ERASE_FAILED,
    lOG_MALLOC_FAILED,
    lOG_FULL,

    lOG_ACK_READ = 0xFE, /* Can be read continue */
    lOG_NACK_READ = 0xFF /* Can't be read continue */
};

enum elOGMode {
	UNKNOWN = 0,
	OVERRIDE,
	AUTO_ERASE,
	MANUAL_ERASE,
};

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    uint32_t magicNum;
    uint32_t blockSize;
    uint32_t mallocAddr;
    int16_t blockId;
    int16_t linkBlockId;
    uint32_t lOGCount;
    uint32_t lifeCycle;
    bool isLinked;
} BlockHeader_t;

typedef struct {
    struct {
        bool target;
        int16_t bId;
        BlockHeader_t headerInfo;
        uint32_t endAddr;
    } readlOG;
    
    uint16_t nbrOfBlockUsed;
    int16_t currIdUsed;
    int16_t startIdRead;
    bool isFault;
} lOG_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void lOG_FactoryInit();
extern void lOG_NormalInit();
extern bool lOG_isBroken();

extern int8_t lOG_Write(uint8_t *payload, uint32_t nbrOfByte, uint8_t mode);
extern int8_t lOG_Read(uint8_t *payload, uint32_t nbrOfByte, uint32_t *addrPoint, uint8_t mode);
extern uint8_t lOG_Rst(void);
extern void lOG_Print(int16_t bId);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_FLASH_H */

/*
void UserReadlOGExample() {
    uint16_t counterData = 0;
    lOGDataStruct_t es35SwlOG;
    uint32_t addr;
    uint8_t isNack;

    addr = MAGIC_ADDR_START_READ_lOG;

    do {
        watchdogRst();

        memset(&es35SwlOG, 0, sizeof(lOGDataStruct_t));
        isNack = lOG_Read((uint8_t*)&es35SwlOG, sizeof(lOGDataStruct_t), &addr, AUTO_ERASE);
        delayMicros(20);
        APP_PRINT("%4d.[%s]:[", ++counterData, es35SwlOG.uptime);
        APP_PRINT("temp: %4d, humi: %4d", es35SwlOG.tempDat, es35SwlOG.humiDat);
        APP_PRINT("]\r\n");

        if (isNack == lOG_NACK_READ) {
            break;
        }
    } while (1);
}
*/