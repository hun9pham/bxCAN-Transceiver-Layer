#include "pca9554a.h"

#include "io_cfg.h"

ExpandIO_PCA9554A::ExpandIO_PCA9554A(uint8_t address) : m_addess(address) {
    
}

bool ExpandIO_PCA9554A::portMode(uint8_t mode) {
#if 1
	uint8_t byteSend = mode;
	return this->portPcaSend(&byteSend, sizeof(uint8_t));
#else
	uint16_t byteSend = 0x0000;

	byteSend |= (CONFIGPORT << 8) & 0xFF00;
	byteSend |= mode & 0x00FF;
	return this->portPcaSend((uint8_t*)&byteSend, sizeof(uint16_t));
#endif
}

bool ExpandIO_PCA9554A::setPin(uint8_t id, bool state) {
	uint8_t portState;

	if (id > 7) {
		return false;
	}
	if(this->portPcaRead(&portState, sizeof(uint8_t))) {
		if (!state) {
			portState &= ~(1 << id);
		}
		else {
			portState |= (1 << id);
		}
		return this->setPort(portState);
	}
	return false;
}

bool ExpandIO_PCA9554A::setPort(uint8_t val) {
#if 1
	uint8_t byteSend = val;
	return this->portPcaSend(&byteSend, sizeof(uint8_t));
#else
	uint16_t byteSend = 0x0000;

	byteSend |= (OUTPUTPORT << 8) & 0xFF00;
	byteSend |= val & 0x00FF;
	return this->portPcaSend((uint8_t*)&byteSend, sizeof(uint16_t));
#endif
}

bool ExpandIO_PCA9554A::readPin(uint8_t id, uint8_t *ret) {
	if (id > 7) {
		return false;
	}

	if (this->portPcaRead(ret, sizeof(uint8_t))) {
		*ret = ((*ret >> id) & 0x01);
		return true;
	}
	return false;
}

bool ExpandIO_PCA9554A::readPort(uint8_t *ret) {
	if (this->portPcaRead(ret, sizeof(uint8_t))) {
		return true;
	}
	return false;
}

bool ExpandIO_PCA9554A::portPcaSend(uint8_t *inData, uint16_t len) {
	if (mtIICSend(this->m_addess, inData, len)) {
        return true;
    }
    return false;
}

bool ExpandIO_PCA9554A::portPcaRead(uint8_t *outData, uint16_t len) {
	if (mtIICRead(this->m_addess, outData, len)) {
        return true;
    }
    return false;
}
