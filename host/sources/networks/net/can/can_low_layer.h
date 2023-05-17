#ifndef __CAN_LOW_LAYER_H
#define __CAN_LOW_LAYER_H

#include <stdint.h>
#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Enumarics -----------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskCanLowLayerMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskCanLowLayerEntry(void*);

#endif /* __CAN_LOW_LAYER_H */
