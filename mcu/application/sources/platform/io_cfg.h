#ifndef __IO_CFG_H
#define __IO_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ring_buffer.h"

#include "stm32xx_headers.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: NeoPixelRGB Status pin map define
 *  Note: MLT 8530 (Resonant Frequency: 2700Hz) -> T = 0.0003s = 0.3ms = 300 us
 *----------------------------------------------------------------------------*/
#define IO_LED_LIFE_PIN				( GPIO_Pin_13 )
#define IO_LED_LIFE_PORT			( GPIOC )

/*----------------------------------------------------------------------------*
 *  DECLARE: CPU Serial interface
 *  Note: Communication with PC, Pi, ...
 *----------------------------------------------------------------------------*/
#define UART_CPU_SERIAL_IF_BAUD     ( 115200 )
      
#define IO_CPU_SERIAL_IF_PORT       ( GPIOA )
#define IO_CPU_SERIAL_IF_TX_PIN     ( GPIO_Pin_2 )
#define IO_CPU_SERIAL_IF_RX_PIN     ( GPIO_Pin_3 )

#define CPU_SERIAL_IF_BUFFER_SIZE	( 256 )

/*----------------------------------------------------------------------------*
 *  DECLARE: Flash SPI
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_FLASH_SPI_CLK_PIN        ( GPIO_Pin_5 )
#define IO_FLASH_SPI_MISO_PIN       ( GPIO_Pin_6 )
#define IO_FLASH_SPI_MOSI_PIN       ( GPIO_Pin_7 )
#define IO_FLASH_SPI_PORT           ( GPIOA )

#define IO_FLASH_CS_PIN      		( GPIO_Pin_4 )
#define IO_FLASH_CS_PORT			( GPIOA )

/*----------------------------------------------------------------------------*
 *  DECLARE: bxCAN bus
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_CAN1_TX_PIN               ( GPIO_Pin_12 )
#define IO_CAN1_RX_PIN               ( GPIO_Pin_11 )
#define IO_CAN1_PORT                 ( GPIOA )

#define CANRX_BUFFER_SIZE            ( 256 )

/*----------------------------------------------------------------------------*
 *  DECLARE: IO Commons definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define GetBit(byte, ind)     ((byte >> ind) & 0x01)
#define SetBit(byte, ind)     (byte & (~(1 << (ind))))
#define ToggleBit(byte, ind)  (byte ^ (1 << (ind)))

#define TIMER_FREQ_1KHZ         ( SystemCoreClock / 1000 )
#define TIMER_FREQ_10KHZ        ( SystemCoreClock / 10000 )
#define TIMER_FREQ_100KHZ       ( SystemCoreClock / 100000 )
/* 
 *						SystemCoreClock
 * TIM_Freq =	____________________________________
 *				(TIM_Prescaler + 1)x(TIM_Period + 1)
 *
 *
 *				(TIM_Period + 1) * Duty_Cycle_PWM
 * TIM_Pulse =	___________________________________ - 1
 *								100
 *
*/

/* Enumarics -----------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern ring_buffer_char_t uartIfRx;
extern ring_buffer_char_t canFrameRx;

/* Function prototypes -------------------------------------------------------*/
extern void IO_RCC_OpenClk();
extern void ledLifeInit();
extern void flashSpiInit();
extern void flashCsInit();
extern void cpuSerialIfInit();
extern void bxCanInit(uint32_t IdFilter);

extern void ledLifeBlink();
extern void ctrlFlashCs(uint8_t st);
extern uint8_t transfFlashData(uint8_t data);
extern uint8_t getCpuSerialIfData();
extern void putCpuSerialIfData(uint8_t data);

extern uint8_t bxCanSendData(uint32_t nodeId, uint8_t *data, uint16_t len); /* Send data from another node on CAN Bus */
extern uint8_t bxCanSendRemote(uint32_t nodeId); /* Request data from another node on CAN Bus */

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */

