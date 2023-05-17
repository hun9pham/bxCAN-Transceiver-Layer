#include "ds1307.h"

#define TAG	"rtcDs1307"


rtcDs1307 timRtc;

uint8_t rtcDs1307::decToBcd(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

uint8_t rtcDs1307::bcdToDec(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

void rtcDs1307::begin() {
    rtcDs1307::initRtcIIC();
}

int8_t rtcDs1307::getTime() {
	int8_t ret = RtcDs1307_RETOK;
    uint8_t timRTC[7];
    uint8_t rtcCmd = 0x00;

	ret = writeRtcIIC(this->__addr, &rtcCmd, sizeof(uint8_t));
    if (ret < 0) {
        rtcDBG_PRINT(TAG, "getTime::writeRtcIIC()->NG | ret: %d", ret);
        return ret;
    }

	ret = readRtcIIC(this->__addr, &timRTC[0], 7);
    if (ret < 0) {
        rtcDBG_PRINT(TAG, "getTime::writeRtcIIC()->NG | ret: %d", ret);
        return ret;
    }

    rtcDBG_PRINT(TAG, "T%d, %02d/%02d/%2d - %02d:%02d:%02d", 
        bcdToDec(timRTC[3]),
        bcdToDec(timRTC[4]),
        bcdToDec(timRTC[5]),
        bcdToDec(timRTC[6]),
        bcdToDec(timRTC[2]),
        bcdToDec(timRTC[1]),
        bcdToDec(timRTC[0])
    );

	return ret;
}

int8_t rtcDs1307::setTime() {
	int8_t ret = RtcDs1307_RETOK;
    uint8_t timRTC[8];

    timRTC[0] = 0x00;
    timRTC[1] = decToBcd(this->second);
    timRTC[2] = decToBcd(this->minute);
    timRTC[3] = decToBcd(this->hour);
    timRTC[4] = decToBcd(this->date);
    timRTC[5] = decToBcd(this->day);
    timRTC[6] = decToBcd(this->month);
    timRTC[7] = decToBcd(this->year);

	ret = writeRtcIIC(this->__addr, &timRTC[0], 8);
    if (ret < 0) {
        rtcDBG_PRINT(TAG, "setTime::writeRtcIIC()->NG | ret: %d", ret);
        return ret;
    }

	return ret;
}

void rtcDs1307::fillByHMS(uint8_t hour, uint8_t minute, uint8_t second) {
    this->hour = hour;
    this->minute = minute;
    this->second = second;
}

void rtcDs1307::fillByYMD(uint16_t year, uint8_t month, uint8_t day) {
    this->year = year - 2000;
    this->month = month;
    this->day = day;
}

void rtcDs1307::fillDayOfWeek(uint8_t date) {
    this->date = date;
}

void rtcDs1307::initRtcIIC() {
    I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_I2C1_SCL_PIN | IO_I2C1_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(IO_I2C1_PORT, &GPIO_InitStructure);
	
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_ClockSpeed = speed;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

int8_t rtcDs1307::readRtcIIC(uint8_t addr, uint8_t *data, uint16_t len) {
    volatile uint16_t tim = 0xFFFF;

	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(tim-- == 0) {
			return -1;
		}
	}

	I2C_Send7bitAddress(I2C1, (addr << 1), I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		if(tim-- == 0) {
			return -2;
		}
	}

	while (len) {
		if (tim-- == 0) {
			return -3;
		}
		if (len == 1) {
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}

		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
			*data = I2C_ReceiveData(I2C1);
			data++;
			len--;
		}
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	return 1;
}

int8_t rtcDs1307::writeRtcIIC(uint8_t addr, uint8_t *data, uint16_t len) {
    volatile uint16_t tim = 0xFFFF;

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if (tim-- == 0) {
			

			return -1;	
		}
	}

	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if (tim-- == 0) {
			return -2;
		}
	}

	I2C_Send7bitAddress(I2C1, (addr << 1), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (tim-- == 0) {
			return -3;
		}
	}

	while (len--) {
		I2C_SendData(I2C1, *data++);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  {
			if (tim-- == 0) {
				return -4;
			}
		}
	}
	I2C_GenerateSTOP(I2C1, ENABLE);

	return 1;
}