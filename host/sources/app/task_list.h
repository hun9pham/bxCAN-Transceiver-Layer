#ifndef __TASK_LIST_H
#define __TASK_LIST_H

#include "ak.h"
#include "message.h"

#include "app.h"
#include "app_data.h"

#include "task_if.h"
#include "task_system.h"

#include "task_can_if.h"
#include "can_low_layer.h"
#include "can_high_layer.h"

/*---------------------------------------------------------------------------*
 *  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
    /* SYSTEM TASKS */
    MT_TASK_TIMER_ID,

    /* APP TASKS */
    MT_TASK_IF_ID,
    MT_TASK_SYSTEM_ID,

    /* CANBUS */
    MT_TASK_CAN_IF_ID,
    MT_TASK_CAN_HIGH_LAYER_ID,
	MT_TASK_CAN_LOW_LAYER_ID,

    /* EOT task ID */
    MT_TASK_LIST_LEN
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern ak_task_t task_list[];

/* Function prototypes -------------------------------------------------------*/


#endif /* __TASK_LIST_H */
