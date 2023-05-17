/*  Version: 1.1.3 |  License: Apache 2.0  |  Author: JDWifWaf@gmail.com   */
/*  Author: HungPNQ@fpt.com.vn | Date: 12/04/2023 | Brief: Port to STM32 Platform   */

#ifndef __tca9548a_H
#define __tca9548a_H

#include <stdio.h>
#include <stdint.h>

#include "io_cfg.h"

/* Channel hex values for writeRegister() function */
#define TCA_CHANNEL_0 0x01
#define TCA_CHANNEL_1 0x02
#define TCA_CHANNEL_2 0x04
#define TCA_CHANNEL_3 0x08
#define TCA_CHANNEL_4 0x10
#define TCA_CHANNEL_5 0x20
#define TCA_CHANNEL_6 0x40
#define TCA_CHANNEL_7 0x80

class I2CMuxTCA9548A {
public:
    I2CMuxTCA9548A(uint8_t address = 0x70);  /* Default IC Address */
    ~I2CMuxTCA9548A() {
        
    }
    void begin();
    bool openChannel(uint8_t channel);
    bool closeChannel(uint8_t channel);
    uint8_t getChannel();
    bool writeRegister(uint8_t value);
    inline bool readRegister(uint8_t *outData, uint16_t len) {
        return this->portTcaRead(outData, len);
    }
    bool closeAll();
    bool openAll();
    
protected:

private:
    uint8_t m_addess;
    uint8_t m_channels;

    /* User Port function */
    bool portTcaSend(uint8_t *inData, uint16_t len);
    bool portTcaRead(uint8_t *outData, uint16_t len);
};

#endif