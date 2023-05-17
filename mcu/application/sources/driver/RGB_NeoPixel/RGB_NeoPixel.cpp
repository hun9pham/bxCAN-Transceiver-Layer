#include "RGB_NeoPixel.h"

NeoPixelRGB::NeoPixelRGB() {

}

void NeoPixelRGB::begin(uint8_t lineLen) {
	this->lineLen =  lineLen;
	this->timShort = portTIM_PERIOD - (SystemCoreClock / 2500000); /* 0.4us = short pulse period */
	this->timLong = portTIM_PERIOD - (SystemCoreClock / 1250000);	/* 0.8us = long pulse period */
	NeoPixelRGB::clrAllNeoPixels();
}

void NeoPixelRGB::setAllNeopixels(rgbColors_t rgb) {
	for (uint8_t ledId = 0; ledId < this->ledLen; ++ledId) {
		NeoPixelRGB::setNeopixelNOUP(rgb, ledId);
	}
}

void NeoPixelRGB::setAllNeopixels(uint8_t red, uint8_t green, uint8_t blue) {
	for (uint8_t ledId = 0; ledId < this->ledLen; ++ledId) {
		NeoPixelRGB::setNeopixelNOUP(red, green, blue, ledId);
	}
}

void NeoPixelRGB::setNeopixel(rgbColors_t rgb, uint8_t ledId) {
	ledBufferGREEN[ledId] = rgb.green;
	ledBufferBLUE[ledId] = rgb.blue;
	ledBufferRED[ledId] = rgb.red;
}

void NeoPixelRGB::setNeopixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t ledId) {
	ledBufferRED[ledId] = red;
	ledBufferGREEN[ledId] = green;
	ledBufferBLUE[ledId] = blue;
}

void NeoPixelRGB::setNeopixelNOUP(rgbColors_t rgb, uint8_t ledId) {
	ledBufferRED[ledId] = rgb.red;
	ledBufferGREEN[ledId] = rgb.green;
	ledBufferBLUE[ledId] = rgb.blue;
}

void NeoPixelRGB::setNeopixelNOUP(uint8_t red, uint8_t green, uint8_t blue, uint8_t ledId) {
	ledBufferRED[ledId] = red;
	ledBufferGREEN[ledId] = green;
	ledBufferBLUE[ledId] = blue;
}

void NeoPixelRGB::clrAllNeoPixels() {
	memset(ledBufferRED, 0, LEDs_NeoPixel_Amount);
	memset(ledBufferGREEN, 0, LEDs_NeoPixel_Amount);
	memset(ledBufferBLUE, 0, LEDs_NeoPixel_Amount);
}

#pragma GCC push_options
#pragma GCC optimize ("Og")
void NeoPixelRGB::updateNeoPixel(uint8_t lineId) {
	uint16_t pinId;
	uint8_t waitTime;

	pinId = getLedDataOutPin(lineId);
	portLINE_PIN_SET(pinId, 0);

	/* Disable Interrupts for strictly timings have to be accurate */
	DISABLE_INTERRUPTS(); 

	for (uint8_t ledId = 0; ledId < this->ledLen; ledId++) {
		uint8_t mask = 0x80;
		uint8_t r = ledBufferRED[ledId];
		uint8_t g = ledBufferGREEN[ledId];
		uint8_t b = ledBufferBLUE[ledId];

		portTIM_RELOAD();

		for (uint8_t bit = 0; bit < 8; bit++) {
			waitTime = ((g & mask) ? this->timLong : this->timShort);

			portLINE_PIN_SET(pinId, 1);
			while (portTIM_GETCOUNNTER > waitTime);
			portLINE_PIN_SET(pinId, 0);
			while (portTIM_GETCOUNNTER <= waitTime);
			portTIM_RELOAD();
			mask = (mask >> 1);
		}

		mask = 0x80;
		portTIM_RELOAD();

		for (uint8_t bit = 0; bit < 8; bit++) {
			waitTime = ((r & mask) ? this->timLong : this->timShort);

			portLINE_PIN_SET(pinId, 1);
			while (portTIM_GETCOUNNTER > waitTime);
			portLINE_PIN_SET(pinId, 0);
			while (portTIM_GETCOUNNTER <= waitTime);
			portTIM_RELOAD();
			mask = (mask >> 1);
		}

		mask = 0x80;
		portTIM_RELOAD();

		for (uint8_t bit = 0; bit < 8; bit++) {
			waitTime = ((b & mask) ? this->timLong : this->timShort);

			portLINE_PIN_SET(pinId, 1);
			while (portTIM_GETCOUNNTER > waitTime);
			portLINE_PIN_SET(pinId, 0);
			while (portTIM_GETCOUNNTER <= waitTime);
			portTIM_RELOAD();
			mask = (mask >> 1);
		}
	}

	ENABLE_INTERRUPTS();
	delayTickMs(20);
}

void NeoPixelRGB::updateAllNeopixels() {
#if 0
	delayMilliseconds(10);
	
	uint32_t oldLOAD = SysTick->LOAD;
	uint32_t oldVAL = SysTick->VAL;

	uint8_t bitPeriod = SystemCoreClock / 800000; /*1.25us */
	uint8_t t0 = bitPeriod - (SystemCoreClock / 2500000); /* 0.4us */
	uint8_t t1 = bitPeriod - (SystemCoreClock / 1250000); /* 0.8us */

	SysTick->LOAD = bitPeriod - 1;
	SysTick->VAL = 0;

	ENTRY_CRITICAL();

	for (uint8_t led = 0; led < this->ledLen; led++) {
		uint8_t mask = 0x80;
		uint8_t r = ledBufferRED[led];
		uint8_t g = ledBufferGREEN[led];
		uint8_t b = ledBufferBLUE[led];

		SysTick->VAL = 0;
		for (uint8_t bit = 0; bit < 8; bit++) {
			uint8_t waittime = ((g & mask) ? t1 : t0);

			GPIOB->ODR |= 0b0001001100011011;
			while (SysTick->VAL > waittime);
			GPIOB->ODR &= ~(0b0001001100011011);
			while (SysTick->VAL <= waittime);
			SysTick->VAL = 0;
			mask = (mask >> 1);
		}

		mask = 0x80;
		SysTick->VAL = 0;
		for (uint8_t bit = 0; bit < 8; bit++) {
			uint8_t waittime = ((r & mask) ? t1 : t0);

			GPIOB->ODR |= 0b0001001100011011;
			while (SysTick->VAL > waittime);
			GPIOB->ODR &= ~(0b0001001100011011);
			while (SysTick->VAL <= waittime);
			SysTick->VAL = 0;
			mask = (mask >> 1);
		}

		mask = 0x80;
		SysTick->VAL = 0;
		for (uint8_t bit = 0; bit < 8; bit++) {
			uint8_t waittime = ((b & mask) ? t1 : t0);
			GPIOB->ODR |= 0b0001001100011011;
			while (SysTick->VAL > waittime);
			GPIOB->ODR &= ~(0b0001001100011011);
			while (SysTick->VAL <= waittime);
			SysTick->VAL = 0;
			mask = (mask >> 1);
		}
	}

	for (uint8_t cnt = 0; cnt < 30; cnt++) {
		SysTick->VAL = 0;
		while ((SysTick->VAL) >= 50);
	}

	SysTick->LOAD = oldLOAD;
	SysTick->VAL = oldVAL;

	EXIT_CRITICAL();
#else
	for (uint8_t lineId = 0; lineId < this->lineLen; ++lineId) {
		NeoPixelRGB::updateNeoPixel(lineId);
	}
#endif
}

#pragma GCC reset_options
