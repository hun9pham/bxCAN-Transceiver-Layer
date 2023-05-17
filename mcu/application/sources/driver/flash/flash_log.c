//-----------------------------------------------------------------------------
// Project   :  Flash lOG
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- Flash lOG storaged data structure
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "xprintf.h"

#include "flash.h"
#include "flash_log.h"

#define TAG "flashlOG"

/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define ASSERT_FAIL_RETRY_MAX           (3)
#define lOG_MONITOR_EN                  (1)

#if (lOG_MONITOR_EN == 1)
#define lOG_PRINT(tag, fmt, ...)        xprintf(KRED "[" tag "] " KBLU fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define lOG_PRINT(tag, fmt, ...)
#endif

/* Private variables ---------------------------------------------------------*/
static lOG_t lOG = {
    .readlOG = {
        .target = false,
        .bId = flUNEXPECTED_VAL,
        .headerInfo = { 0 }
    },
    .nbrOfBlockUsed = 0,
    .currIdUsed = 0,
    .startIdRead = 0,
    .isFault = false
};

/* Private function prototypes -----------------------------------------------*/
static bool defaultHeaderInit(int16_t bId);
static bool headerUpdate(int16_t bId, BlockHeader_t *new);

static bool assertWrite(uint32_t addr, uint8_t *buf, uint32_t len);
static bool assertRead(uint32_t addr, uint8_t *buf, uint32_t len);
static bool assertBlockErase(int16_t bId);
static bool assertSectorErase(int16_t bId, uint8_t seId);

/* Function implementation ---------------------------------------------------*/
void lOG_FactoryInit() {
    uint8_t faultTimes = 0;

    for (int16_t id = lOG_START_BLOCK_ID; id <= lOG_END_BLOCK_ID; ++id) {
        flFUNC_REFRESH_CALLBACK();
        faultTimes += (uint8_t)defaultHeaderInit(id);
    }

    lOG.currIdUsed = lOG_START_BLOCK_ID;
    lOG.startIdRead = lOG_START_BLOCK_ID;
    lOG.nbrOfBlockUsed = 1;
    lOG.readlOG.target = false;
    lOG.readlOG.bId = flUNEXPECTED_VAL;
    if (faultTimes != 0) {
        lOG.isFault = true;
    }
    else {
        lOG.isFault = false;
    }

    lOG_PRINT(TAG, "[FACINIT] startIdRead: %d", lOG.startIdRead);
    lOG_PRINT(TAG, "[FACINIT] currIdUsed: %d", lOG.currIdUsed);
    lOG_PRINT(TAG, "[FACINIT] nbrOfBlockUsed: %d", lOG.nbrOfBlockUsed);
    lOG_PRINT(TAG, "[FACINIT] isFault: %d", lOG.isFault);
}

void lOG_NormalInit() {
    BlockHeader_t header;
    uint8_t faultTimes = 0;
    uint32_t minCycleLife = 0xFFFF;

    lOG.nbrOfBlockUsed = 1;
    lOG.readlOG.target = false;
    lOG.readlOG.bId = flUNEXPECTED_VAL;
    lOG.currIdUsed = flUNEXPECTED_VAL;
    lOG.isFault = false;

    for (int16_t id = lOG_START_BLOCK_ID; id <= lOG_END_BLOCK_ID; ++id) {
        flFUNC_REFRESH_CALLBACK();        
        faultTimes += (uint8_t)assertRead(lOG_GET_HEADER_ADDR(id), (uint8_t*)&header, sizeof(BlockHeader_t));
        if (header.magicNum != lOG_HEADER_SETTING_MAGIC_NUM) {
            faultTimes += (uint8_t)defaultHeaderInit(id);
            if (flUNEXPECTED_VAL == lOG.currIdUsed) {
                lOG.currIdUsed = id;
            }
            continue;
        }

        /* Finding last block id used */
        if (header.isLinked == false && lOG.currIdUsed == flUNEXPECTED_VAL) {
            lOG.currIdUsed = header.blockId;
        }
        else if (header.isLinked == true) {
            ++lOG.nbrOfBlockUsed;
        }

        /* Finding start block read id */
        if (minCycleLife > header.lifeCycle) {
            minCycleLife = header.lifeCycle;
            lOG.startIdRead = header.blockId;
        }
    }

    if (faultTimes != 0) {
        lOG.isFault = true;
    }
    
    lOG_PRINT(TAG, "[NORINIT] startIdRead: %d", lOG.startIdRead);
    lOG_PRINT(TAG, "[NORINIT] currIdUsed: %d", lOG.currIdUsed);
    lOG_PRINT(TAG, "[NORINIT] nbrOfBlockUsed: %d", lOG.nbrOfBlockUsed);
    lOG_PRINT(TAG, "[NORINIT] isFault: %d", lOG.isFault);
}

bool lOG_isBroken() {
    return lOG.isFault;
}

int8_t lOG_Write(uint8_t *payload, uint32_t nbrOfByte, uint8_t mode) {
    uint32_t headerAddr;
    BlockHeader_t header;
    uint32_t addressWrite;

#if 0
    if (lOG.currIdUsed == flUNEXPECTED_VAL) {
        int16_t tmpId = lOG_START_BLOCK_ID;
        do {
            flFUNC_REFRESH_CALLBACK();            
            if (assertRead(lOG_GET_HEADER_ADDR(tmpId), (uint8_t*)&header, sizeof(BlockHeader_t))) {
                return lOG_READ_FAILED;
            }
            tmpId = header.linkBlockId;
        } while (header.isLinked == true);
        lOG.currIdUsed = header.blockId;
    }
#endif

    headerAddr = lOG_GET_HEADER_ADDR(lOG.currIdUsed);
    if (assertRead(headerAddr, (uint8_t*)&header, sizeof(BlockHeader_t))) {
        lOG.isFault = true;
        return -1;
    }

    if (header.blockSize < nbrOfByte) {
        if (AUTO_ERASE == mode && header.blockId == lOG_END_BLOCK_ID) {
            if (lOG_NOERROR != lOG_Rst()) {
                return lOG_ERASE_FAILED;
            }

            lOG.startIdRead = lOG_START_BLOCK_ID;
            lOG.currIdUsed = lOG_START_BLOCK_ID;
            headerAddr = lOG_GET_HEADER_ADDR(lOG.currIdUsed);
            if (assertRead(headerAddr, (uint8_t*)&header, sizeof(BlockHeader_t))) {
                return -2;
            }
        }
        else if (MANUAL_ERASE == mode && header.blockId == lOG_END_BLOCK_ID) {
            return lOG_FULL;
        }
        else {
            int16_t nextId;

            nextId = header.linkBlockId;
            header.isLinked = true;
            if (headerUpdate(header.blockId, &header)) {
                return -3;
            }
            headerAddr = lOG_GET_HEADER_ADDR(nextId);
            if (assertRead(headerAddr, (uint8_t*)&header, sizeof(BlockHeader_t))) {
                return -4;
            }
            if (OVERRIDE == mode) {
                if (header.isLinked == true) {
                    header.blockSize = LOG_BLOCK_DATA_SIZE;
                    header.mallocAddr = lOG_GET_START_DATA_ADDR(nextId);
                    header.lOGCount = 0;
                    header.isLinked = false;
                    header.lifeCycle += 1;
                    if (assertBlockErase(header.blockId)) {
                        return -5;
                    }
                    if (headerUpdate(nextId, &header)) {
                        return -6;
                    }
                    lOG.startIdRead = header.linkBlockId;
                }
            }
            lOG.currIdUsed = nextId;
            lOG.nbrOfBlockUsed += (lOG.nbrOfBlockUsed < lOG_END_BLOCK_ID ? 1 : 0);
        }
    }

    if (header.blockSize > nbrOfByte) {
        addressWrite = header.mallocAddr;
        header.mallocAddr += nbrOfByte;
        header.blockSize -= nbrOfByte;
        if (assertWrite(addressWrite, payload, nbrOfByte)) {
            return -7;
        }
        ++(header.lOGCount);
        if (headerUpdate(lOG.currIdUsed, &header)) {
            return -8;
        }
    }
    else {
        return lOG_MALLOC_FAILED;
    }

#if defined (lOG_MONITOR_EN)
    assertRead(lOG_GET_HEADER_ADDR(lOG.currIdUsed), (uint8_t*)&header, sizeof(BlockHeader_t));
    lOG_PRINT(TAG, "[lOGW] Id: %d, Write to 0x%X, len: %d, blockSize: %d", lOG.currIdUsed, addressWrite, nbrOfByte, header.blockSize);
#endif

    return lOG_NOERROR;
}

int8_t lOG_Read(uint8_t *payload, uint32_t nbrOfByte, uint32_t *addrPoint, uint8_t mode) {
    static int16_t __nbrOfBlockUsed;

    if (*addrPoint == MAGIC_ADDR_START_READ_lOG) {
        lOG.readlOG.target = false;
        __nbrOfBlockUsed = flUNEXPECTED_VAL;
    }

    if (flUNEXPECTED_VAL == __nbrOfBlockUsed) {
        __nbrOfBlockUsed = lOG.nbrOfBlockUsed;
    }

    if (lOG.readlOG.target == false) {
        lOG.readlOG.target = true;
        lOG.readlOG.bId = (OVERRIDE != mode ? lOG_START_BLOCK_ID : lOG.startIdRead);
        memset(&lOG.readlOG.headerInfo, 0, sizeof(BlockHeader_t));
        if (assertRead(lOG_GET_HEADER_ADDR(lOG.readlOG.bId), (uint8_t*)&lOG.readlOG.headerInfo, sizeof(BlockHeader_t))) {
            return -1;
        }
        lOG.readlOG.endAddr = lOG.readlOG.headerInfo.mallocAddr - 0x00000001;
        *addrPoint = lOG_GET_START_DATA_ADDR(lOG.readlOG.bId);
        --(__nbrOfBlockUsed);
        lOG_PRINT(TAG, "[lOGR] Id: %d, lOGCnt: %d, isLinked: %d", lOG.readlOG.bId, lOG.readlOG.headerInfo.lOGCount, lOG.readlOG.headerInfo.isLinked);
    }

    if (lOG.readlOG.headerInfo.lOGCount != 0) {
        if (assertRead(*addrPoint, payload, nbrOfByte)) {
            return -2;
        }
        *addrPoint += nbrOfByte;
        if (*addrPoint >= lOG.readlOG.endAddr) {
            if (OVERRIDE != mode) {
                if (__nbrOfBlockUsed == 0) {
                    return lOG_NACK_READ;
                }
                lOG.readlOG.bId = lOG.readlOG.headerInfo.linkBlockId;
                --(__nbrOfBlockUsed);
            }
            else {
                if (lOG.readlOG.headerInfo.isLinked == false) {
                    return lOG_NACK_READ;
                }
                lOG.readlOG.bId = lOG.readlOG.headerInfo.linkBlockId;
            }

            memset(&lOG.readlOG.headerInfo, 0, sizeof(BlockHeader_t));
            if (assertRead(lOG_GET_HEADER_ADDR(lOG.readlOG.bId), (uint8_t*)&lOG.readlOG.headerInfo, sizeof(BlockHeader_t))) {
                return -2;
            }
            lOG.readlOG.endAddr = lOG.readlOG.headerInfo.mallocAddr - 0x00000001;
            *addrPoint = lOG_GET_START_DATA_ADDR(lOG.readlOG.bId);
            lOG_PRINT(TAG, "[lOGR] Id: %d, lOGCnt: %d, isLinked: %d", lOG.readlOG.bId, lOG.readlOG.headerInfo.lOGCount, lOG.readlOG.headerInfo.isLinked);
        }
    }
    else {
        return lOG_NACK_READ;
    }

    return lOG_NOERROR;
}

uint8_t lOG_Rst() {
    uint8_t faultTimes = 0;
    BlockHeader_t headerRst;
    int16_t tmpId = lOG_START_BLOCK_ID;

#if 1
    for (; tmpId <= lOG.nbrOfBlockUsed; ++tmpId) {
        lOG_PRINT(TAG, "[lOGE] Id: %d", tmpId);
        flFUNC_REFRESH_CALLBACK();
        faultTimes += assertRead(lOG_GET_HEADER_ADDR(tmpId), (uint8_t*)&headerRst, sizeof(BlockHeader_t));
        faultTimes += assertBlockErase(tmpId);
        faultTimes += defaultHeaderInit(tmpId);
    }
#else
    do {
        flFUNC_REFRESH_CALLBACK();
        faultTimes += assertRead(lOG_GET_HEADER_ADDR(tmpId), (uint8_t*)&headerRst, sizeof(BlockHeader_t));
        faultTimes += assertBlockErase(tmpId);
        faultTimes += defaultHeaderInit(tmpId);
        tmpId = headerRst.linkBlockId;
    } while (headerRst.isLinked == true);
#endif

    lOG.startIdRead = lOG_START_BLOCK_ID;
    lOG.currIdUsed = lOG_START_BLOCK_ID;
    lOG.nbrOfBlockUsed = 1;

    if (faultTimes != 0) {
        lOG.isFault = true;
        return lOG_ERASE_FAILED;
    }

    return lOG_NOERROR;
}

void lOG_Print(int16_t bId) {
    BlockHeader_t header;

    assertRead(GET_FLASH_BLOCK_START_ADDR(bId), (uint8_t*)&header, sizeof(BlockHeader_t));
    lOG_PRINT(TAG, "[lOGP] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                header.blockId,
                header.linkBlockId,
                header.blockSize,
                header.mallocAddr,
                header.isLinked,
                header.lOGCount,
                header.lifeCycle
            );
}

/* Static function implementation ---------------------------------------------*/
bool defaultHeaderInit(int16_t bId) {
    bool isNG = false;
    BlockHeader_t headerInit;

    memset(&headerInit, 0, sizeof(BlockHeader_t));
    isNG = assertBlockErase(bId);

    headerInit.magicNum    = lOG_HEADER_SETTING_MAGIC_NUM;
    headerInit.blockSize   = LOG_BLOCK_DATA_SIZE;
    headerInit.mallocAddr  = lOG_GET_START_DATA_ADDR(bId);
    headerInit.blockId     = bId;
    headerInit.lOGCount    = 0;
    headerInit.isLinked    = false;
    headerInit.lifeCycle   = 0;
    if (headerInit.blockId != lOG_END_BLOCK_ID) {
        headerInit.linkBlockId = headerInit.blockId + 1;
    }
    else {
        headerInit.linkBlockId = lOG_START_BLOCK_ID;
    }
    
    isNG = headerUpdate(bId, &headerInit);

#if (lOG_MONITOR_EN == 1)
    memset(&headerInit, 0, sizeof(BlockHeader_t));
    flashRead(lOG_GET_HEADER_ADDR(bId), (uint8_t*)&headerInit, sizeof(BlockHeader_t));
    lOG_PRINT(TAG, "[InitDefault] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d,  lifeCycle: %d, lOGCount: %d",
            headerInit.blockId,
            headerInit.linkBlockId,
            headerInit.blockSize,
            headerInit.mallocAddr,
            headerInit.isLinked,
            headerInit.lifeCycle,
            headerInit.lOGCount
        );
#endif

    return isNG;
}

bool headerUpdate(int16_t bId, BlockHeader_t *new) {
    uint8_t isNG = true;

    isNG = assertSectorErase(bId, SECTOR_ID_STORAGE_HEADER_INFO);
    isNG = assertWrite(lOG_GET_HEADER_ADDR(bId), (uint8_t*)new, sizeof(BlockHeader_t));

    return isNG;
}

/* Assertion function implementation --------------------------------------------*/
bool assertWrite(uint32_t addr, uint8_t *buf, uint32_t len) {
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashWrite(addr, buf, len);

        if (__ret == FLASH_OK) {
            return false;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    lOG.isFault = true;
    return true;
}

bool assertRead(uint32_t addr, uint8_t *buf, uint32_t len) {
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashRead(addr, buf, len);

        if (__ret == FLASH_OK) {
            return false;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    lOG.isFault = true;
    return true;
}

bool assertBlockErase(int16_t bId) {
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashEraseBlock64k((GET_FLASH_BLOCK_START_ADDR(bId)));

        if (__ret == FLASH_OK) {
            return false;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    lOG.isFault = true;
    return true;
}

bool assertSectorErase(int16_t bId, uint8_t seId) {
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashEraseSector(GET_FLASH_SECTOR_START_ADDR(bId, seId));

        if (__ret == FLASH_OK) {
            return false;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    lOG.isFault = true;
    return true;
}