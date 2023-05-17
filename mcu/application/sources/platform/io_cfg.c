#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "sys_log.h"

#define TAG	"IOCfg"

/* Extern variables ----------------------------------------------------------*/
ring_buffer_char_t uartIfRx;
ring_buffer_char_t canFrameRx;

/* Private variables ---------------------------------------------------------*/
static uint8_t uartIfRxBuf[CPU_SERIAL_IF_BUFFER_SIZE];
static uint8_t canFrameRxBuf[CANRX_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void IO_RCC_OpenClk() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void ledLifeInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_LED_LIFE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_LED_LIFE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ledLifeBlink() {
	static uint8_t invertStatus = 0;

	invertStatus = !invertStatus;
	GPIO_WriteBit(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN, invertStatus ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_6, invertStatus ? Bit_SET : Bit_RESET);
}

void flashSpiInit() {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_CLK_PIN | IO_FLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
}

void flashCsInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_FLASH_CS_PORT, &GPIO_InitStructure);
}

void ctrlFlashCs(uint8_t st) {
	GPIO_WriteBit(IO_FLASH_CS_PORT, IO_FLASH_CS_PIN, st ? Bit_SET : Bit_RESET);
}

uint8_t transfFlashData(uint8_t data) {
	uint16_t rxtxData = (uint16_t)data;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, rxtxData);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)rxtxData;
}

void cpuSerialIfInit(void) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_CPU_SERIAL_IF_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_Cmd(USART2, ENABLE);

	ring_buffer_char_init(&uartIfRx, uartIfRxBuf, CPU_SERIAL_IF_BUFFER_SIZE);
}

void putCpuSerialIfData(uint8_t data) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, (uint8_t)data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void bxCanInit(uint32_t IdFilter) {
#define bxCAN_1MBps_EN		(0)
#define bxCAN_125KBps_EN	(1)

	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

	GPIO_InitStructure.GPIO_Pin = IO_CAN1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(IO_CAN1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_CAN1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_CAN1_PORT, &GPIO_InitStructure);

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM = DISABLE; /* Time-triggered communication mode */
	CAN_InitStructure.CAN_ABOM = DISABLE; /* Automatic bus-off management */
	CAN_InitStructure.CAN_AWUM = DISABLE; /* Automatic wake-up mode */
	CAN_InitStructure.CAN_NART = DISABLE; /* Non-automatic retransmission mode */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* Receive FIFO locked mode */
	CAN_InitStructure.CAN_TXFP = DISABLE; /* Transmit FIFO priority */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/* 
	 - Baudrate = 36000000 / (CAN_Prescaler * (CAN_SJW + CAN_BS1 + CAN_BS2)
	*/
#if bxCAN_1MBps_EN
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 4;
#endif
#if bxCAN_125KBps_EN
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 48;
#endif
	CAN_Init(CAN1, &CAN_InitStructure);	
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = IdFilter << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ring_buffer_char_init(&canFrameRx, canFrameRxBuf, CANRX_BUFFER_SIZE);
}

static uint8_t bxCanSendU8(uint32_t nodeId, uint8_t *data, uint8_t len) {
	CanTxMsg txCanHeader;

	if (len > 8) { /* CAN Bus max byte transfer is 8 */
		return -1;
	}

	txCanHeader.StdId = nodeId;
	txCanHeader.ExtId = 0x0000;
	txCanHeader.RTR = CAN_RTR_DATA;
	txCanHeader.IDE = CAN_ID_STD;
	txCanHeader.DLC = len;
	memcpy(&txCanHeader.Data[0], data, len);

#if 0 /* Debug hex bytes transmit */
    for (uint8_t id = 0; id < len; ++id) {
        SYS_PRINT("0x%X ", txCanHeader.Data[id]);
    }
    SYS_PRINT("\r\n");
#endif 
	uint8_t txMboxId = CAN_Transmit(CAN1, &txCanHeader);
	if (CAN_NO_MB == txMboxId) {
		return -2;
	}

	uint16_t timOut = 0xFFFF;
	while (timOut > 0) {
		if (CANTXOK == CAN_TransmitStatus(CAN1, txMboxId)) {
			break;
		}
		--timOut;
	}

	if (timOut == 0) {
		CAN_CancelTransmit(CAN1, txMboxId);
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EWG)) CAN_ClearFlag(CAN1, CAN_FLAG_EWG); /* ErrorWarningInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EPV)) CAN_ClearFlag(CAN1, CAN_FLAG_EPV); /* ErrorPassiveInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF)) CAN_ClearFlag(CAN1, CAN_FLAG_BOF); /* BusOffInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_LEC)) CAN_ClearFlag(CAN1, CAN_FLAG_LEC); /* LastErrorCodeInterrupt */
	}

	return CAN_GetLastErrorCode(CAN1);
}

uint8_t bxCanSendData(uint32_t nodeId, uint8_t *data, uint16_t len) {
#define CAN_MAX_FRAMETX_SIZE	8

	uint8_t ret = 0;
	uint8_t packetLen = 0;

	while (len) {
		if (len > CAN_MAX_FRAMETX_SIZE) {
			packetLen = CAN_MAX_FRAMETX_SIZE;
		}
		else {
			packetLen = len;
		}
		ret += bxCanSendU8(nodeId, data, packetLen); /* Send MSB First */
		data += packetLen;
		len -= packetLen;
	}

	return ret;
}

uint8_t bxCanSendRemote(uint32_t nodeId) {
	CanTxMsg txCanHeader;
	
	txCanHeader.StdId = nodeId;
	txCanHeader.ExtId = 0x0000;
	txCanHeader.RTR = CAN_RTR_REMOTE;
	txCanHeader.IDE = CAN_ID_STD;
	txCanHeader.DLC = 0;

	uint8_t txMboxId = CAN_Transmit(CAN1, &txCanHeader);
	if (CAN_NO_MB == txMboxId) {
		return -1;
	}

	uint16_t timOut = 0xFFFF;
	while (timOut > 0) {
		if (CANTXOK == CAN_TransmitStatus(CAN1, txMboxId)) {
			break;
		}
		--timOut;
	}

	if (timOut == 0) {
		CAN_CancelTransmit(CAN1, txMboxId);
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EWG)) CAN_ClearFlag(CAN1, CAN_FLAG_EWG); /* ErrorWarningInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EPV)) CAN_ClearFlag(CAN1, CAN_FLAG_EPV); /* ErrorPassiveInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF)) CAN_ClearFlag(CAN1, CAN_FLAG_BOF); /* BusOffInterrupt */
		if (CAN_GetFlagStatus(CAN1, CAN_FLAG_LEC)) CAN_ClearFlag(CAN1, CAN_FLAG_LEC); /* LastErrorCodeInterrupt*/
	}

	return CAN_GetLastErrorCode(CAN1);
}