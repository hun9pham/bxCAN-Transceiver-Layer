#include "task_list.h"
#include "timer.h"

/* Extern variables ------------------------------------------------------------*/
const task_t app_task_table[] = {
	/*--------------------------------------------------------------------------*/
	/*                              SYSTEM TASK                                 */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_TIMER_TICK_ID		,	TASK_PRI_LEVEL_7	,	task_timer_tick		},

	/*--------------------------------------------------------------------------*/
	/*                              APP TASK                                    */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_CONSOLE_ID			,	TASK_PRI_LEVEL_3	,	TaskConsole			},
	{SL_TASK_SYSTEM_ID			,	TASK_PRI_LEVEL_6	,	TaskSystem			},
	{SL_TASK_SM_ID				,	TASK_PRI_LEVEL_3	,	TaskSm				},
	{SL_TASK_IF_ID				,	TASK_PRI_LEVEL_4	,	TaskIf				},

#ifdef IF_LINK_UART_EN
	{SL_TASK_CPU_SERIAL_IF_ID	,	TASK_PRI_LEVEL_4	,	TaskCpuSerialIf		},
#endif

#ifdef IF_CAN_BUS_EN
	{SL_TASK_CAN_IF_ID			,	TASK_PRI_LEVEL_4	,	TaskCanIf			},
	{SL_TASK_CAN_HIGH_LAYER_ID	,	TASK_PRI_LEVEL_5	,	TaskCanHighLayer	},
	{SL_TASK_CAN_LOW_LAYER_ID	,	TASK_PRI_LEVEL_3	,	TaskCanLowLayer		},
#endif
#ifdef IF_LINK_UART_EN
	{SL_LINK_PHY_ID				,	TASK_PRI_LEVEL_3	,	TaskLinkPhy			},
	{SL_LINK_MAC_ID				,	TASK_PRI_LEVEL_4	,	TaskLinkMac			},
	{SL_LINK_ID					,	TASK_PRI_LEVEL_5	,	TaskLink			},
#endif

	/*--------------------------------------------------------------------------*/
	/*                            END OF TABLE                                  */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_EOT_ID				,	TASK_PRI_LEVEL_0	,	(pf_task)0			}
};

task_polling_t app_task_polling_table[] = {
	/*--------------------------------------------------------------------------*/
	/*                              APP TASK                                    */
	/*--------------------------------------------------------------------------*/
    {SL_TASK_POLL_CONSOLE_ID	,	AK_ENABLE			,	TaskPollConsole     },

#ifdef IF_CAN_BUS_EN
	{SL_TASK_POLL_CANBUS_ID		,	AK_DISABLE			,	TaskPollCanBus	    },
#endif
#ifdef IF_LINK_UART_EN
	{SL_TASK_POLL_CPU_SERIAL_ID	,	AK_DISABLE			,	TaskPollCpuSerialIf },
#endif

	/*--------------------------------------------------------------------------*/
	/*                            END OF TABLE                                  */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_POLLING_EOT_ID		,	AK_DISABLE			,	(pf_task_polling)0	},
};
