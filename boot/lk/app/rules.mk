LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
#	lib/openssl

INCLUDES += -I$(LK_TOP_DIR)/include/lib

OBJS += \
	$(LOCAL_DIR)/app.o

