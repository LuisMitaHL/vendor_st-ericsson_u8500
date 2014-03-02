ifeq ($(ENABLE_FEATURE_BUILD_HBTS),)
LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(USB_USE_MENTOR),1)
OBJS += \
	$(LOCAL_DIR)/mentor_usb.o
else
OBJS += \
	$(LOCAL_DIR)/usb.o
endif
endif

