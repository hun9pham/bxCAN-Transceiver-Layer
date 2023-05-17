ifeq ($(IF_LINK_OPTION),-DIF_LINK_UART_EN)
include sources/networks/net/link/Makefile.mk
endif

ifeq ($(IF_CAN_OPTION),-DIF_CAN_BUS_EN)
include sources/networks/net/can/Makefile.mk
endif