#ifndef __SYS_CTL_H
#define __SYS_CTL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "sys_boot.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: System control defines
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void softReset();
extern void watchdogRst();

/* Delay 3 cycles clock of system */
extern void delayAsm(uint32_t __cnt);

extern void delayTickMs(uint16_t ms);
extern void delayTickUs(uint32_t us);
extern uint32_t millisTick(void);
extern uint32_t microsTick(void);
extern void delayMicroseconds(uint32_t t);
extern void delayMilliseconds(uint32_t t);

/* Internal flash function */
extern void internalFlashUnlock(void);
extern void internalFlashLock(void);
extern void internalFlashEraseCalc(uint32_t addr, uint32_t Len);
extern uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* pData, uint32_t Len);
extern void internalFlashClearFlag(void);

/* TIM3 */
extern uint16_t tim3GetPeriod();
extern uint16_t tim3GetCounter();
extern void tim3Reload();

/* UART Console interface */
extern uint8_t consoleGetChar(void);
extern void consolePutChar(uint8_t dat);

/* System utilities */
extern void firmwareHeaderRead(firmwareHeader_t*);
extern char* getRstReason(bool clr);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CTL_H */
