#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "heap.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"

#include "task_list.h"
#include "task_sm.h"
#include "task_if.h"
#include "task_cpu_serial_if.h"
#include "task_system.h"
#include "task_console.h"
#include "task_can_if.h"
#include "can_high_layer.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "platform.h"

#include "sys_dbg.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

/* Function implementation ---------------------------------------------------*/
int main_app() {
#if (CANBUS_ID_OPTION == 0x01)
	APP_PRINT("\r\n"                                               
			KYEL"   _|_|_|    _|_|    _|      _|        _|      _|\r\n"
			KYEL" _|        _|    _|  _|_|    _|      _|  _|  _|_|\r\n"
			KYEL" _|        _|_|_|_|  _|  _|  _|      _|  _|    _|\r\n"
			KYEL" _|        _|    _|  _|    _|_|      _|  _|    _|\r\n"
			KYEL"   _|_|_|  _|    _|  _|      _|        _|      _|\r\n"
		);
#elif (CANBUS_ID_OPTION == 0x02)
	APP_PRINT("\r\n"                                               
			KYEL"  _|_|_|    _|_|    _|      _|        _|      _|_|  \r\n"
			KYEL"_|        _|    _|  _|_|    _|      _|  _|  _|    _|\r\n"
			KYEL"_|        _|_|_|_|  _|  _|  _|      _|  _|      _|  \r\n"
			KYEL"_|        _|    _|  _|    _|_|      _|  _|    _|    \r\n"
			KYEL"  _|_|_|  _|    _|  _|      _|        _|    _|_|_|_|\r\n"
		);
#else
    
                                                 
#endif

	/*---------------------------------------------------------------------*/
	/*						Init active kernel 						   	   */	 
	/*---------------------------------------------------------------------*/
	ENTRY_CRITICAL();
	task_init();
	task_create((task_t*)app_task_table);
	task_polling_create((task_polling_t*)app_task_polling_table);
	EXIT_CRITICAL();

	/*---------------------------------------------------------------------*/
	/*					 Software configuration 						   */	 
	/*---------------------------------------------------------------------*/
	IO_RCC_OpenClk();
	watchdogInit();	/* 32s */

	/*---------------------------------------------------------------------*/
	/*					 Hardware configuration							   */
	/*---------------------------------------------------------------------*/
	ledLifeInit();
	
	/*---------------------------------------------------------------------*/
	/*							 System app setup						   */	
	/*---------------------------------------------------------------------*/
	fatalInit();
	sysBootInit();

	/*---------------------------------------------------------------------*/
	/*						Application task initial					   */	
	/*---------------------------------------------------------------------*/
	app_init_state_machine();
	app_start_timer();
	app_task_init();

	/*---------------------------------------------------------------------*/
	/*							 System app run							   */
	/*---------------------------------------------------------------------*/
	EXIT_CRITICAL();

	return task_run();
}
/*-------------------------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*/
/* App initial function.
 */
/*----------------------------------------------------------------------------*/
/*---------------------------------------
 * Start software timer for application
 * used for app tasks
 ---------------------------------------*/
void app_start_timer() {
    timer_set(SL_TASK_SYSTEM_ID, SL_SYSTEM_KEEP_ALIVE, SL_SYSTEM_ALIVE_NOTIFY_INTERVAL, TIMER_PERIODIC);
}

/*---------------------------------------
 * Init state machine for tasks
 * used for app tasks
 ---------------------------------------*/
void app_init_state_machine() {
	
}

/*---------------------------------------------
 * Send first message to trigger start tasks
 * used for app tasks
 ---------------------------------------------*/
void app_task_init() {
#ifdef IF_LINK_UART_EN
	task_post_pure_msg(SL_TASK_CPU_SERIAL_IF_ID, SL_CPU_SERIAL_IF_INIT);
#endif
#ifdef IF_CAN_BUS_EN
	task_post_pure_msg(SL_TASK_CAN_IF_ID, SL_CAN_IF_INIT);
#endif
}

/*-------------------------------------------------------------------------------------------*/


