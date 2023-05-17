/*
    DS1307.h
    library for Seeed RTC module
    Copyright (c) 2013 seeed technology inc.
    Author        :   FrankieChu
    Create Time   :   Jan 2013
    Change Log    :
    The MIT License (MIT)
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    ----------------------------------------------------------------------------
    Author        :   HungPNQ
    Create Time   :   Apr 2023
    Modification  :   Clone and port to STM32L151C8T6
    ----------------------------------------------------------------------------
*/
#ifndef __Rtc_Ds1307_H
#define __Rtc_Ds1307_H

#include <stdio.h>
#include <stdint.h>

#include "stm32xx_headers.h"

#define RtcDs1307_DBG_EN        (1)
#define RtcDs1307_RETOK         (1)

#if (RtcDs1307_DBG_EN)
#define rtcDBG_PRINT(tag, fmt, ...)        xprintf(KRED "[" tag "] " KBLU fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define rtcDBG_PRINT(tag, fmt, ...)
#endif

#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6
#define SUN 7

class rtcDs1307 {
private:
    uint8_t __addr = 0x68;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t date;
    uint8_t day;
    uint8_t month;
    uint16_t year;

    uint8_t decToBcd(uint8_t val);
    uint8_t bcdToDec(uint8_t val);

    /* Function Portal */
    void initRtcIIC();
    int8_t readRtcIIC(uint8_t addr, uint8_t *data, uint16_t len);
    int8_t writeRtcIIC(uint8_t addr, uint8_t *data, uint16_t len);

public:
    void begin();
    int8_t setTime(void);
    int8_t getTime(void);
    void fillByHMS(uint8_t hour, uint8_t minute, uint8_t second);
    void fillByYMD(uint16_t year, uint8_t month, uint8_t day);
    void fillDayOfWeek(uint8_t _dow);
};

extern rtcDs1307 timRtc;

#endif