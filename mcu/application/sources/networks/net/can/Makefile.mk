CFLAGS		+= -I./sources/networks/net/can
CPPFLAGS	+= -I./sources/networks/net/can

VPATH += sources/networks/net/can

SOURCES_CPP += sources/networks/net/can/can_data.cpp
SOURCES_CPP += sources/networks/net/can/can_low_layer.cpp
SOURCES_CPP += sources/networks/net/can/can_high_layer.cpp
