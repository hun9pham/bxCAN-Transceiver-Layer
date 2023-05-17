#include "can_dbg.h"
#include "can_data.h"
#include "sys_dbg.h"

CAN_Pdu CanPduManager;
u8Queue PduWaitForHandle;
u8Queue CanIdWaitForTrans;

void CAN_Pdu::PduInit() {
	this->freePduPool = &this->pduPool[0];
	for (uint8_t id = 0; id < CAN_PDU_POOL_SIZE; id++) {
		this->pduPool[id].pduId = id;
		this->pduPool[id].isUsed = false;
		this->pduPool[id].len = 0;
		if (id == (CAN_PDU_POOL_SIZE - 1)) {
			this->pduPool[id].next = CAN_PDU_NULL;
		}
		else {
			this->pduPool[id].next = &this->pduPool[id + 1];
		}
	}
	this->usedCounter = 0;
}

CAN_Pdu_t* CAN_Pdu::pduNew() {
	CAN_Pdu_t* pduRet = this->freePduPool;

	if (CAN_PDU_NULL != pduRet) {
		pduRet->isUsed = true;
		this->freePduPool = this->freePduPool->next;

		++this->usedCounter;
	}
	else {
		FATAL("PDU", 0x01);
	}
	return pduRet;
}

bool CAN_Pdu::isSpaceAvaiable() {
	return (this->usedCounter < CAN_PDU_POOL_SIZE);
}

CAN_Pdu_t* CAN_Pdu::getPdu(uint8_t pduId) {
	CAN_Pdu_t* pduRet = CAN_PDU_NULL;

	if ((pduId < CAN_PDU_POOL_SIZE) && this->pduPool[pduId].isUsed) {
		pduRet = &this->pduPool[pduId];
	}
	else {
		FATAL("PDU", 0x02);
	}
	return pduRet;
}

uint8_t CAN_Pdu::getSizeUsed() {
	return this->usedCounter;
}

uint8_t CAN_Pdu::getSizeMax() {
	return CAN_PDU_POOL_SIZE;
}

void CAN_Pdu::freePdu(uint8_t pduId) {
	if (pduId < CAN_PDU_POOL_SIZE && this->pduPool[pduId].isUsed) {
		this->pduPool[pduId].isUsed = false;
		this->pduPool[pduId].next = this->freePduPool;
		this->pduPool[pduId].len = 0;
		this->freePduPool = &this->pduPool[pduId];

		--this->usedCounter;
	}
	else {
		FATAL("PDU", 0x03);
	}
}

void CAN_Pdu::freePdu(CAN_Pdu_t *ptrPdu) {
	if ((ptrPdu != CAN_PDU_NULL) && 
		(ptrPdu->pduId < CAN_PDU_POOL_SIZE) &&
		ptrPdu->isUsed)
	{
		ptrPdu->isUsed = false;
		ptrPdu->next = this->freePduPool;
		ptrPdu->len = 0;
		this->freePduPool = ptrPdu;

		--this->usedCounter;
	}
	else {
		FATAL("PDU", 0x04);
	}
}

/*--------------------------------------------------------------------------------------------*/
void u8Queue::QueueInit() {
	this->rear = 0;
	this->front = 0;
	this->counter = 0;
	memset(this->u8Buf, 0, CAN_PDU_POOL_SIZE);
}

bool u8Queue::isFull() {
	return (CAN_PDU_POOL_SIZE == this->counter);
}

bool u8Queue::isEmpty() {
	return (this->counter == 0);
}

bool u8Queue::isAvailable() {
	return (this->counter > 0);
}

uint8_t u8Queue::readFront() {
	uint8_t ret = 0;

	if (u8Queue::isAvailable() == true) {
		ret = this->u8Buf[this->front];
	}
	return ret;
}

uint8_t u8Queue::readRear() {
	uint8_t ret = 0;

	if (u8Queue::isAvailable() == true) {
		ret = this->u8Buf[this->rear - 1];
	}
	return ret;
}

int8_t u8Queue::enQueue(uint8_t Id) {
	uint8_t nextInd;

	if (u8Queue::isFull() == false) {
		this->u8Buf[this->rear] = Id;
		nextInd = (++this->rear) % CAN_PDU_POOL_SIZE;
		this->rear = nextInd;

		++this->counter;
		return 1;
	}
	return -1;
}

uint8_t u8Queue::deQueue() {
	uint8_t ret = 0;
	uint8_t nextInd;

	if (u8Queue::isEmpty() == false) {
		ret = this->u8Buf[this->front];
		nextInd = (++this->front) % CAN_PDU_POOL_SIZE;
		this->front = nextInd;

		--this->counter;
	}
	return ret;
}