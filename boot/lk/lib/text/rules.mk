LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/dev/display

MODULES += lib/font \
  lib/gfx

OBJS += \
	$(LOCAL_DIR)/text.o
