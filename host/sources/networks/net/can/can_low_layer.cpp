#include <stdint.h>
#include <stdlib.h>

#include "ak.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_can_if.h"

#include "can_usb.h"
#include "can_sig.h"
#include "can_dbg.h"
#include "can_data.h"
#include "can_low_layer.h"
#include "can_high_layer.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define TAG	"CanLowLayer"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskCanLowLayerMailbox;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void* TaskCanLowLayerEntry(void*) {
    ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_CAN_LOW_LAYER_ID Entry\n");
    task_post_pure_msg(MT_TASK_CAN_LOW_LAYER_ID, MT_CAN_LOW_LAYER_INIT);

    while (1) {
        msg = ak_msg_rev(MT_TASK_CAN_LOW_LAYER_ID);

        switch (msg->header->sig) {
        case MT_CAN_LOW_LAYER_INIT: {
            CAN_DBG_SIG(TAG, "MT_CAN_LOW_LAYER_INIT");

            PduWaitForHandle.QueueInit();
            CanIdWaitForTrans.QueueInit();
            CanPduManager.PduInit();
        }
        break;

        case MT_CAN_LOW_SEND_PURE_MSG: {
            CAN_DBG_SIG(TAG, "MT_CAN_LOW_SEND_PURE_MSG");

            ak_msg_pure_if_t if_msg;
            if_msg.header.src_task_id = msg->header->if_src_task_id;
            if_msg.header.des_task_id = msg->header->if_des_task_id;
            if_msg.header.type = get_msg_type(msg);
            if_msg.header.if_src_type = msg->header->if_src_type;
            if_msg.header.if_des_type = msg->header->if_des_type;
            if_msg.header.sig = msg->header->if_sig;

            if (CanPduManager.isSpaceAvaiable()) {
                CAN_Pdu_t *ptPdu = CanPduManager.pduNew();
                ptPdu->CanNodeIdTx = CanIdWaitForTrans.deQueue();
                ptPdu->len = sizeof(ak_msg_if_header_t);
                memcpy(ptPdu->payload, (uint8_t*)&if_msg, ptPdu->len);

                uint8_t pduId = ptPdu->pduId;
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_HEADER, &pduId, sizeof(uint8_t));
            }
            else {
                task_post_pure_msg(MT_TASK_CAN_LOW_LAYER_ID, MT_CAN_LOW_HANDLE_PDU_FULL);
            }
        }
        break;

        case MT_CAN_LOW_SEND_COMMON_MSG: {
            CAN_DBG_SIG(TAG, "MT_CAN_LOW_SEND_COMMON_MSG");

            ak_msg_common_if_t if_msg;
            if_msg.header.src_task_id = msg->header->if_src_task_id;
            if_msg.header.des_task_id = msg->header->if_des_task_id;
            if_msg.header.type = get_msg_type(msg);
            if_msg.header.if_src_type = msg->header->if_src_type;
            if_msg.header.if_des_type = msg->header->if_des_type;
            if_msg.header.sig = msg->header->if_sig;
            if_msg.len = get_data_len_common_msg(msg);
            memcpy(if_msg.data, get_data_common_msg(msg), if_msg.len);

            if (CanPduManager.isSpaceAvaiable()) {
                CAN_Pdu_t *ptPdu = CanPduManager.pduNew();
                ptPdu->CanNodeIdTx = CanIdWaitForTrans.deQueue();
                ptPdu->len = sizeof(ak_msg_if_header_t) + sizeof(uint8_t) + if_msg.len;
                uint8_t *ptU8 = (uint8_t*)ptPdu->payload;
                memcpy(ptU8, (uint8_t*)&if_msg.header, sizeof(ak_msg_if_header_t));
                memcpy(ptU8 + sizeof(ak_msg_if_header_t), (uint8_t*)&if_msg.len, sizeof(uint8_t));
                memcpy(ptU8 + sizeof(ak_msg_if_header_t) + sizeof(uint8_t), (uint8_t*)&if_msg.data, if_msg.len);

                uint8_t pduId = ptPdu->pduId;
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_HEADER, &pduId, sizeof(uint8_t));
            }
            else {
                task_post_pure_msg(MT_TASK_CAN_LOW_LAYER_ID, MT_CAN_LOW_HANDLE_PDU_FULL);
            }
        }
        break;

        case MT_CAN_LOW_SEND_DYNAMIC_MSG: {
            CAN_DBG_SIG(TAG, "MT_CAN_LOW_SEND_DYNAMIC_MSG");

            ak_msg_dynamic_if_t if_msg;
            if_msg.header.src_task_id = msg->header->if_src_task_id;
            if_msg.header.des_task_id = msg->header->if_des_task_id;
            if_msg.header.type = get_msg_type(msg);
            if_msg.header.if_src_type = msg->header->if_src_type;
            if_msg.header.if_des_type = msg->header->if_des_type;
            if_msg.header.sig = msg->header->if_sig;
            if_msg.len = get_data_len_dynamic_msg(msg);

            if (CanPduManager.isSpaceAvaiable()) {
                CAN_Pdu_t *ptPdu = CanPduManager.pduNew();
                ptPdu->CanNodeIdTx = CanIdWaitForTrans.deQueue();
                ptPdu->len = sizeof(ak_msg_if_header_t) + sizeof(uint32_t) + if_msg.len;
                uint8_t *ptU8 = (uint8_t*)ptPdu->payload;
                memcpy(ptU8, (uint8_t*)&if_msg.header, sizeof(ak_msg_if_header_t));
                memcpy(ptU8 + sizeof(ak_msg_if_header_t), (uint8_t*)&if_msg.len, sizeof(uint32_t));
                get_data_dynamic_msg(msg, (ptU8 + sizeof(ak_msg_if_header_t) + sizeof(uint32_t)), if_msg.len);
                // memcpy(ptU8 + sizeof(ak_msg_if_header_t) + sizeof(uint32_t), (uint8_t*)if_msg.data, if_msg.len);

                uint8_t pduId = ptPdu->pduId;
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_HEADER, &pduId, sizeof(uint8_t));
            }
            else {
                task_post_pure_msg(MT_TASK_CAN_LOW_LAYER_ID, MT_CAN_LOW_HANDLE_PDU_FULL);
            }
        }
        break;

        case MT_CAN_LOW_PARSER_FRAME_DATA_RECV: {
            CAN_DBG_SIG(TAG, "MT_CAN_LOW_PARSER_FRAME_DATA_RECV");

            uint8_t pduId;
            memcpy(&pduId, get_data_common_msg(msg), sizeof(uint8_t));

            CAN_Pdu_t *ptPdu = CanPduManager.getPdu(pduId);
            ak_msg_if_header_t *if_msg = (ak_msg_if_header_t*)ptPdu->payload;

            switch (if_msg->type) {
            case PURE_MSG_TYPE: {
                ak_msg_t* s_msg = get_pure_msg();
                set_if_src_task_id(s_msg, if_msg->src_task_id);
                set_if_des_task_id(s_msg, if_msg->des_task_id);
                set_if_src_type(s_msg, if_msg->if_src_type);
                set_if_des_type(s_msg, if_msg->if_des_type);
                set_if_sig(s_msg, if_msg->sig);

                set_msg_sig(s_msg, MT_CAN_IF_PURE_MSG_IN);
                task_post(MT_TASK_CAN_IF_ID, s_msg);
            }
            break;

            case COMMON_MSG_TYPE: {
                uint8_t len = *(uint8_t*)(ptPdu->payload + sizeof(ak_msg_if_header_t));
                uint8_t *data = ptPdu->payload + sizeof(ak_msg_if_header_t) + sizeof(uint8_t);

                ak_msg_t* s_msg = get_common_msg();
                set_if_src_task_id(s_msg, if_msg->src_task_id);
                set_if_des_task_id(s_msg, if_msg->des_task_id);
                set_if_src_type(s_msg, if_msg->if_src_type);
                set_if_des_type(s_msg, if_msg->if_des_type);
                set_if_sig(s_msg, if_msg->sig);
                set_if_data_common_msg(s_msg, data, len);

                set_msg_sig(s_msg, MT_CAN_IF_COMMON_MSG_IN);
                task_post(MT_TASK_CAN_IF_ID, s_msg);
            }
            break;

            case DYNAMIC_MSG_TYPE: {
                uint32_t len = *(uint32_t*)(ptPdu->payload + sizeof(ak_msg_if_header_t));
                uint8_t *data = ptPdu->payload + sizeof(ak_msg_if_header_t) + sizeof(uint32_t);

                ak_msg_t* s_msg = get_dynamic_msg();
                set_if_src_task_id(s_msg, if_msg->src_task_id);
                set_if_des_task_id(s_msg, if_msg->des_task_id);
                set_if_src_type(s_msg, if_msg->if_src_type);
                set_if_des_type(s_msg, if_msg->if_des_type);
                set_if_sig(s_msg, if_msg->sig);
                set_if_data_dynamic_msg(s_msg, data, len);

                set_msg_sig(s_msg, MT_CAN_IF_DYNAMIC_MSG_IN);
                task_post(MT_TASK_CAN_IF_ID, s_msg);
            }
            break;

            case MT_CAN_LOW_HANDLE_PDU_FULL: {
                CAN_DBG_SIG(TAG, "MT_CAN_LOW_HANDLE_PDU_FULL");

                /*
                - TODO
                -
                */
            }
            break;
            
            default:
            break;
            }

            CanPduManager.freePdu(pduId);
        }
        break;
        
        default:
        break;
        }
    }
}
