#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_if.h"

#define TAG		"TaskIf"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskIf(ak_msg_t* msg) {
	if (msg->if_des_type == IF_TYPE_CPU_SERIAL_MT ||
		msg->if_des_type == IF_TYPE_CPU_SERIAL_SL)
	{	
#ifdef IF_LINK_UART_EN
		switch (msg->sig) {
		case SL_IF_PURE_MSG_IN:	{
			APP_DBG_SIG(TAG, "SL_IF_PURE_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_COMMON_MSG_IN: {
			APP_DBG_SIG(TAG, "SL_IF_COMMON_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_DYNAMIC_MSG_IN: {
			APP_DBG_SIG(TAG, "SL_IF_DYNAMIC_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_PURE_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_PURE_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CPU_SERIAL_IF_PURE_MSG_OUT);
			task_post(SL_TASK_CPU_SERIAL_IF_ID, msg);
		}
		break;

		case SL_IF_COMMON_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_COMMON_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CPU_SERIAL_IF_COMMON_MSG_OUT);
			task_post(SL_TASK_CPU_SERIAL_IF_ID, msg);
		}
		break;

		case SL_IF_DYNAMIC_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_DYNAMIC_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CPU_SERIAL_IF_DYNAMIC_MSG_OUT);
			task_post(SL_TASK_CPU_SERIAL_IF_ID, msg);
		}
		break;

		default:
		break;
		}
#endif
	}
	else if (msg->if_des_type == IF_TYPE_CAN_BUS_MT ||
			 msg->if_des_type == IF_TYPE_CAN_BUS_SL)
	{
#ifdef IF_CAN_BUS_EN
		switch (msg->sig) {
		case SL_IF_PURE_MSG_IN:	{
			APP_DBG_SIG(TAG, "SL_IF_PURE_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_COMMON_MSG_IN: {
			APP_DBG_SIG(TAG, "SL_IF_COMMON_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_DYNAMIC_MSG_IN: {
			APP_DBG_SIG(TAG, "SL_IF_DYNAMIC_MSG_IN");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, msg->if_sig);
			set_msg_src_task_id(msg, msg->if_src_task_id);
			task_post(msg->if_des_task_id, msg);
		}
		break;

		case SL_IF_PURE_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_PURE_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CAN_IF_PURE_MSG_OUT);
			task_post(SL_TASK_CAN_IF_ID, msg);
		}
		break;

		case SL_IF_COMMON_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_COMMON_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CAN_IF_COMMON_MSG_OUT);
			task_post(SL_TASK_CAN_IF_ID, msg);
		}
		break;

		case SL_IF_DYNAMIC_MSG_OUT: {
			APP_DBG_SIG(TAG, "SL_IF_DYNAMIC_MSG_OUT");

			msg_inc_ref_count(msg);
			set_msg_sig(msg, SL_CAN_IF_DYNAMIC_MSG_OUT);
			task_post(SL_TASK_CAN_IF_ID, msg);
		}
		break;

		default:
		break;
		}
#endif
	}
}
