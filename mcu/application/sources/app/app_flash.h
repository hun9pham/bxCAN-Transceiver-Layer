#ifndef __APP_FLASH_H
#define __APP_FLASH_H

#include <stdint.h>

#include "flash.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FLASH_SECTOR_SETTING_CONFIGURE      GET_FLASH_SECTOR_START_ADDR(0, 0)

#define FLASH_FIRMWARE_FACTORY_ADDR         GET_FLASH_BLOCK_START_ADDR(14)
#define FLASH_FIRMWARE_OTA_ADDR             GET_FLASH_BLOCK_START_ADDR(10)

/* Enumarics -----------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

#endif /* __APP_FLASH_H */
