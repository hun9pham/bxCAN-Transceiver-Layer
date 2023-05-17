#include "timer.h"
#include "task_list.h"

ak_task_t task_list[] = {
    {	MT_TASK_TIMER_ID,			TASK_PRI_LEVEL_1,	TaskTimerEntry			,	&timerMailbox				,	"APPLICATION TIMER SERVICE"		},
    {	MT_TASK_IF_ID,				TASK_PRI_LEVEL_1,	TaskIfEntry				,	&taskIfMailbox				,	"MASTER INTERFACE"				},
	{	MT_TASK_SYSTEM_ID,			TASK_PRI_LEVEL_1,	TaskSystemEntry			,	&taskSystemMailbox			,	"SYSTEM MANAGEMENT"				},

	{	MT_TASK_CAN_IF_ID,			TASK_PRI_LEVEL_1,	TaskCanIfEntry			,	&taskCanIfMailbox			,	"CAN INTERFACE"					},
	{	MT_TASK_CAN_HIGH_LAYER_ID,	TASK_PRI_LEVEL_2,	TaskCanHighLayerEntry	,	&taskCanHighLayerMailbox	,	"CAN HIGH LAYER"				},
	{	MT_TASK_CAN_LOW_LAYER_ID,	TASK_PRI_LEVEL_1,	TaskCanLowLayerEntry	,	&taskCanLowLayerMailbox		,	"CAN LOW LAYER"					},
};
