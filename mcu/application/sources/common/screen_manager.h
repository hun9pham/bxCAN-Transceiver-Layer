//-----------------------------------------------------------------------------
// Project   :  Screen manager design pattern
// Author    :  HungPNQ
// Date      :  27/5/2023
//-----------------------------------------------------------------------------

#ifndef __SCREEN_MANAGER_H
#define __SCREEN_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/
typedef void (*pf_scrRender)(void*);

typedef struct t_ScreenAttribute {
    uint8_t scrId;
    pf_scrRender renderCb;
    struct t_ScreenAttribute *scrNext;
} screenAttr_t;

typedef struct {
    screenAttr_t *listScreen;
    screenAttr_t *stackPointer;
} screenManager_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void screenManagerInit(screenManager_t *me, screenAttr_t *listScreen, uint8_t scrEntryId, void *sig);
extern void screenGoTo(screenManager_t *me, uint8_t srcId, void *sig);
extern void screenBack(screenManager_t *me, void *sig);

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN_MANAGER_H */

/*

BOTTOM  ->  SCREEN_A    ->  SCREEN_B    ->  SCREEN_C
                                            StackPointer
*/