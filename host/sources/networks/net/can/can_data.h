#ifndef __CAN_DATA_H
#define __CAN_DATA_H

#include <stdint.h>
#include <stdbool.h>

#define CAN_PDU_BUF_SIZE			(600)
#define CAN_PDU_POOL_SIZE			(4)
#define CAN_PDU_NULL				((t_CAN_Pdu*)0)
#define CAN_DATA_BUF_SIZE           (CAN_PDU_BUF_SIZE - sizeof(frameHeader_t))

typedef struct t_CAN_Pdu {
	struct t_CAN_Pdu* next;
	uint8_t pduId;
	uint32_t CanNodeIdTx;
	uint32_t len;
	bool isUsed;
	uint8_t payload[CAN_PDU_BUF_SIZE];
} CAN_Pdu_t;

class CAN_Pdu {
private:
	CAN_Pdu_t pduPool[CAN_PDU_POOL_SIZE];
	CAN_Pdu_t *freePduPool;
	uint8_t usedCounter;
public:
	void PduInit();
	CAN_Pdu_t *pduNew();
	bool isSpaceAvaiable();
	uint8_t getSizeUsed();
	uint8_t getSizeMax();
	CAN_Pdu_t* getPdu(uint8_t pduId);
	void freePdu(uint8_t pduId);
	void freePdu(CAN_Pdu_t *ptrPdu);
};

class u8Queue {
private:
	uint8_t front;
	uint8_t rear;
	uint8_t counter;
	uint8_t u8Buf[CAN_PDU_POOL_SIZE];
public:
	void QueueInit();
	bool isFull();
	bool isEmpty();
	bool isAvailable();
	uint8_t readFront();
	uint8_t readRear();
	int8_t enQueue(uint8_t u8t);
	uint8_t deQueue();
};

extern CAN_Pdu CanPduManager;
extern u8Queue PduWaitForHandle;
extern u8Queue CanIdWaitForTrans;

#endif /* __CAN_DATA_H */
