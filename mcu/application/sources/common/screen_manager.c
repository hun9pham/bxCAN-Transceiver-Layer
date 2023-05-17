#include "screen_manager.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void screenManagerInit(screenManager_t *me, screenAttr_t *listScreen, uint8_t scrEntryId, void *sig) {
    me->listScreen = listScreen;
    me->listScreen[scrEntryId].scrNext = NULL;
    me->stackPointer = &me->listScreen[scrEntryId];

    me->stackPointer->renderCb(sig);
}

void screenGoTo(screenManager_t *me, uint8_t srcId, void *sig) {
    me->listScreen[srcId].scrNext = me->stackPointer;
    me->stackPointer = &me->listScreen[srcId];

    me->stackPointer->renderCb(sig);
}

void screenBack(screenManager_t *me, void *sig) {
    if (me->stackPointer->scrNext != NULL) {
        me->stackPointer = me->stackPointer->scrNext;
        me->stackPointer->renderCb(sig);
    }
}
