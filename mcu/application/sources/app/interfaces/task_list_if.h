#ifndef __TASK_LIST_IF_H
#define __TASK_LIST_IF_H

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


#endif /* __TASK_LIST_IF_H */
