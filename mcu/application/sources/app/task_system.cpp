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
#include "task_system.h"
#include "can_data.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskSystem"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void CanLayerTest();

/* Function implementation ---------------------------------------------------*/
void TaskSystem(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_SYSTEM_KEEP_ALIVE: {
		ledLifeBlink();
		watchdogRst();
	}
	break;

	case SL_SYSTEM_REBOOT_REQ: {
		softReset();
	}
	break;

	case SL_SYSTEM_CANBUS_PURE: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_CANBUS_PURE");
	}
	break;

	case SL_SYSTEM_CANBUS_COMMON: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_CANBUS_COMMON");
		uint8_t *data = get_data_common_msg(msg);
		APP_PRINT("%s\r\n", data);
	}
	break;

	case SL_SYSTEM_CANBUS_DYNAMIC: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_CANBUS_DYNAMIC");
		uint8_t *data = get_data_dynamic_msg(msg);
		APP_PRINT("%s\r\n", data);
	}
	break;

	default:
	break;
	}
}

#pragma GCC diagnostic ignored "-Wunused-function"
void CanLayerTest() {
#if 0
	ak_msg_t *pureMsg = get_pure_msg();
	set_if_src_task_id(pureMsg, SL_TASK_CONSOLE_ID);
	set_if_des_task_id(pureMsg, SL_TASK_SYSTEM_ID);
	set_if_src_type(pureMsg, IF_TYPE_CAN_BUS_SL);
	set_if_des_type(pureMsg, IF_TYPE_CAN_BUS_MT);
	set_if_sig(pureMsg, SL_SYSTEM_CANBUS_PURE);
	set_msg_src_task_id(pureMsg, SL_TASK_CONSOLE_ID);
	set_msg_sig(pureMsg, SL_IF_PURE_MSG_OUT);
	task_post(SL_TASK_IF_ID, pureMsg);
	CanIdWaitForTrans.enQueue(0x0000);

	const char *comStr = (const char*)"HungPNQ";
	ak_msg_t *comMsg = get_common_msg();
	set_if_src_task_id(comMsg, SL_TASK_CONSOLE_ID);
	set_if_des_task_id(comMsg, SL_TASK_SYSTEM_ID);
	set_if_src_type(comMsg, IF_TYPE_CAN_BUS_SL);
	set_if_des_type(comMsg, IF_TYPE_CAN_BUS_MT);
	set_if_sig(comMsg, SL_SYSTEM_CANBUS_COMMON);
	set_msg_src_task_id(comMsg, SL_TASK_CONSOLE_ID);
	set_msg_sig(comMsg, SL_IF_COMMON_MSG_OUT);
	set_if_data_common_msg(comMsg, (uint8_t*)comStr, strlen(comStr));
	task_post(SL_TASK_IF_ID, comMsg);
	CanIdWaitForTrans.enQueue(0x0000);

	const char *dynStr ="\r\n"                                                                           
						"    _/    _/  _/_/_/_/  _/        _/          _/_/        _/          _/    _/_/    _/_/_/    _/        _/_/_/    \r\n"
						"   _/    _/  _/        _/        _/        _/    _/      _/          _/  _/    _/  _/    _/  _/        _/    _/   \r\n"
						"  _/_/_/_/  _/_/_/    _/        _/        _/    _/      _/    _/    _/  _/    _/  _/_/_/    _/        _/    _/    \r\n"
						" _/    _/  _/        _/        _/        _/    _/        _/  _/  _/    _/    _/  _/    _/  _/        _/    _/     \r\n"
						"_/    _/  _/_/_/_/  _/_/_/_/  _/_/_/_/    _/_/            _/  _/        _/_/    _/    _/  _/_/_/_/  _/_/_/        \r\n"
						"\0";
	ak_msg_t *dynMsg = get_dynamic_msg();
	set_if_src_task_id(dynMsg, SL_TASK_CONSOLE_ID);
	set_if_des_task_id(dynMsg, SL_TASK_SYSTEM_ID);
	set_if_src_type(dynMsg, IF_TYPE_CAN_BUS_SL);
	set_if_des_type(dynMsg, IF_TYPE_CAN_BUS_MT);
	set_if_sig(dynMsg, SL_SYSTEM_CANBUS_DYNAMIC);
	set_msg_src_task_id(dynMsg, SL_TASK_CONSOLE_ID);
	set_msg_sig(dynMsg, SL_IF_DYNAMIC_MSG_OUT);
	set_if_data_dynamic_msg(dynMsg, (uint8_t*)dynStr, strlen(dynStr));
	task_post(SL_TASK_IF_ID, dynMsg);
	CanIdWaitForTrans.enQueue(0x0000);
#endif
}