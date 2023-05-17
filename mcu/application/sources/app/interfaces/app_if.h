#ifndef __APP_IF_H
#define __APP_IF_H

#include <stdint.h>

#include "ak.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_CPU_SERIAL_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_CPU_SERIAL_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    MT_CPU_SERIAL_IF_PURE_MSG_OUT,
    MT_CPU_SERIAL_IF_COMMON_MSG_IN,
    MT_CPU_SERIAL_IF_COMMON_MSG_OUT,
    MT_CPU_SERIAL_IF_DYNAMIC_MSG_IN,
    MT_CPU_SERIAL_IF_DYNAMIC_MSG_OUT,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    MT_IF_PURE_MSG_OUT,
    MT_IF_COMMON_MSG_IN,
    MT_IF_COMMON_MSG_OUT,
    MT_IF_DYNAMIC_MSG_IN,
    MT_IF_DYNAMIC_MSG_OUT
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_SYSTEM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_SYSTEM_GATEWAY_ONLINE_ENTRY = AK_USER_DEFINE_SIG,
    MT_SYSTEM_SEND_DYNAMIC_VIA_CANBUS,
    MT_SYSTEM_RECV_DYNAMIC_VIA_CANBUS,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_SM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define MT_SM_SL_SYNC_READ_INFO_TIMEOUT_INTERVAL       (7000)
#define MT_SM_SL_SYNC_UPDATE_JUMPERS_INFO_TIMEOUT_INTERVAL     (7000)

/* Define signal */
enum {
    MT_SM_SL_END_OF_TABLE = TSM_EOT_SIGNAL,
    MT_SM_SL_POWER_ON_NOTIFY = AK_USER_DEFINE_SIG,
    MT_SM_SL_GATEWAY_REBOOT_NOTIFY,
    MT_SM_SL_START_SYNC_REQ,
    MT_SM_SL_SYNC_READ_INFO_REQ,
    MT_SM_SL_SYNC_READ_INFO_REQ_TO,
    MT_SM_SL_SYNC_READ_INFO_RES,
    MT_SM_SL_SYNC_UPDATE_JUMPERS_INFO_REQ,
    MT_SM_SL_SYNC_UPDATE_JUMPERS_INFO_REQ_TO,
    MT_SM_SL_SYNC_UPDATE_JUMPERS_INFO_RES,
    MT_SM_SL_STOP_SYNC_REQ,
    MT_SM_SL_GET_SENSOR_INFO_REQ,
    MT_SM_SL_GET_SENSOR_INFO_RES,
    MT_SM_SL_START_WORK_ORDER_REQ,
    MT_SM_SL_START_WORK_ORDER_RES,
    MT_SM_SL_MAKE_STEP_WORK_ORDER_REQ,
    MT_SM_SL_MAKE_STEP_WORK_ORDER_RES,
    MT_SM_SL_STOP_WORK_ORDER_REQ,
    MT_SM_SL_STOP_WORK_ORDER_RES,
    MT_SM_SL_START_TRACKING_REQ,
    MT_SM_SL_START_TRACKING_RES,
    MT_SM_SL_STOP_TRACKING_REQ,
    MT_SM_SL_STOP_TRACKING_RES,
    MT_SM_SL_DETECT_PORT_CHANGE_REP
};

#define ID_WORK_ORDER_BUFFER_SIZE_MAX       (40)
#define STEP_WORK_ORDER_MAX                 (10)

#define CAP_CORE        "CAPCORE"
#define DELETE_CORE     "XOACORE"
#define CHANGE_CORE     "DOICORE"

/* order state */
enum {
    ORDER_INIT,
    ORDER_HANDLING,
    ORDER_WAITING,
    ORDER_DONE
};

/* order status */
enum {
    ST_OD_SYNCED,
    ST_OD_COMPLETED
};

/* step status */
enum {
    ST_READY,
    ST_WAITING,
    ST_DONE,
};

typedef struct t_port_info {
    uint8_t status;             //PLUG
    /* port info */
    uint8_t odf_idx;
    uint8_t port_idx;
} port_info_t;

typedef struct t_step_info {
    uint8_t status;              //ST_READY, ST_DONE
    port_info_t port_info;
} step_info_t;

typedef struct t_step_info_info_transfer {
    char _id[ID_WORK_ORDER_BUFFER_SIZE_MAX];    //id order
    port_info_t port_info;
    int8_t errCode;                        //TIMEOUT, SL_NG, ..
    uint8_t current_step;
} step_info_transfer_t;

#endif /* __APP_IF_H */
