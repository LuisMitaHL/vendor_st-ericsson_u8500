
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/db_shared/include \
	-I$(LOCAL_DIR)/arch/arm/include \
	-I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/boot.o

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),)
OBJS += \
	$(LOCAL_DIR)/crashdump.o \
	$(LOCAL_DIR)/hats.o

ifeq ($(ENABLE_FASTBOOT),1)
OBJS += \
	$(LOCAL_DIR)/fastboot.o
endif
endif
