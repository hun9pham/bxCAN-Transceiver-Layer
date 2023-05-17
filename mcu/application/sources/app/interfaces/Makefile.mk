CFLAGS		+= -I./sources/app/interfaces
CPPFLAGS	+= -I./sources/app/interfaces

VPATH += sources/app/interfaces

ifeq ($(IF_LINK_OPTION),-DIF_LINK_UART_EN)
SOURCES_CPP += sources/app/interfaces/task_cpu_serial_if.cpp
endif

ifeq ($(IF_CAN_OPTION),-DIF_CAN_BUS_EN)
SOURCES_CPP += sources/app/interfaces/task_can_if.cpp
endif

