LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include

OBJS += \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/recovery.o

