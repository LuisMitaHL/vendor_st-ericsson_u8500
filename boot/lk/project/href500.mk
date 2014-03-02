# top level project rules for the mop500 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += \
	-I./platform/ste/x500 \
	-I./platform/ste/x500/include \
	-I./platform/ste/x500/include/platform \
	-I./target/ste/href500/include/target \
	-I./target/ste/href500 \
	-I./platform/ste/shared/include \
	-I./dev/i2c \
	-I./dev/keys \
	-I./dev/mmc \
	-I./dev/prcmu \
	-I./dev/uart \
	-I./dev/usb \
	-I./include/lib/fs \
	-I./lib/fs/ff9

TARGET := ste/href500

MODULES += \
	app/shell \
	app/steboot\
	lib/text
