#ifndef __TASK_SYSTEM_H
#define __TASK_SYSTEM_H

#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Enumarics -----------------------------------------------------------------*/
enum eCsOption {
    COMMAND_LIST = 0,
    CAN_SEND_PURE_MSG_TO_01_02,
    CAN_SEND_COMMON_MSG_TO_01_02,
    CAN_SEND_DYNAMIC_MSG_TO_01_02,
    CAN_PERIODIC_SEND_DYNAMIC_MSG_TO_01_02,
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskSystemMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskSystemEntry(void*);

#endif /* __TASK_SYSTEM_H */
