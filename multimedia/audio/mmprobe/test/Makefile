include $(MM_MAKEFILES_DIR)/AudioConfig.mk

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)
endif

CFLAGS += -I$(COMPONENT_TOP_DIR)/../api

VPATH += \
api

ifeq ($(MMPROCESSOR),cortexA9)
        EXE_LDFLAGS += -L../lib/cortexA9-android-android-href_v1 -lmmprobe
else ifeq ($(MMPROCESSOR),x86)
        EXE_LDFLAGS += -L../lib/x86 -lmmprobe
endif

#EXE_LDFLAGS += -L../lib/cortexA9-android-android-href_v1 -lmmprobe

EXENAME = probe
EXEOBJ = probe

todo: exe

include $(MM_MAKEFILES_DIR)/SharedRules.mk
