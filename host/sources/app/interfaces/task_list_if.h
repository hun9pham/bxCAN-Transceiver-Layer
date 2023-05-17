#ifndef __TASK_LIST_IF_H
#define __TASK_LIST_IF_H

/*---------------------------------------------------------------------------*
 *  DECLARE: SLAVE Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
#define IF_CAN_BUS_EN

enum {
	/* SYSTEM TASKS */
	SL_TASK_TIMER_TICK_ID,

	/* APP TASKS */
	SL_TASK_CONSOLE_ID,
	SL_TASK_SYSTEM_ID,
	SL_TASK_SM_ID,
	SL_TASK_IF_ID,
#ifdef IF_LINK_UART_EN
	SL_TASK_CPU_SERIAL_IF_ID,
#endif
#ifdef IF_CAN_BUS_EN
	SL_TASK_CAN_IF_ID,
	SL_TASK_CAN_HIGH_LAYER_ID,
	SL_TASK_CAN_LOW_LAYER_ID,
#endif
#ifdef IF_LINK_UART_EN
	SL_TASK_CPU_SERIAL_IF_ID,
	SL_LINK_PHY_ID,
	SL_LINK_MAC_ID,
	SL_LINK_ID,
#endif

	/* EOT task ID */
	SL_TASK_EOT_ID,
};
/*---------------------------------------------------------------------------*/

#endif /* __TASK_LIST_IF_H */
