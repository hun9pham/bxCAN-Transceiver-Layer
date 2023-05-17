#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/un.h>

#include "ak.h"

#include "firmware.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_list_if.h"
#include "task_system.h"
#include "can_data.h"

#define TAG	"TaskSystem"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskSystemMailbox;

/* Private variables ---------------------------------------------------------*/
static pthread_t csInterfaceThreadId;
static const char *dynStr ="\r\n"                                                                           
					"    _/    _/  _/_/_/_/  _/        _/          _/_/        _/          _/    _/_/    _/_/_/    _/        _/_/_/    \r\n"
					"   _/    _/  _/        _/        _/        _/    _/      _/          _/  _/    _/  _/    _/  _/        _/    _/   \r\n"
					"  _/_/_/_/  _/_/_/    _/        _/        _/    _/      _/    _/    _/  _/    _/  _/_/_/    _/        _/    _/    \r\n"
					" _/    _/  _/        _/        _/        _/    _/        _/  _/  _/    _/    _/  _/    _/  _/        _/    _/     \r\n"
					"_/    _/  _/_/_/_/  _/_/_/_/  _/_/_/_/    _/_/            _/  _/        _/_/    _/    _/  _/_/_/_/  _/_/_/        \r\n"
					;

/* Private function prototypes -----------------------------------------------*/
static void parserInputCs(uint8_t num);
static void* csInterfaceCb(void* argv);

/* Function implementation ---------------------------------------------------*/
void* TaskSystemEntry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_SYSTEM_ID Entry\n");

	pthread_create(&csInterfaceThreadId, NULL, csInterfaceCb, NULL);
	task_post_pure_msg(MT_TASK_SYSTEM_ID, MT_SYSTEM_GATEWAY_ONLINE_ENTRY);

	while (1) {
		msg = ak_msg_rev(MT_TASK_SYSTEM_ID);

		switch (msg->header->sig) {
        case MT_SYSTEM_GATEWAY_ONLINE_ENTRY: {
            APP_DBG_SIG("MT_SYSTEM_GATEWAY_ONLINE_ENTRY\n");
			
			
        }
        break;

		case MT_SYSTEM_SEND_DYNAMIC_VIA_CANBUS: {			
			static bool isChange = true;

			ak_msg_t* if_msg = get_dynamic_msg();
			set_if_src_task_id(if_msg, MT_TASK_SYSTEM_ID);
			set_if_des_task_id(if_msg, SL_TASK_SYSTEM_ID);
			set_if_src_type(if_msg, IF_TYPE_CAN_BUS_MT);
			set_if_des_type(if_msg, IF_TYPE_CAN_BUS_SL);
			set_if_sig(if_msg, SL_SYSTEM_CANBUS_DYNAMIC);
			set_if_data_dynamic_msg(if_msg, (uint8_t*)dynStr, strlen(dynStr));

			set_msg_sig(if_msg, MT_CAN_IF_DYNAMIC_MSG_OUT);
			task_post(MT_TASK_CAN_IF_ID, if_msg);

			if (isChange) {
				CanIdWaitForTrans.enQueue(0x01);
			}
			else {
				CanIdWaitForTrans.enQueue(0x02);
			}
			isChange = !isChange;
		}
		break;

		case MT_SYSTEM_RECV_DYNAMIC_VIA_CANBUS: {
			APP_DBG_SIG("MT_SYSTEM_RECV_DYNAMIC_VIA_CANBUS\n");

			uint8_t *dynSt = (uint8_t*)msg->header->payload;
			APP_PRINT("%s", dynSt);
		}
		break;

		default:
        break;
		}
		
		ak_msg_free(msg);
	}

	return (void*)0;
}

static void* csInterfaceCb(void* argv) {
	char ch;

	for (;;) {
		do {
			ch = getchar();

			if (ch != '\r' && ch != '\n') {
				uint8_t num = atoi(&ch);
				parserInputCs(num);					
							
			}
			else {
				parserInputCs(COMMAND_LIST);
			}

			usleep(100);

		} while (ch != '\n');

		usleep(100000);
	}

	return (void*)0;
}

void parserInputCs(uint8_t num) {
	APP_PRINT("[SELECTED]: %d\n", num);

	switch (num) {
	case COMMAND_LIST: {
		APP_PRINT("+ [%d]. CAN_SEND_PURE_MSG_TO_01_02\r\n", CAN_SEND_PURE_MSG_TO_01_02);
		APP_PRINT("+ [%d]. CAN_SEND_COMMON_MSG_TO_01_02\r\n", CAN_SEND_COMMON_MSG_TO_01_02);
		APP_PRINT("+ [%d]. CAN_SEND_DYNAMIC_MSG_TO_01_02\r\n", CAN_SEND_DYNAMIC_MSG_TO_01_02);
		APP_PRINT("+ [%d]. CAN_PERIODIC_SEND_DYNAMIC_MSG_TO_01_02\r\n", CAN_PERIODIC_SEND_DYNAMIC_MSG_TO_01_02);
	}
	break;

	case CAN_SEND_PURE_MSG_TO_01_02: {
		ak_msg_t* if_msg_0x01 = get_pure_msg();
		set_if_src_task_id(if_msg_0x01, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x01, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x01, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x01, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x01, SL_SYSTEM_CANBUS_PURE);

		set_msg_sig(if_msg_0x01, MT_CAN_IF_PURE_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x01);
		CanIdWaitForTrans.enQueue(0x01);

		ak_msg_t* if_msg_0x02 = get_pure_msg();
		set_if_src_task_id(if_msg_0x02, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x02, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x02, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x02, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x02, SL_SYSTEM_CANBUS_PURE);

		set_msg_sig(if_msg_0x02, MT_CAN_IF_PURE_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x02);
		CanIdWaitForTrans.enQueue(0x02);
	}
	break;

	case CAN_SEND_COMMON_MSG_TO_01_02: {
		const char *comStr = (const char*)"HungPNQ";
		
		ak_msg_t* if_msg_0x01 = get_common_msg();
		set_if_src_task_id(if_msg_0x01, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x01, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x01, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x01, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x01, SL_SYSTEM_CANBUS_COMMON);
		set_if_data_common_msg(if_msg_0x01, (uint8_t*)comStr, strlen(comStr));

		set_msg_sig(if_msg_0x01, MT_CAN_IF_COMMON_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x01);
		CanIdWaitForTrans.enQueue(0x01);

		ak_msg_t* if_msg_0x02 = get_common_msg();
		set_if_src_task_id(if_msg_0x02, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x02, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x02, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x02, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x02, SL_SYSTEM_CANBUS_COMMON);
		set_if_data_common_msg(if_msg_0x02, (uint8_t*)comStr, strlen(comStr));

		set_msg_sig(if_msg_0x02, MT_CAN_IF_COMMON_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x02);
		CanIdWaitForTrans.enQueue(0x02);
	}
	break;

	case CAN_SEND_DYNAMIC_MSG_TO_01_02: {			
		ak_msg_t* if_msg_0x01 = get_dynamic_msg();
		set_if_src_task_id(if_msg_0x01, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x01, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x01, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x01, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x01, SL_SYSTEM_CANBUS_DYNAMIC);
		set_if_data_dynamic_msg(if_msg_0x01, (uint8_t*)dynStr, strlen(dynStr));

		set_msg_sig(if_msg_0x01, MT_CAN_IF_DYNAMIC_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x01);
		CanIdWaitForTrans.enQueue(0x01);

		ak_msg_t* if_msg_0x02 = get_dynamic_msg();
		set_if_src_task_id(if_msg_0x02, MT_TASK_SYSTEM_ID);
		set_if_des_task_id(if_msg_0x02, SL_TASK_SYSTEM_ID);
		set_if_src_type(if_msg_0x02, IF_TYPE_CAN_BUS_MT);
		set_if_des_type(if_msg_0x02, IF_TYPE_CAN_BUS_SL);
		set_if_sig(if_msg_0x02, SL_SYSTEM_CANBUS_DYNAMIC);
		set_if_data_dynamic_msg(if_msg_0x02, (uint8_t*)dynStr, strlen(dynStr));

		set_msg_sig(if_msg_0x02, MT_CAN_IF_DYNAMIC_MSG_OUT);
		task_post(MT_TASK_CAN_IF_ID, if_msg_0x02);
		CanIdWaitForTrans.enQueue(0x02);
	}
	break;

	case CAN_PERIODIC_SEND_DYNAMIC_MSG_TO_01_02: {
		static bool boolean = true;
		if (boolean) {
			timer_set(MT_TASK_SYSTEM_ID, MT_SYSTEM_SEND_DYNAMIC_VIA_CANBUS, 1000, TIMER_PERIODIC);
		}
		else {
			timer_remove_attr(MT_TASK_SYSTEM_ID, MT_SYSTEM_SEND_DYNAMIC_VIA_CANBUS);
		}
		boolean = !boolean;
	}
	break;

	default: {
		APP_PRINT("?????\n");
	}
	break;
	}
}
