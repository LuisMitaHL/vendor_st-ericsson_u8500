# top level project rules for the 9540 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += \
	-I./platform/ste/x500 \
	-I./platform/ste/x500/include \
	-I./platform/ste/x500/include/platform \
	-I./target/ste/ccu9540/include/target \
	-I./target/ste/ccu9540 \
	-I./platform/ste/shared/include \
	-I./dev/i2c \
	-I./dev/keys \
	-I./dev/mmc \
	-I./dev/prcmu \
	-I./dev/uart \
	-I./dev/usb \
	-I./include/lib/fs \
	-I./lib/fs/ff9

TARGET := ste/ccu9540

MODULES += \
	app/shell \
	app/steboot \
	lib/text

#	app/env
