/*  Version: 1.1.3  |  License: Apache 2.0  |  Author: JDWifWaf@gmail.com   */
/*  Author: HungPNQ@fpt.com.vn | Date: 12/04/2023 | Brief: Port to STM32 Platform   */

#include "tca9548a.h"

I2CMuxTCA9548A::I2CMuxTCA9548A(uint8_t address) : m_addess(address) {
    
}

void I2CMuxTCA9548A::begin() {
    this->closeAll();
}

uint8_t I2CMuxTCA9548A::getChannel() {
    return this->m_channels;
}

bool I2CMuxTCA9548A::openChannel(uint8_t channel) {
    this->m_channels |= (1 << channel);
    return this->portTcaSend(&this->m_channels, sizeof(uint8_t));
}

bool I2CMuxTCA9548A::closeChannel(uint8_t channel) {
    this->m_channels ^= (1 << channel);
    return this->portTcaSend(&this->m_channels, sizeof(uint8_t));
}

bool I2CMuxTCA9548A::closeAll() {
    this->m_channels = 0x00;
    return this->portTcaSend(&this->m_channels, sizeof(uint8_t));
}

bool I2CMuxTCA9548A::openAll() {
    this->m_channels = 0xFF;
    return this->portTcaSend(&this->m_channels, sizeof(uint8_t));
}

bool I2CMuxTCA9548A::writeRegister(uint8_t value) {
    this->m_channels = value;
    return this->portTcaSend(&this->m_channels, sizeof(uint8_t));
}

bool I2CMuxTCA9548A::portTcaSend(uint8_t *inData, uint16_t len) {
    if (mtIICSend(this->m_addess, inData, len) == 1) {
        return true;
    }
    return false;
}

bool I2CMuxTCA9548A::portTcaRead(uint8_t *outData, uint16_t len) {
    if (mtIICRead(this->m_addess, outData, len) == 1) {
        return true;
    }
    return false;
}