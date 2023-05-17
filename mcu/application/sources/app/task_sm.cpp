#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "timer.h"
#include "tsm.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "task_list.h"
#include "task_sm.h"

#include "app_if.h"
#include "task_list_if.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG "TaskSM"

#define FORWARD_MSG_OUT(i, s, m)     serialForwardOutside(i, s, m);
#define FORWARD_MSG_IN(i, s, m)      serialForwardInside(i, s, m)

/* Extern variables ----------------------------------------------------------*/
tsm_tbl_t slStateMachine;

/* Private function prototypes -----------------------------------------------*/
static void canForwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg);
static void canForwardInside(uint8_t eId, uint8_t eSig, ak_msg_t *msg);
static void serialForwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg);
static void serialForwardInside(uint8_t iId, uint8_t iSig, ak_msg_t *msg);

/*---------------------------------------------
 * State machine IDLE
 *
 ---------------------------------------------*/
static tsm_t idleStatusTbl[] = {
    /* IDLE */
   

    /* OTA */
    

    /* END */
    
};


/*---------------------------------------------
 * State machine FIRMWARE OTA
 *
 ---------------------------------------------*/
static tsm_t otaStatusTbl[] = {
    /* IDLE */


    /* END */
    
};

/*---------------------------------------------
 * State machine table control
 *
 ---------------------------------------------*/
tsm_t* slStateMachineTbl[] = {
    idleStatusTbl,
    otaStatusTbl
};

/* Function implementation ---------------------------------------------------*/
void TaskSm(ak_msg_t* msg) {
    tsm_dispatch(&slStateMachine, msg);
}

/*----------------------------------------------------------------------------*/
void slStateMachineOnState(tsm_state_t state) {
    switch(state) {
    case SM_IDLE: {
        APP_DBG(TAG, "SM_IDLE");
    }
    break;

    case SM_OTA: {
        APP_DBG(TAG, "SM_OTA");
    }
    break;

    default:
    break;
    }
}

/* Groups functions state SM_IDLE ----------------------------------------------*/

/* Groups functions state SM_OTA -----------------------------------------------*/

/* Private functions -----------------------------------------------------------*/
#pragma GCC diagnostic ignored "-Wunused-function"
void serialForwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg) {
    msg_inc_ref_count(msg);
    set_if_src_task_id(msg, SL_TASK_SM_ID);
    set_if_des_task_id(msg, eId);
    set_if_src_type(msg, IF_TYPE_CPU_SERIAL_SL);
    set_if_des_type(msg, IF_TYPE_CPU_SERIAL_MT);
    set_if_sig(msg, eSig);
    set_msg_src_task_id(msg, SL_TASK_SM_ID);

    switch (get_msg_type(msg)) {
    case PURE_MSG_TYPE: set_msg_sig(msg, SL_IF_PURE_MSG_OUT);
    break;
    
    case COMMON_MSG_TYPE: set_msg_sig(msg, SL_IF_COMMON_MSG_OUT);
    break;

    case DYNAMIC_MSG_TYPE: set_msg_sig(msg, SL_IF_DYNAMIC_MSG_OUT);
    break;

    default:
    break;
    }
    task_post(SL_TASK_IF_ID, msg);
}

void serialForwardInside(uint8_t iId, uint8_t iSig, ak_msg_t *msg) {
    msg_inc_ref_count(msg);
    set_msg_sig(msg, iSig);
    // set_msg_src_task_id(msg, MT_TASK_SM_ID);
    task_post(iId, msg);  
}

void canForwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg) {
    msg_inc_ref_count(msg);
    set_if_src_task_id(msg, SL_TASK_SM_ID);
    set_if_des_task_id(msg, eId);
    set_if_src_type(msg, IF_TYPE_CAN_BUS_SL);
    set_if_des_type(msg, IF_TYPE_CAN_BUS_MT);
    set_if_sig(msg, eSig);
    set_msg_src_task_id(msg, SL_TASK_SM_ID);

    switch (get_msg_type(msg)) {
    case PURE_MSG_TYPE: set_msg_sig(msg, SL_IF_PURE_MSG_OUT);
    break;
    
    case COMMON_MSG_TYPE: set_msg_sig(msg, SL_IF_COMMON_MSG_OUT);
    break;

    case DYNAMIC_MSG_TYPE: set_msg_sig(msg, SL_IF_DYNAMIC_MSG_OUT);
    break;

    default:
    break;
    }
    task_post(SL_TASK_IF_ID, msg);
}

void canForwardInside(uint8_t eId, uint8_t eSig, ak_msg_t *msg) {

}
