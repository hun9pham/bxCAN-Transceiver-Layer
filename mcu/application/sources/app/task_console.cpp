#include <stdbool.h>

#include "ak.h"
#include "heap.h"
#include "task.h"
#include "message.h"
#include "timer.h"

#include "cmd_line.h"
#include "ring_buffer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"
#include "task_list.h"
#include "task_list_if.h"
#include "task_console.h"
#include "can_data.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"
#include "sys_boot.h"

#define TAG	"TaskConsole"

/* Extern variables ----------------------------------------------------------*/
extern sysStructInfo_t sysStructInfo;

/* Private function prototypes -----------------------------------------------*/
static int8_t csInfo(uint8_t* argv);
static int8_t csHelp(uint8_t* argv);
static int8_t csRst(uint8_t* argv);
static int8_t csFatal(uint8_t* argv);
static int8_t csMake(uint8_t* argv);

static cmdLineStruct_t lgnCmdTable[] = {
	/*------------------------------------------------------------------------------*/
	/*									System command								*/
	/*------------------------------------------------------------------------------*/
	{(const int8_t*)"info",		csInfo,	    (const int8_t*)"System information"		},
	{(const int8_t*)"help",		csHelp,		(const int8_t*)"Help information"		},
	{(const int8_t*)"rst",		csRst,		(const int8_t*)"Reset system"			},
	{(const int8_t*)"fatal"	,	csFatal,	(const int8_t*)"Fatal information"		},
	{(const int8_t*)"make"	,	csMake,		(const int8_t*)"Devices test"	 		},
	/*------------------------------------------------------------------------------*/
	/*									End of table								*/
	/*------------------------------------------------------------------------------*/
	{(const int8_t*)0,			(pfCmdFunc)0,			(const int8_t*)0			}
};

/* Private variables ----------------------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskConsole(ak_msg_t *msg) {
	switch (msg->sig) {
	case SL_CONSOLE_HANDLE_CMD_LINE: {
		switch (cmdLineParser(lgnCmdTable, ' ')) {
		case CMD_SUCCESS:
		break;
		case CMD_NOT_FOUND:
		break;
		case CMD_TOO_LONG:
		break;
		case CMD_TBL_NOT_FOUND:
		break;
		default:
		break;
		}
	}
	break;

	default:
	break;
	}
}

void TaskPollConsole(void) {
	extern ring_buffer_char_t systemConsoleRx;

	uint8_t ch;
	while (!ring_buffer_char_is_empty(&systemConsoleRx)) {
		ch = ring_buffer_char_get(&systemConsoleRx);

		if (ch == '\r' || ch == '\n') {
			APP_PRINT("\r\n");
		
			task_post_pure_msg(SL_TASK_CONSOLE_ID, SL_CONSOLE_HANDLE_CMD_LINE);
			APP_PRINT("- ");
		}
		else if (ch == 8 || ch == 127) { /* Backspace & Delelte */
			cmdLineClrChar();
		}
		else {
			cmdLinePutChar(ch);
		}
	}
}

/*----------------------------------------------------------------------------*/
int8_t csInfo(uint8_t* argv) {
	(void)argv;
	
	switch(getCmdLineParserCounter()) {
	case 1: {
		if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"heap") == 0) {
			APP_PRINT("\n[HEAP] Size: %d, used: %d, free: %d\n", getTotalHeapSize(), getTotalHeapUsed(),  getTotalHeapFree());
		}
	}
	break;

	default: {
		APP_PRINT("\n[SYSTEM INFORMATION]\n");
		APP_PRINT("CPU clock:\t\t%d Hz\n", 			sysStructInfo.CpuClk);
		APP_PRINT("Time tick:\t\t%d ms\n", 			sysStructInfo.Tick);
		APP_PRINT("Baudrate:\t\t%d bps\n", 			sysStructInfo.Baudrate);
		APP_PRINT("Flash used:\t\t%d bytes\n", 		sysStructInfo.flashUsed);
		APP_PRINT("Sram used:\t\t%d bytes\n", 		sysStructInfo.sramUsed);
		APP_PRINT("\t.data:\t\t%d bytes\n", 			sysStructInfo.dataUsed);
		APP_PRINT("\t.bss:\t\t%d bytes\n", 			sysStructInfo.bssUsed);
		APP_PRINT("\tStack avaiable:\t%d bytes\n", 	sysStructInfo.stackRemain);
		APP_PRINT("\tHeap length:\t%d bytes\n", 		sysStructInfo.heapSize);
		APP_PRINT("\n");
	}
	break;
	}
	
	return 0;
}	

int8_t csHelp(uint8_t* argv) {	
	APP_PRINT("\r\nHelp commands:\r\n");
	for (uint8_t id = 0; id < sizeof(lgnCmdTable) / sizeof(lgnCmdTable[0]) - 1; ++id) {
		APP_PRINT("  -%s: %s\r\n", lgnCmdTable[id].cmd, lgnCmdTable[id].Str);
	}

	return 0;
}

int8_t csRst(uint8_t* argv) {
	(void)argv;
	softReset();

	return 0;
}

int8_t csFatal(uint8_t* argv) {
	switch (*(argv + 6)) {
	case 't': {
		FATAL("TEST", 0x01);
	}
	break;

	case 'r': {
		if (fatalClear()) {
			APP_PRINT("Fatal clear\n");
		}
	}
	break;

	case 'l': {
		fatalLog_t *fatalLogCs = fatalRead();

		APP_PRINT("\r\n");
		APP_PRINT("[Times] Fatal:   %d\r\n", fatalLogCs->fatalTimes);
		APP_PRINT("[Times] Restart: %d\r\n", fatalLogCs->restartTimes);

		APP_PRINT("\r\n");
		APP_PRINT("[FATAL] Type: %s\r\n", fatalLogCs->str);
		APP_PRINT("[FATAL] Code: 0x%02X\r\n", fatalLogCs->code);

		APP_PRINT("\r\n");
		APP_PRINT("Task\r\n");
		APP_PRINT("Id: %d, Prio: %d\r\n", fatalLogCs->currentTaskId, fatalLogCs->currentTaskPrio);

		APP_PRINT("\r\n");
		APP_PRINT("Message\r\n");
		APP_PRINT("[Obj] Task: %d\r\n", fatalLogCs->desTaskId);
		APP_PRINT("[Obj] Sig: %d\r\n", fatalLogCs->sig);
		APP_PRINT("[Obj] Type: 0x%x\r\n", fatalLogCs->type);
		APP_PRINT("[Obj] Ref_count: %d\r\n", fatalLogCs->refCount);
		APP_PRINT("\n");

		APP_PRINT("[RST] Reason: %s\n",	fatalLogCs->reasonRst);
		APP_PRINT("\n");
	}
	break;

	default: {
		APP_PRINT("\n<Fatal commands>\n");
		APP_PRINT("Usage:\n");
		APP_PRINT("  fatal [options]\n");
		APP_PRINT("Options:\n");
		APP_PRINT("  l: Fatal log\n");
		APP_PRINT("  r: Fatal clear\n\n");
	}
	break;
	}

	return 0;
}

int8_t csMake(uint8_t* argv) {
	switch(getCmdLineParserCounter()) {
	case 1: {
		if (*(argv + 5) == 'a') {
			ak_msg_t *if_msg = get_dynamic_msg();

			set_if_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_if_des_task_id(if_msg, MT_TASK_SYSTEM_ID);
			set_if_src_type(if_msg, IF_TYPE_CAN_BUS_SL);
			set_if_des_type(if_msg, IF_TYPE_CAN_BUS_MT);
			set_if_sig(if_msg, MT_SYSTEM_RECV_DYNAMIC_VIA_CANBUS);
			set_msg_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_msg_sig(if_msg, SL_IF_DYNAMIC_MSG_OUT);
			set_if_data_dynamic_msg(if_msg, (uint8_t*)hugeMsg, strlen(hugeMsg));
			task_post(SL_TASK_IF_ID, if_msg);
			CanIdWaitForTrans.enQueue(0x03);
		}
		else if (*(argv + 5) == 'b') {
			ak_msg_t *if_msg = get_dynamic_msg();

			set_if_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_if_des_task_id(if_msg, SL_TASK_SYSTEM_ID);
			set_if_src_type(if_msg, IF_TYPE_CAN_BUS_SL);
			set_if_des_type(if_msg, IF_TYPE_CAN_BUS_MT);
			set_if_sig(if_msg, SL_SYSTEM_CANBUS_DYNAMIC);
			set_msg_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_msg_sig(if_msg, SL_IF_DYNAMIC_MSG_OUT);
			set_if_data_dynamic_msg(if_msg, (uint8_t*)hugeMsg, strlen(hugeMsg));
			task_post(SL_TASK_IF_ID, if_msg);
			CanIdWaitForTrans.enQueue(0x01);
		}
		else if (*(argv + 5) == 'c') {
			ak_msg_t *if_msg = get_dynamic_msg();

			set_if_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_if_des_task_id(if_msg, SL_TASK_SYSTEM_ID);
			set_if_src_type(if_msg, IF_TYPE_CAN_BUS_SL);
			set_if_des_type(if_msg, IF_TYPE_CAN_BUS_MT);
			set_if_sig(if_msg, SL_SYSTEM_CANBUS_DYNAMIC);
			set_msg_src_task_id(if_msg, SL_TASK_CONSOLE_ID);
			set_msg_sig(if_msg, SL_IF_DYNAMIC_MSG_OUT);
			set_if_data_dynamic_msg(if_msg, (uint8_t*)hugeMsg, strlen(hugeMsg));
			task_post(SL_TASK_IF_ID, if_msg);
			CanIdWaitForTrans.enQueue(0x02);
		}
		else {
			APP_PRINT("\n<Make commands>\n");
			APP_PRINT("Usage:\n");
			APP_PRINT("  make [options]\n");
			APP_PRINT("Options:\n");
			APP_PRINT("  a: Send message to host 0x03 via CAN Bus\n");
			APP_PRINT("  b: Send message to node 0x01 via CAN Bus\n");
			APP_PRINT("  c: Send message to node 0x02 via CAN Bus\n");
		}
	}
	break;

	case 2: {
		
	}
	break;

	default: {

	}
	break;
	}

	return 0;
}