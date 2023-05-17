#ifndef __CAN_HIGH_LAYER_H
#define __CAN_HIGH_LAYER_H

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define CANBUS_RET_ERR              (0)
#define CANBUS_RET_NOERR            (1)
#define CAN_PHY_SOF                 (0xFA)
#define CAN_PHY_EOF                 (0xFB)
#define CAN_PHYTX_MAX_RETRY         (3)

#define CAN_PHY_FRAME_HEADER_SIZE   (sizeof(frameHeader_t))
#define CAN_PHY_FRAME_FOOTER_SIZE   (sizeof(frameFooter_t))
#define CAN_PHY_FRAME_SIZE          (sizeof(CanFrame_t))
#define CAN_PHY_FRAME_DATA_SIZE     (AK_COMMON_MSG_DATA_SIZE - (CAN_PHY_FRAME_HEADER_SIZE + CAN_PHY_FRAME_FOOTER_SIZE))

/* Enumarics -----------------------------------------------------------------*/
enum eCanFrameType {
	FRAME_TYPE_REQ_ACK_FROM_RECEIVER = 0x00,
    FRAME_TYPE_RECEIVER_RESP_ACK,
    FRAME_TYPE_DATA_TRANSMIT_COMPLETED,
};

typedef enum eCanTransmitState {
    CANTX_STATE_IDLE = 0,
    CANTX_STATE_SENDING,
} CanTransState_t;

typedef enum eCanReceiveState {
    CANRX_STATE_IDLE = 0,
    CANRX_STATE_BUSY,
} CanRecvState_t;

typedef enum eCanRecvParserState {
	PARSER_STATE_SOF = 0x00,
    PARSER_STATE_DES_CANNODE,
    PARSER_STATE_SRC_CANNODE,
    PARSER_STATE_TYPE,
	PARSER_STATE_LEN,
	PARSER_STATE_DATA,
	PARSER_STATE_FCS,
	PARSER_STATE_EOF,
} CanRecvParserState_t;

/* Typedef -------------------------------------------------------------------*/
typedef struct t_CanFrameHeader {
    uint8_t sof;        /* Start of frame 				*/
    uint32_t destNodeId;/* Destination node on CAN Bus 	*/
	uint32_t srcNodeId; /* Source node on CAN Bus 		*/
    uint8_t type;       /* Type of frame 				*/
    uint16_t len;       /* Length of data 				*/
} __attribute__((__packed__)) frameHeader_t;

typedef struct {
    uint8_t fcs;        /* Frame checksum data			*/
    uint8_t eof;        /* End of frame                 */
} frameFooter_t;

typedef struct {
    frameHeader_t header;
	uint8_t payload[CAN_PHY_FRAME_DATA_SIZE];
    frameFooter_t footer;
} __attribute__((__packed__)) CanFrame_t;

/* Extern variables ----------------------------------------------------------*/
extern uint8_t (*pf_txCanCb)(uint32_t, uint8_t*, uint16_t);

/* Function prototypes -------------------------------------------------------*/
extern void CanParserByteRecv(uint8_t c);

#endif /* __CAN_HIGH_LAYER_H */

/*
    ===================================================
    | SOF | DES | SRC | TYPE | LEN | DATA | FCS | EOF |
    ===================================================
    - SOF: Start of frame
    - DES: Destination node on CAN Bus
    - SRC: Source node on CAN Bus
    - TYPE: Type of frame
    - LEN: Length of data
    - DATA: Block data transmission
    - FCS: Frame checksum data
    - EOF: End of frame
*/
