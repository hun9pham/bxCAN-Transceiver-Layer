#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>

#include "ak.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_can_if.h"

#include "can_sig.h"
#include "can_low_layer.h"
#include "can_high_layer.h"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskCanIfMailbox;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void* TaskCanIfEntry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_CAN_IF_ID Entry\n");

	while (1) {
		msg = ak_msg_rev(MT_TASK_CAN_IF_ID);

		switch (msg->header->sig) {
		case MT_CAN_IF_PURE_MSG_OUT: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_CAN_LOW_SEND_PURE_MSG);
			task_post(MT_TASK_CAN_LOW_LAYER_ID, s_msg);
		}
			break;

		case MT_CAN_IF_COMMON_MSG_OUT: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_CAN_LOW_SEND_COMMON_MSG);
			task_post(MT_TASK_CAN_LOW_LAYER_ID, s_msg);
		}
			break;

		case MT_CAN_IF_DYNAMIC_MSG_OUT: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_CAN_LOW_SEND_DYNAMIC_MSG);
			task_post(MT_TASK_CAN_LOW_LAYER_ID, s_msg);
		}
			break;

		case MT_CAN_IF_PURE_MSG_IN: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_IF_PURE_MSG_IN);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		case MT_CAN_IF_COMMON_MSG_IN: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_IF_COMMON_MSG_IN);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		case MT_CAN_IF_DYNAMIC_MSG_IN: {
			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(s_msg, MT_IF_DYNAMIC_MSG_IN);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		default:
			break;
		}

		ak_msg_free(msg);
	}

	return (void*)0;
}
