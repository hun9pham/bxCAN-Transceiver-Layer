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
#define TAG	"CanHighLayer"
#define CAN_FRAME_DBG_EN    (0)

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskCanHighLayerMailbox;

/* Private variables ---------------------------------------------------------*/
static pthread_t CanRxThread;

static CanFrame_t txCanPhy;
static CanTransState_t CanTransmitterState = CANTX_STATE_IDLE;
static uint8_t *txFramePointer = NULL;
static uint16_t txFrameSize;
static uint8_t txPduIdAtv;
static uint8_t txFailedRetryCnt;

static CanFrame_t rxCanPhy;
static uint8_t rxPduIdAtv;
static CanRecvState_t CanReceiverState = CANRX_STATE_IDLE;
static CanRecvParserState_t rxFrameRecvParserState = CAN_PARSER_STATE_SOF;

/* Private function prototypes -----------------------------------------------*/
static uint8_t calcCsU8Frame(CanFrame_t *ptr) {
    uint8_t ret = 0, id;
    uint8_t *ptrU8 = (uint8_t*)ptr;

    for (id = 0; id < sizeof(frameHeader_t); ++id) {
        ret ^= ptrU8[id];
    }
    ptrU8 += sizeof(frameHeader_t);
    for (id = 0; id < ptr->header.len; ++id) {
        ret ^= ptrU8[id];
    }
    ptrU8 += CAN_PHY_FRAME_DATA_SIZE;

	return ret;
}

static uint8_t sendFrameBlock(CanFrame_t *frameTrans) {
#if CAN_FRAME_DBG_EN
    CAN_PRINT("TX=======================================================================================\r\n");
    CAN_PRINT("| SOF: 0x%X | DES: 0x%X | SRC: 0x%X | TYPE: %d | LEN: %d | DATA | FCS: 0x%X | EOF: 0x%X |\r\n",
                frameTrans->header.sof, frameTrans->header.destNodeId, frameTrans->header.srcNodeId,
                frameTrans->header.type, frameTrans->header.len, frameTrans->footer.fcs, frameTrans->footer.eof);
    CAN_PRINT("TX=======================================================================================\r\n");
#endif

    /* Send frame header */
    if (CANUSB_OK != UsbCan.sendData(frameTrans->header.destNodeId, (uint8_t*)&frameTrans->header, sizeof(frameHeader_t))) {
        /* TODO */
        CAN_DBG(TAG, "[NG] FrameSend->Header");
    }

    /* Send frame data */
    if (CANUSB_OK != UsbCan.sendData(frameTrans->header.destNodeId, frameTrans->payload, frameTrans->header.len)) {
        /* TODO */
        CAN_DBG(TAG, "[NG] FrameSend->payload");
    }

    /* Send frame footer */
    if (CANUSB_OK != UsbCan.sendData(frameTrans->header.destNodeId, (uint8_t*)&frameTrans->footer, sizeof(frameFooter_t))) {
        /* TODO */
        CAN_DBG(TAG, "[NG] FrameSend->footer");
    }
    
    return CANBUS_RET_NOERR;
}

static void* CanRxThreadCb(void*) {
    while (1) {
        if (CANUSB_OK == UsbCan.recvData()) {
            for (int id = 0; id < UsbCan.getRxSize(); ++id) {
                uint8_t *pt = UsbCan.getRxData();
                CanParserByteRecv(pt[id]);
            }
        }
    }

    return (void*)0;
}

/* Function implementation ---------------------------------------------------*/
void* TaskCanHighLayerEntry(void*) {
    ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    int ret = UsbCan.Start((const char*)"/dev/ttyUSB0", CANUSB_TTY_BAUD_RATE_DEFAULT,
            GW_CANNODE_FILTER_ID, CANUSB_SPEED_1000000, CANUSB_FRAME_STANDARD, CANUSB_MODE_NORMAL);

    if (ret != -1) {
        CAN_DBG(TAG, "[OK] USB_to_CAN");
        pthread_create(&CanRxThread, NULL, CanRxThreadCb, NULL);
    }
    else {
        CAN_DBG(TAG, "[NG] USB_to_CAN");
    }

    APP_PRINT("[STARTED] MT_TASK_CAN_HIGH_LAYER_ID Entry\n");

    while (1) {
        msg = ak_msg_rev(MT_TASK_CAN_HIGH_LAYER_ID);

        switch (msg->header->sig) {
        case MT_CAN_HIGH_PACKED_FRAME_HEADER: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_PACKED_FRAME_HEADER");

            /*
            ===================================================
            | SOF | DES | SRC | TYPE | LEN | . . . . . . 
            ===================================================
            - Package frame header include:
            - SOF: Start of frame
            - DES: Destination node on CAN Bus
            - SRC: Source node on CAN Bus
            - TYPE: Type of frame
            - LEN: Length of data
            */

            uint8_t pduId = *(get_data_common_msg(msg));
            CAN_Pdu_t *pduPtr = CanPduManager.getPdu(pduId);

            if (CANTX_STATE_IDLE == CanTransmitterState) {
                CanTransmitterState = CANTX_STATE_SENDING;

                txCanPhy.header.sof = CAN_PHY_SOF;
                txCanPhy.header.destNodeId = pduPtr->CanNodeIdTx;
                txCanPhy.header.srcNodeId = GW_CANNODE_FILTER_ID;
                txCanPhy.header.type = FRAME_TYPE_REQ_ACK_FROM_RECEIVER;
                txCanPhy.footer.eof = CAN_PHY_EOF;

                txFramePointer = pduPtr->payload;
                txFrameSize = pduPtr->len;
                txPduIdAtv = pduId;
                txFailedRetryCnt = 0;
                task_post_pure_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_DATA);
            }
            else {
                PduWaitForHandle.enQueue(pduId);
            }
        }
        break;

        case MT_CAN_HIGH_PACKED_FRAME_DATA: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_PACKED_FRAME_DATA");

            /*
            - Package and start transmission for each data frame in the 
            - PDU with size of CAN_PHY_FRAME_DATA_SIZE
            */
            uint8_t packetLen;

            if (txFrameSize > CAN_PHY_FRAME_DATA_SIZE) {
                packetLen = CAN_PHY_FRAME_DATA_SIZE;
            }
            else {
                packetLen = txFrameSize;
            }
            memcpy(&txCanPhy.payload[0], txFramePointer, packetLen);
            txCanPhy.header.len = packetLen;
            txCanPhy.footer.fcs = calcCsU8Frame(&txCanPhy);
            txFramePointer += packetLen;
            task_post_pure_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_SEND_FRAME);
        }
        break;

        case MT_CAN_HIGH_SEND_FRAME: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_SEND_FRAME");

            if (CANTX_STATE_SENDING == CanTransmitterState) {
                sendFrameBlock(&txCanPhy);
                timer_set(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_RECV_ACK_TIMEOUT, CANBUS_FRAME_RECEIVE_TIMEOUT_AFTER, TIMER_ONE_SHOT);   
            }
        }
        break;

        case MT_CAN_HIGH_RECV_FRAME: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_RECV_FRAME");

            /*
            - Parse frame receive and handle based on frame type:
            - FRAME_TYPE_REQ_ACK_FROM_RECEIVER: Respond ACK to CAN Node has transmitted
            - FRAME_TYPE_RECEIVER_RESP_ACK: Check ACK from CAN Node is correct or not for 
                                            deciding to transmit next frame data
            - FRAME_TYPE_DATA_TRANSMIT_COMPLETED: Frame data transmission has completed, start parser frame data
            */

            CanFrame_t *frameRecv = (CanFrame_t*)get_data_common_msg(msg);

    #if CAN_FRAME_DBG_EN
            CAN_PRINT("RX=======================================================================================\r\n");
            CAN_PRINT("| SOF: 0x%X | DES: 0x%X | SRC: 0x%X | TYPE: %d | LEN: %d | DATA | FCS: 0x%X | EOF: 0x%X |\r\n",
                        frameRecv->header.sof, frameRecv->header.destNodeId, frameRecv->header.srcNodeId,
                        frameRecv->header.type, frameRecv->header.len, frameRecv->footer.fcs, frameRecv->footer.eof);
            CAN_PRINT("RX=======================================================================================\r\n");
    #endif

            switch (frameRecv->header.type) {
            case FRAME_TYPE_REQ_ACK_FROM_RECEIVER: {
                uint32_t ACK_RespTo = frameRecv->header.srcNodeId;

                /* Handle data received */
                CAN_Pdu_t *ptPdu = NULL;
                if (CANRX_STATE_IDLE == CanReceiverState) {
                    CanReceiverState = CANRX_STATE_BUSY;
                    ptPdu = CanPduManager.pduNew();
                    rxPduIdAtv = ptPdu->pduId;
                }
                ptPdu = CanPduManager.getPdu(rxPduIdAtv);
                uint8_t *ptU8 = ptPdu->payload;
                ptU8 += ptPdu->len;
                memcpy(ptU8, frameRecv->payload, frameRecv->header.len);
                ptPdu->len += frameRecv->header.len;

                /* Respond ACK */
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_SEND_ACK, (uint8_t*)&ACK_RespTo, sizeof(uint32_t));
            }
            break;

            case FRAME_TYPE_RECEIVER_RESP_ACK: {
                uint32_t ACK_RecvFrom = frameRecv->header.srcNodeId;
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_RECV_ACK, (uint8_t*)&ACK_RecvFrom, sizeof(uint32_t));
            }
            break;

            case FRAME_TYPE_DATA_TRANSMIT_COMPLETED: {
                CanReceiverState = CANRX_STATE_IDLE;
                task_post_common_msg(MT_TASK_CAN_LOW_LAYER_ID, MT_CAN_LOW_PARSER_FRAME_DATA_RECV, &rxPduIdAtv, sizeof(uint8_t));
            }
            break;
            
            default:
            break;
            }
            
        }
        break;

        case MT_CAN_HIGH_SEND_ACK: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_SEND_ACK");

            uint32_t destNodeId;
            memcpy(&destNodeId, get_data_common_msg(msg), sizeof(uint32_t));

            CanFrame_t frameAckSend;
            frameAckSend.header.sof = CAN_PHY_SOF;
            frameAckSend.header.destNodeId = destNodeId;
            frameAckSend.header.srcNodeId = GW_CANNODE_FILTER_ID;
            frameAckSend.header.type = FRAME_TYPE_RECEIVER_RESP_ACK;
            frameAckSend.header.len = 0;
            memset(frameAckSend.payload, 0, CAN_PHY_FRAME_DATA_SIZE);
            frameAckSend.footer.fcs = calcCsU8Frame(&frameAckSend);
            frameAckSend.footer.eof = CAN_PHY_EOF;

            sendFrameBlock(&frameAckSend);
        }
        break;

        case MT_CAN_HIGH_RECV_ACK: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_RECV_ACK");

            uint32_t ackFromNodeId;
            memcpy(&ackFromNodeId, get_data_common_msg(msg), sizeof(uint32_t));
            if (ackFromNodeId == txCanPhy.header.destNodeId) {
                txFrameSize -= txCanPhy.header.len;
                timer_remove_attr(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_RECV_ACK_TIMEOUT);
                if (txFrameSize > 0) {
                    task_post_pure_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_DATA);
                }
                else {
                    task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_SEND_FRAME_COMPLETED, (uint8_t*)&ackFromNodeId, sizeof(uint32_t));
                }
            }
            else {
                /*
                - ERROR: ACK Respond lost address
                -
                */
            }
        }
        break;

        case MT_CAN_HIGH_SEND_FRAME_COMPLETED: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_SEND_FRAME_COMPLETED");

            uint32_t destId;
            memcpy(&destId, get_data_common_msg(msg), sizeof(uint32_t));

            CanFrame_t ackComplete;
            ackComplete.header.sof = CAN_PHY_SOF;
            ackComplete.header.destNodeId = destId;
            ackComplete.header.srcNodeId = GW_CANNODE_FILTER_ID;
            ackComplete.header.type = FRAME_TYPE_DATA_TRANSMIT_COMPLETED;
            ackComplete.header.len = 0;
            memset(ackComplete.payload, 0, CAN_PHY_FRAME_DATA_SIZE);
            ackComplete.footer.fcs = calcCsU8Frame(&ackComplete);
            ackComplete.footer.eof = CAN_PHY_EOF;
            sendFrameBlock(&ackComplete);

            CanPduManager.freePdu(txPduIdAtv);
            CanTransmitterState = CANTX_STATE_IDLE;

            /* Handle next PDU Waiting */
            if (PduWaitForHandle.isAvailable()) {
                uint8_t pduId = PduWaitForHandle.deQueue();
                task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_HEADER, &pduId, sizeof(uint8_t));
            }
        }
        break;

        case MT_CAN_HIGH_RECV_FRAME_TIMEOUT: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_RECV_FRAME_TIMEOUT");

            CanReceiverState = CANRX_STATE_IDLE;
            rxFrameRecvParserState = CAN_PARSER_STATE_SOF;
            CanPduManager.freePdu(rxPduIdAtv);
        }
        break;

        case MT_CAN_HIGH_RECV_ACK_TIMEOUT: {
            CAN_DBG_SIG(TAG, "MT_CAN_HIGH_RECV_ACK_TIMEOUT");

            if (++txFailedRetryCnt < CAN_PHYTX_MAX_RETRY) {
                task_post_pure_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_SEND_FRAME);
            }
            else {
                CAN_DBG(TAG, "CAN PDU-%d TRANSMIT FAILED", txPduIdAtv);
                CanTransmitterState = CANTX_STATE_IDLE;
                CanPduManager.freePdu(txPduIdAtv);
                
                if (PduWaitForHandle.isAvailable()) {
                    uint8_t pduId = PduWaitForHandle.deQueue();
                    task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_PACKED_FRAME_HEADER, &pduId, sizeof(uint8_t));
                }
            }
        }
        break;
        
        default:
        break;
        }
    }

    return (void*)0;
}

void CanParserByteRecv(uint8_t c) {
	static uint8_t dataIndxRecv;

	switch (rxFrameRecvParserState) {
	case CAN_PARSER_STATE_SOF: {
		if (CAN_PHY_SOF == c) {
            memset(&rxCanPhy, 0, sizeof(CanFrame_t));
			rxCanPhy.header.sof = c;
			rxFrameRecvParserState = CAN_PARSER_STATE_DES_CANNODE;
			dataIndxRecv = 0;
		}
	}
	break;

	case CAN_PARSER_STATE_DES_CANNODE: {
		((uint8_t*)&rxCanPhy.header.destNodeId)[dataIndxRecv++] = c;
		if (dataIndxRecv == sizeof(rxCanPhy.header.destNodeId)) {
			rxFrameRecvParserState = CAN_PARSER_STATE_SRC_CANNODE;
			dataIndxRecv = 0;
		}
	}
	break;

	case CAN_PARSER_STATE_SRC_CANNODE: {
		((uint8_t*)&rxCanPhy.header.srcNodeId)[dataIndxRecv++] = c;
		if (dataIndxRecv == sizeof(rxCanPhy.header.srcNodeId)) {
            rxFrameRecvParserState = CAN_PARSER_STATE_TYPE;
		}
	}
	break;

	case CAN_PARSER_STATE_TYPE: {
		rxCanPhy.header.type = c;
		rxFrameRecvParserState = CAN_PARSER_STATE_LEN;
		dataIndxRecv = 0;
	}
	break;

	case CAN_PARSER_STATE_LEN: {
		((uint8_t*)&rxCanPhy.header.len)[dataIndxRecv++] = c;
		if (dataIndxRecv == sizeof(rxCanPhy.header.len)) {

            if (rxCanPhy.header.len > CAN_PHY_FRAME_DATA_SIZE) {
				rxFrameRecvParserState = CAN_PARSER_STATE_SOF;
			}
            else if (rxCanPhy.header.len == 0) {
                rxFrameRecvParserState = CAN_PARSER_STATE_FCS;
            }
            else {
                dataIndxRecv = 0;
                rxFrameRecvParserState = CAN_PARSER_STATE_DATA;
            }
		}
	}
	break;

	case CAN_PARSER_STATE_DATA: {
        if (rxCanPhy.header.len > 0) {
            rxCanPhy.payload[dataIndxRecv++] = c;
		    if (dataIndxRecv == rxCanPhy.header.len) {
			    rxFrameRecvParserState = CAN_PARSER_STATE_FCS;
            }
        }
        else {
            rxFrameRecvParserState = CAN_PARSER_STATE_SOF;
        }
	}
	break;

    case CAN_PARSER_STATE_FCS: {
		rxCanPhy.footer.fcs = c;
        uint8_t fCsCalc = calcCsU8Frame(&rxCanPhy);
        if (fCsCalc == rxCanPhy.footer.fcs) {
            rxFrameRecvParserState = CAN_PARSER_STATE_EOF;
        }
        else {
            rxFrameRecvParserState = CAN_PARSER_STATE_SOF;
        }
	}
	break;

	case CAN_PARSER_STATE_EOF: {
		if (CAN_PHY_EOF == c) {
            rxCanPhy.footer.eof = c;
            task_post_common_msg(MT_TASK_CAN_HIGH_LAYER_ID, MT_CAN_HIGH_RECV_FRAME, (uint8_t*)&rxCanPhy, sizeof(CanFrame_t));
		}
		rxFrameRecvParserState = CAN_PARSER_STATE_SOF;
	}
	break;
	
	default:
	break;
	}
}
