CXXFLAGS	+= -I./sources/networks/net/can

VPATH += sources/networks/net/can

OBJ += $(OBJ_DIR)/can_usb.o
OBJ += $(OBJ_DIR)/can_data.o
OBJ += $(OBJ_DIR)/can_low_layer.o
OBJ += $(OBJ_DIR)/can_high_layer.o
