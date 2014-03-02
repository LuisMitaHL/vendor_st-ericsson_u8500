FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../../..)
TOOLCHAINPATH	?= $(FORESTROOT_DIR)/toolchain/arm-2009q3
TOOLCHAIN       := $(TOOLCHAINPATH)/bin

CROSS_PREFIX    ?= arm-none-linux-gnueabi
CROSS_COMPILE   ?= $(CROSS_PREFIX)-

PATH            := $(TOOLCHAIN):$(PATH)
export PATH

CC              := $(CROSS_COMPILE)gcc

CFLAGS          := -Wall -Wbad-function-cast -Wcast-align \
		   -Werror-implicit-function-declaration -Wextra \
		   -Wfloat-equal -Wformat-nonliteral -Wformat-security \
		   -Wformat=2 -Winit-self -Wmissing-declarations \
		   -Wmissing-format-attribute -Wmissing-include-dirs \
		   -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs \
		   -Wpointer-arith -Wshadow -Wstrict-prototypes \
		   -Wswitch-default -Wunsafe-loop-optimizations \
		   -Wwrite-strings
CFLAGS          += -c -fPIC

ifneq ($(CC), gcc)
CFLAGS          += -mthumb -mthumb-interwork
endif

DEBUG		?= 0
ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

LD              := $(CROSS_COMPILE)ld
LDFLAGS         :=

RM              := rm -rf

OUT_DIR		?= $(FORESTROOT_DIR)/out
ifdef STE_PLATFORM
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/bass_app/teec
else
OUT_DIR		:= $(OUT_DIR)/bass_app/teec
endif

BASS_APP_DEBUG_PRINT ?= 2

.PHONY: all teec clean

all: teec
################################################################################
# Teec configuration
################################################################################
vpath %.c ../util
PACKAGE_NAME	:= libtee.so.1.0.0

TEEC_SRCS	:= tee_client_api.c \
		   debug.c
TEEC_SRC_DIR	:= .
TEEC_OBJ_DIR	:= $(OUT_DIR)
TEEC_OBJS 	:= $(patsubst %.c,$(TEEC_OBJ_DIR)/%.o,$(TEEC_SRCS))
TEEC_INCLUDES 	:= $(CURDIR)/../include $(CURDIR)/include
TEEC_CFLAGS	:= $(addprefix -I, $(TEEC_INCLUDES)) $(CFLAGS) -D_GNU_SOURCE \
		   -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		   -DBINARY_PREFIX=\"TEEC\"
TEEC_LFLAGS	:= -lpthread
TEEC_LIBRARY	:= $(OUT_DIR)/$(PACKAGE_NAME)

teec: $(TEEC_LIBRARY)
ifeq ($(DEBUG), 1)
	@echo "PACKAGE_DIR = $(PACKAGE_DIR)"
	@echo "PACKAGE_FILE = $(PACKAGE_FILE)"
	@echo "PACKAGE_LINK = $(PACKAGE_LINK)"
endif
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/lib 755 0 0
	@$(PACKAGE_DIR) /usr/lib/tee 755 0 0
	@$(PACKAGE_FILE) /usr/lib/$(PACKAGE_NAME) $(TEEC_LIBRARY) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libtee.so.1 $(PACKAGE_NAME) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libtee.so $(PACKAGE_NAME) 755 0 0
endif

ifeq ($(findstring libtee.so.1,$(wildcard *.so.1)), )
	@echo "creating symlink to $(OUT_DIR)/libtee.so.1"
	@ln -sf $(TEEC_LIBRARY) $(OUT_DIR)/libtee.so.1
endif

ifeq ($(findstring libtee.so,$(wildcard *.so)), )
	@echo "creating symlink to $(OUT_DIR)/libtee.so"
	@ln -sf $(TEEC_LIBRARY) $(OUT_DIR)/libtee.so
endif


$(TEEC_LIBRARY): $(TEEC_OBJS)
	@echo "  LD      $@"
	@$(CC) -shared -Wl,-soname,$(PACKAGE_NAME) $(TEEC_LFLAGS) -o $@ $+
	@echo ""

$(TEEC_OBJ_DIR)/%.o: %.c
	@mkdir -p $(TEEC_OBJ_DIR)
	@echo "  CC      $<"
	@$(CC) $(TEEC_CFLAGS) -c $< -o $@

################################################################################
# Cleaning up configuration
################################################################################
clean:
	$(RM) $(OUT_DIR)
