#ifndef __CAN_USB_H
#define __CAN_USB_H

#include <stdint.h>

#define CANUSB_TTY_BAUD_RATE_DEFAULT        (2000000)

#define CANUSB_HEADER_SOF                   (0xAA)  /* Start of frame */
#define CANUSB_HEADER_EOF                   (0x55)  /* End of frame */
#define CANUSB_FRAME_SIZE_MAX               (13)

#define CANUSB_OK                           (1)
#define CANUSB_NG                           (0)

typedef enum eCAN_Speed {
    CANUSB_SPEED_1000000 = 0x01,
    CANUSB_SPEED_800000  = 0x02,
    CANUSB_SPEED_500000  = 0x03,
    CANUSB_SPEED_400000  = 0x04,
    CANUSB_SPEED_250000  = 0x05,
    CANUSB_SPEED_200000  = 0x06,
    CANUSB_SPEED_125000  = 0x07,
    CANUSB_SPEED_100000  = 0x08,
    CANUSB_SPEED_50000   = 0x09,
    CANUSB_SPEED_20000   = 0x0a,
    CANUSB_SPEED_10000   = 0x0b,
    CANUSB_SPEED_5000    = 0x0c,
} CanSpeed_t;

typedef enum eCAN_Mode {
    CANUSB_MODE_NORMAL          = 0x00,
    CANUSB_MODE_LOOPBACK        = 0x01,
    CANUSB_MODE_SILENT          = 0x02,
    CANUSB_MODE_LOOPBACK_SILENT = 0x03,
} CanMode_t;

typedef enum eCAN_Frame {
    CANUSB_FRAME_STANDARD = 0x01,
    CANUSB_FRAME_EXTENDED = 0x02,
} CanFrameType_t;

enum eReceiveParserState {
    CANUSB_PARSER_STATE_SOF,
    CANUSB_PARSER_STATE_DLC,
    CANUSB_PARSER_STATE_FILTERID,
    CANUSB_PARSER_STATE_DATA,
    CANUSB_PARSER_STATE_EOF,
};

class USB_to_CAN {
private:
    int tty_fd;

    struct {
        uint8_t parserStatus;
        uint32_t IdFilter;
        uint8_t rxBuf[8];
        uint8_t len;
    } CanRxCtl;

    struct {
        uint8_t txSize;
        uint8_t txBuf[CANUSB_FRAME_SIZE_MAX];
    } CanTxCtl;

    struct {
        uint32_t IdFilter;
        CanMode_t mode;
        CanSpeed_t speed;
        CanFrameType_t frameType;
        bool enFilterId;
    } CanProperties;

    int8_t adapterInit(const char *ttyDev, uint32_t baud);
    bool isTxCompleted();
    bool isRxCompleted();

protected:

public:
    USB_to_CAN();
    ~USB_to_CAN();
    int8_t Start(const char *ttyDev, uint32_t baud, uint32_t IdFilter, CanSpeed_t canSpd, CanFrameType_t frameType, CanMode_t mode);
    void setIdFilter(bool en);
    int8_t sendData(uint32_t destNodeId, uint8_t *payload, uint32_t len);
    int8_t recvData(void);

    uint8_t *getRxData() {
        return this->CanRxCtl.rxBuf;
    }
    uint8_t getRxSize() {
        return this->CanRxCtl.len;
    }
};

extern USB_to_CAN UsbCan;

#endif /* __CAN_USB_H */