FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../../../..)
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

ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

LD              := $(CROSS_COMPILE)ld
LDFLAGS         :=

RM              := rm -rf

OUT_DIR		?= $(FORESTROOT_DIR)/out
ifdef STE_PLATFORM
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/cops/tools/cops_cmd
else
OUT_DIR		:= $(OUT_DIR)/cops/tools/cops_cmd
endif

.PHONY: all build-cops_cmd check-libs clean

all: build-cops_cmd
###############################################################################
# cops_cmd configuration
###############################################################################
PACKAGE_NAME		:= cops_cmd

COPS_CMD_SRCS		:= cops_cmd.c
COPS_CMD_OBJ_DIR	:= $(OUT_DIR)
COPS_CMD_OBJS		:= $(addprefix $(COPS_CMD_OBJ_DIR)/, \
			     $(patsubst %.c, %.o, $(COPS_CMD_SRCS)))
COPS_CMD_INCLUDES 	:= $(CURDIR)/../../cops-api/include
COPS_CMD_CFLAGS		:= $(addprefix -I, $(COPS_CMD_INCLUDES)) $(CFLAGS)
COPS_CMD_LDFLAGS	:= -L$(OUT_DIR)/../../cops-api/ -lcops $(LDFLAGS)
COPS_CMD_BINARY		:= $(OUT_DIR)/cops_cmd

build-cops_cmd: $(COPS_CMD_OBJ_DIR) check-libs $(COPS_CMD_BINARY)
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/$(PACKAGE_NAME) $(COPS_CMD_BINARY) 755 0 0
endif

$(COPS_CMD_BINARY): $(COPS_CMD_OBJS)
	@echo "  LD      $@"
	@$(CC) -o $@ $(COPS_CMD_LDFLAGS) $^
	@echo ""

$(COPS_CMD_OBJ_DIR)/%.o: %.c
	@echo "  CC      $<"
	@$(CC) $(COPS_CMD_CFLAGS) -c $< -o $@

check-libs: $(COPS_CMD_OBJ_DIR)
	$(if $(wildcard $(OUT_DIR)/../../cops-api/libcops.so),,\
	  $(error libcops.so missing, build cops-api first.))

# Target just to create a temporary obj-folder
$(COPS_CMD_OBJ_DIR):
	@[ -d $@ ] || mkdir -p $@

###############################################################################
# Cleaning up configuration
###############################################################################
clean:
	$(RM) $(OUT_DIR)
