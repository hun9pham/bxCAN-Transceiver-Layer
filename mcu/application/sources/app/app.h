#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ak.h"
#include "tsm.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_CONSOLE_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_CONSOLE_HANDLE_CMD_LINE = AK_USER_DEFINE_SIG,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
#define IF_TYPE_CAN_BUS_MT      ( 118 )
#define IF_TYPE_CAN_BUS_SL      ( 119 )

#define IF_TYPE_CPU_SERIAL_MT   ( 120 )
#define IF_TYPE_CPU_SERIAL_SL	( 121 )

/* Define timer */

/* Define signal */
enum {
    SL_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    SL_IF_PURE_MSG_OUT,
    SL_IF_COMMON_MSG_IN	,
    SL_IF_COMMON_MSG_OUT,
    SL_IF_DYNAMIC_MSG_IN,
    SL_IF_DYNAMIC_MSG_OUT
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_CPU_SERIAL_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_CPU_SERIAL_IF_INIT = AK_USER_DEFINE_SIG,
    SL_CPU_SERIAL_IF_PURE_MSG_OUT,
    SL_CPU_SERIAL_IF_COMMON_MSG_OUT,
    SL_CPU_SERIAL_IF_DYNAMIC_MSG_OUT,
    SL_CPU_SERIAL_IF_PURE_MSG_IN,
    SL_CPU_SERIAL_IF_COMMON_MSG_IN,
    SL_CPU_SERIAL_IF_DYNAMIC_MSG_IN,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_CAN_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_CAN_IF_INIT = AK_USER_DEFINE_SIG,
    SL_CAN_IF_PURE_MSG_OUT,
    SL_CAN_IF_COMMON_MSG_OUT,
    SL_CAN_IF_DYNAMIC_MSG_OUT,
    SL_CAN_IF_PURE_MSG_IN,
    SL_CAN_IF_COMMON_MSG_IN,
    SL_CAN_IF_DYNAMIC_MSG_IN,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_SYSTEM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define SL_SYSTEM_ALIVE_NOTIFY_INTERVAL         ( 1000 )
#define SL_SYSTEM_CONTROL_REBOOT_AFTER          ( 500 )

/* Define signal */
enum {
    SL_SYSTEM_KEEP_ALIVE = AK_USER_DEFINE_SIG,
    SL_SYSTEM_REBOOT_REQ,

    SL_SYSTEM_CANBUS_PURE,
    SL_SYSTEM_CANBUS_COMMON,
    SL_SYSTEM_CANBUS_DYNAMIC,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#ifndef APP_START_ADDR
#define APP_START_ADDR                  ( 0x08003000 )
#define BOOT_START_ADDR                 ( 0x08000000 )
#endif

#define NORMAL_START_ADDRESS			(APP_START_ADDR)
#define BOOT_START_ADDRESS			    (BOOT_START_ADDR)

#ifdef RELEASE
#define FIRMWARE_VERSION 		         "v1.1"
#define HARDWARE_VERSION 		         "v1.1"
#define RUN_AS                           "Release"
#else
#define FIRMWARE_VERSION 		         "v1.0"
#define HARDWARE_VERSION 		         "v1.1"
#define RUN_AS                           "Debug"
#endif

#ifndef IF_CAN_BUS_EN
#define IF_CAN_BUS_EN
#endif

/* Function prototypes -------------------------------------------------------*/
extern int main_app();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
