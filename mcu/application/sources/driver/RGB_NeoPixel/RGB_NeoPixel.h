//-----------------------------------------------------------------------------
// Project   :  RGB NeoPixel Leds Strip (Model: WS2812B works with 800kHz)
// Author    :  HungPNQ
// Date      :  19/04/2023
// Brief     :  Using with STM32F103C8T6 to control multiple led Neopixel RGB
//				- Line0: Port B, Pin 0
//				- Line1: Port B, Pin 1
//				- Line2: Port B, Pin 3
//				- Line3: Port B, Pin 4
//				- Line4: Port B, Pin 8
//				- Line5: Port B, Pin 9
//-----------------------------------------------------------------------------
#ifndef __RGB_NeoPixel_H
#define __RGB_NeoPixel_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "app_dbg.h"

#include "stm32xx_headers.h"
#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#define LEDs_NeoPixel_Amount	(16)
#define Lines_NeoPixel_Amount	(6)

/**
 * User port function
 * This function access directly to IO register to limit control time handle
*/
#define portTIM_PERIOD			(TIM3->ARR)
#define portTIM_RELOAD()		tim3Reload()
#define portTIM_GETCOUNNTER		(TIM3->CNT)
#define portLINE_PIN_SET(i, s)	(s ? IO_DOn_PORT->BSRR = i : IO_DOn_PORT->BRR = i)
#define portLINE_PORT_SET(s)	(s ? IO_DOn_PORT->ODR |= IO_DOn_MASK : IO_DOn_PORT->ODR &= ~IO_DOn_MASK)

typedef struct t_NeoPixel_RGB_Colors {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} rgbColors_t;

class NeoPixelRGB {
public:
	NeoPixelRGB();
	~NeoPixelRGB() {

	}
	void begin(uint8_t lineLen = Lines_NeoPixel_Amount);
	void setNeopixel(rgbColors_t rgb, uint8_t ledId);
	void setNeopixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t ledId);
	void setAllNeopixels(rgbColors_t rgb);
	void setAllNeopixels(uint8_t red, uint8_t green, uint8_t blue);
	void clrAllNeoPixels();
	void updateNeoPixel(uint8_t lineId);
	void updateAllNeopixels();

protected:

private:
	void setNeopixelNOUP(rgbColors_t rgb, uint8_t ledId);
	void setNeopixelNOUP(uint8_t red, uint8_t green, uint8_t blue, uint8_t ledId);

	const uint8_t ledLen = LEDs_NeoPixel_Amount;
	uint8_t lineLen;
	uint8_t timShort;
	uint8_t timLong;

	uint8_t ledBufferRED[LEDs_NeoPixel_Amount];
	uint8_t ledBufferGREEN[LEDs_NeoPixel_Amount];
	uint8_t ledBufferBLUE[LEDs_NeoPixel_Amount];
};

#endif