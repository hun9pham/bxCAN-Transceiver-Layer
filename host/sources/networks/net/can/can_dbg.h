#ifndef __CAN_DBG_H
#define __CAN_DBG_H

#include <stdio.h>

#define CAN_PRINT_EN                (1)
#define CAN_DBG_EN                  (1)
#define CAN_DBG_SIG_EN              (1)

#if (CAN_PRINT_EN == 1)
#define CAN_PRINT                   printf
#else
#define CAN_PRINT
#endif

#if (CAN_DBG_EN == 1)
#define CAN_DBG(tag, fmt, ...)      CAN_PRINT(KRED "[" tag "] " KGRN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define CAN_DBG(tag, fmt, ...)
#endif

#if (CAN_DBG_SIG_EN == 1)
#define CAN_DBG_SIG(tag, fmt, ...)  CAN_PRINT(KCYN "[SIG] " KRED tag " -> " KYEL fmt "\r\n" KNRM, ##__VA_ARGS__)
#else
#define CAN_DBG_SIG(tag, fmt, ...)
#endif

#endif /* __CAN_DBG_H */
