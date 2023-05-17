#ifndef __pca9554a_H
#define __pca9554a_H

#include <stdio.h>
#include <stdint.h>

/* Byte Command */
#define INPUTPORT	0x00	/* Input Port register */
#define OUTPUTPORT	0x01	/* Output Port register */
#define POLINVPORT	0x02	/* Polarity Inversion register */
#define CONFIGPORT	0x03	/* Configuration register */

#define ALLOUTPUT	0x00
#define ALLINPUT	0xFF

#define NORMAL      0
#define INVERTED    1

class ExpandIO_PCA9554A {
public:
	ExpandIO_PCA9554A(uint8_t address = 0x38); /* Default IC Address */
	~ExpandIO_PCA9554A() {

	}
	uint8_t getAddress() {
		return this->m_addess;
	}
	bool portMode(uint8_t mode);
	bool setPin(uint8_t id, bool state);
	bool setPort(uint8_t val);
	bool readPin(uint8_t id, uint8_t *ret);
	bool readPort(uint8_t *ret);
	
private:
	uint8_t m_addess;

	/* User Port function */
    bool portPcaSend(uint8_t *inData, uint16_t len);
    bool portPcaRead(uint8_t *outData, uint16_t len);
};

#endif
