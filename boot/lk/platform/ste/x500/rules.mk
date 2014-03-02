LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH := arm
ARM_CPU := cortex-a8
CPU := generic

# provides a few devices
DEFINES += \
	WITH_DEV_UART=1

MODULES += \
	dev/usb \
	dev/prcmu \
	dev/mmc \
	dev/keys \
	dev/uart \
	dev/i2c \
	dev/display \

INCLUDES += \
	-I$(LOCAL_DIR)/include \
	-Iinclude/platform

OBJS += \
	$(LOCAL_DIR)/abx500.o \
	$(LOCAL_DIR)/clock.o \
	$(LOCAL_DIR)/cpu_early_init.Ao \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/timer.o \
	$(LOCAL_DIR)/usb_abx500.o \
	$(LOCAL_DIR)/abx500_vibrator.o \
	$(LOCAL_DIR)/cmd_ab.o

DEFINES += MEMBASE=$(MEMBASE) \
	WITH_CPU_EARLY_INIT=1

LINKER_SCRIPT += \
	$(BUILDDIR)/system-onesegment.ld

include platform/ste/shared/rules.mk
