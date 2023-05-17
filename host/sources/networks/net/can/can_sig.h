#ifndef __CAN_SIG_H
#define __CAN_SIG_H

#include "ak.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_HIGH_LAYER_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define CANBUS_FRAME_TRANSMIT_TIMEOUT_AFTER     (500)
#define CANBUS_FRAME_RECEIVE_TIMEOUT_AFTER      (500)

/* Define signal */
enum {
    MT_CAN_HIGH_PACKED_FRAME_HEADER = AK_USER_DEFINE_SIG,
    MT_CAN_HIGH_PACKED_FRAME_DATA,
    MT_CAN_HIGH_SEND_FRAME,
    MT_CAN_HIGH_SEND_FRAME_TIMEOUT,
    MT_CAN_HIGH_SEND_FRAME_COMPLETED,
    MT_CAN_HIGH_RECV_FRAME,
    MT_CAN_HIGH_RECV_FRAME_TIMEOUT,
    MT_CAN_HIGH_SEND_ACK,
    MT_CAN_HIGH_RECV_ACK,
    MT_CAN_HIGH_RECV_ACK_TIMEOUT,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_LOW_LAYER_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_CAN_LOW_LAYER_INIT = AK_USER_DEFINE_SIG,
    MT_CAN_LOW_SEND_PURE_MSG,
    MT_CAN_LOW_SEND_COMMON_MSG,
    MT_CAN_LOW_SEND_DYNAMIC_MSG,
    MT_CAN_LOW_PARSER_FRAME_DATA_RECV,
    MT_CAN_LOW_HANDLE_PDU_FULL,
};

#endif /* __CAN_SIG_H */