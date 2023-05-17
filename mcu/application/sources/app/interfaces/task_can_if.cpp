#include <stdbool.h>

#include "ak.h"
#include "heap.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"
#include "task_list.h"
#include "task_can_if.h"

#include "can_sig.h"
#include "can_data.h"
#include "can_dbg.h"
#include "can_low_layer.h"
#include "can_high_layer.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskCanIf"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskCanIf(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_CAN_IF_INIT: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_INIT");

		bxCanInit(CANBUS_ID_OPTION);
		pf_txCanCb = bxCanSendData;
		task_polling_set_ability(SL_TASK_POLL_CANBUS_ID, AK_ENABLE);
		task_post_pure_msg(SL_TASK_CAN_LOW_LAYER_ID, SL_CAN_LOW_LAYER_INIT);
	}
	break;

	case SL_CAN_IF_PURE_MSG_OUT: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_PURE_MSG_OUT");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_CAN_LOW_SEND_PURE_MSG);
		task_post(SL_TASK_CAN_LOW_LAYER_ID, msg);
	}
	break;

	case SL_CAN_IF_COMMON_MSG_OUT: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_COMMON_MSG_OUT");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_CAN_LOW_SEND_COMMON_MSG);
		task_post(SL_TASK_CAN_LOW_LAYER_ID, msg);
	}
	break;

	case SL_CAN_IF_DYNAMIC_MSG_OUT: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_DYNAMIC_MSG_OUT");
		
		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_CAN_LOW_SEND_DYNAMIC_MSG);
		task_post(SL_TASK_CAN_LOW_LAYER_ID, msg);
	}
	break;

	case SL_CAN_IF_PURE_MSG_IN: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_PURE_MSG_IN");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_PURE_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

    case SL_CAN_IF_COMMON_MSG_IN: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_COMMON_MSG_IN");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_PURE_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

    case SL_CAN_IF_DYNAMIC_MSG_IN: {
		CAN_DBG_SIG(TAG, "SL_CAN_IF_DYNAMIC_MSG_IN");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_PURE_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

	default:
	break;
	}
}

void TaskPollCanBus() {
	uint8_t byte;

    while (!ring_buffer_char_is_empty(&canFrameRx)) {
        ENTRY_CRITICAL();
        byte = ring_buffer_char_get(&canFrameRx);
        EXIT_CRITICAL();

		CanParserByteRecv(byte);
    }
}
