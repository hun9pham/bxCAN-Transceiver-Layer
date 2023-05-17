#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asm/termbits.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>

#include "can_usb.h"

int8_t USB_to_CAN::adapterInit(const char *ttyDev, uint32_t baud) {
    int result;
    struct termios2 tio;
    this->tty_fd = open(ttyDev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (this->tty_fd == -1) {
        printf( "open(%s) failed: %s\n", ttyDev, strerror(errno));
        return -1;
    }

    result = ioctl(this->tty_fd, TCGETS2, &tio);
    if (result == -1) {
        printf( "ioctl() failed: %s\n", strerror(errno));
        close(this->tty_fd);
        return -1;
    }

    tio.c_cflag &= ~CBAUD;
    tio.c_cflag = BOTHER | CS8 | CSTOPB;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    tio.c_ispeed = baud;
    tio.c_ospeed = baud;

    result = ioctl(this->tty_fd, TCSETS2, &tio);
    if (result == -1) {
        printf( "ioctl() failed: %s\n", strerror(errno));
        close(this->tty_fd);
        return -1;
    }

    return this->tty_fd;
}

bool USB_to_CAN::isTxCompleted() {
    int result;
    result = write(this->tty_fd, this->CanTxCtl.txBuf, this->CanTxCtl.txSize);
    return (result != -1);
}

bool USB_to_CAN::isRxCompleted() {
    uint8_t byte;

    int ret = read(this->tty_fd, &byte, sizeof(uint8_t));
    if (ret != -1) {
        switch (this->CanRxCtl.parserStatus) {
        case CANUSB_PARSER_STATE_SOF: {
            if (CANUSB_HEADER_SOF == byte) {
                this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_DLC;
            }
        }
        break;

        case CANUSB_PARSER_STATE_DLC: {
            this->CanRxCtl.len = (byte & 0x0F);
            memset(&this->CanRxCtl.IdFilter, 0, sizeof(uint32_t));
            this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_FILTERID;
        }
        break;

        case CANUSB_PARSER_STATE_FILTERID: {
            static uint8_t ind = 0;
            ((uint16_t*)&this->CanRxCtl.IdFilter)[ind++] = byte & 0x00FF;
            if (ind == sizeof(uint16_t)) { /* 2 Bytes (lsb & msb) */
                ind = 0;
                if (this->CanProperties.enFilterId) {
                    if (this->CanRxCtl.IdFilter == this->CanProperties.IdFilter) {
                        this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_DATA;
                    }
                    else {
                        this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_SOF;
                    }
                }
                else {
                    this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_DATA;
                }
            }
        }
        break;

        case CANUSB_PARSER_STATE_DATA: {
            static uint8_t ind = 0;
            this->CanRxCtl.rxBuf[ind++] = byte;
            if (ind == this->CanRxCtl.len) {
                ind = 0;
                this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_EOF;
            }
        }
        break;

        case CANUSB_PARSER_STATE_EOF: {
            this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_SOF;
            if (CANUSB_HEADER_EOF == byte) {
                return true;
            }
        }
        break;
        }
    }

    return false;
}

USB_to_CAN::USB_to_CAN() {
    memset(&this->CanRxCtl, 0, sizeof(this->CanRxCtl));
    memset(&this->CanTxCtl, 0, sizeof(this->CanTxCtl));
    this->CanRxCtl.parserStatus = CANUSB_PARSER_STATE_SOF;
}

USB_to_CAN::~USB_to_CAN() {
    close(this->tty_fd);
}

int8_t USB_to_CAN::Start(const char *ttyDev,
                            uint32_t baud,
                            uint32_t IdFilter, 
                            CanSpeed_t canSpd, 
                            CanFrameType_t frameType, 
                            CanMode_t mode)
{
    if (NULL == ttyDev) {
        return -1;
    }
    this->CanProperties.IdFilter = IdFilter;
    this->CanProperties.mode = mode;
    this->CanProperties.speed = canSpd;
    this->CanProperties.frameType = frameType;
    if (IdFilter != 0x00) {
        this->CanProperties.enFilterId = true;
    }
    else {
        this->CanProperties.enFilterId = false;
    }
    return USB_to_CAN::adapterInit(ttyDev, baud);
}

void USB_to_CAN::setIdFilter(bool en) {
    this->CanProperties.enFilterId = en;
}

int8_t USB_to_CAN::sendData(uint32_t destNodeId, uint8_t *payload, uint32_t len) {
#define FRAMETX_SOF_INDEX   0
#define FRAMETX_INFO_INDEX  1
#define FRAMETX_LSBID_INDEX 2
#define FRAMETX_MSBID_INDEX 3
#define FRAMETX_DATA_INDEX  4

    int8_t ret = 0;

    if (this->tty_fd == -1) {
        return CANUSB_NG;
    }

    /* Byte 0: Packet Start */
    this->CanTxCtl.txBuf[0] = CANUSB_HEADER_SOF;

    /* Byte 1: CAN Bus Data Frame Information */
    this->CanTxCtl.txBuf[1] = 0x00;
    /* Bit 7 Always 1, Bit 6 Always 1 */
    this->CanTxCtl.txBuf[1] |= (1 << 7);
    this->CanTxCtl.txBuf[1] |= (1 << 6); 

    if (CanProperties.frameType == CANUSB_FRAME_STANDARD) { /* Bit 5 */
        this->CanTxCtl.txBuf[1] &= 0xDF; /* STD txBuf */
    }
    else { /* CANUSB_FRAME_EXTENDED */
        this->CanTxCtl.txBuf[1] |= 0x20; /* EXT txBuf */
    }

    /* Byte 2 to 3: ID */
    this->CanTxCtl.txBuf[2] = (destNodeId) & 0xFFFF; /* lsb */
    this->CanTxCtl.txBuf[3] = (destNodeId >> 16) & 0xFFFF; /* msb */

    /* Byte 4 to (4+data_len): Data */
    uint8_t nbrByteSends;
    while (len) {
        if (len > 8) {
            nbrByteSends = 8;
        }
        else {
            nbrByteSends = len;
        }

        this->CanTxCtl.txBuf[1] &= 0xE0; /* Reset nibble low  */
        this->CanTxCtl.txBuf[1] |= nbrByteSends; /* DLC=data_len */

        for (uint32_t id = 0; id < nbrByteSends; id++) {
            this->CanTxCtl.txBuf[4 + id] = payload[id];
        }
        /* Last byte: End of txBuf */
        this->CanTxCtl.txBuf[4 + nbrByteSends] = CANUSB_HEADER_EOF;

        /* SOF(1Byte) + FrameINFO(1Byte) + ID(2Bytes) + DataLength[...] + EOF(1Byte) */
        this->CanTxCtl.txSize = 4 + nbrByteSends + 1;
        ret += USB_to_CAN::isTxCompleted();
        len -= nbrByteSends;
        payload += nbrByteSends;
        usleep(10000); /* 10ms */
    }

    return (ret == 0 ? CANUSB_NG : CANUSB_OK);
}

int8_t USB_to_CAN::recvData() {
    if (this->tty_fd == -1) {
        return CANUSB_NG;
    }

    if (true == USB_to_CAN::isRxCompleted()) {
        return CANUSB_OK;
    }
    return CANUSB_NG;
}

USB_to_CAN UsbCan;