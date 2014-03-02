FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../../..)
TOOLCHAINPATH	?= $(FORESTROOT_DIR)/toolchain/arm-2009q3
TOOLCHAIN       := $(TOOLCHAINPATH)/bin

CROSS_PREFIX    ?= arm-none-linux-gnueabi
CROSS_COMPILE   ?= $(CROSS_PREFIX)-

PATH            := $(TOOLCHAIN):$(PATH)
export PATH

ARCH		?= arm

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

# Don't use thumb mode for x86
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
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/cops/copsd
else
OUT_DIR		:= $(OUT_DIR)/cops/copsd
endif

.PHONY: all build-copsd clean check-libs

.SUFFIXES:
.SUFFIXES: .c .o

all: build-copsd copy-ta
################################################################################
# copsd configuration
################################################################################
# PACKAGE_NAME variable is needed by LBP scripts that generate the final
# filesystem.
PACKAGE_NAME	:= copsdaemon

# The vpath must declare all the folders where we are supposed to find the
# source files stated in COPSD_SRCS.
vpath %.c ipc srv storage tapp ../shared ../shared/ta

# Path to the location of the socket and the log file.
COPS_SOCKET_PATH	?= /etc/cops/cops.socket
COPS_LOG_FILE		?= /etc/cops/cops.log
BACKUP_COPS_LOG_FILE    ?= /etc/cops/backup_cops.log
COPS_STORAGE_DIR	?= /etc/cops/

# Path to CSPSA
CSPSA_PATH=$(FORESTROOT_DIR)/vendor/st-ericsson/storage/parameter_storage/cspsa
ifeq ($(CSPSALIBDIR),)
	override CSPSALIBDIR = $(CSPSA_PATH)
endif

# Path to TOC
TOC_PATH=$(FORESTROOT_DIR)/vendor/st-ericsson/storage/boot_area/toc

COPS_SET_STORAGE	?= cspsa

# Explanation to some of flags etc starting with COPSD_ use below that might not
# be obvious.
#
# For COPSD_INCLUDES, there are hardcoded paths to source code outside this git.
# There are hardcoded paths to bass_app (this is for libtee.so), the kernel
# (this is to find caif_socket.h and arch/xxx/asm/bitsperlong.h) and to cspsa.
#
# For COPSD_CFLAGS, we add COPS_DAEMON to build the daemon and we add __LBP__ to
# solve problems releated to include file mismatch between compilers (socket.h).
#
# For COPSD_LFLAGS, we have hardcoded path to library outside this git.
# Currently we use libtee.so which are located in bass_app.
# Root folder for copsd
COPSD_SRCS	:= cops_bootpartitionhandler.c \
		   cops_debug.c \
		   cops_mac.c \
		   cops_main.c \
		   cops_router.c \
		   cops_msg_handler.c \
		   cops_tocaccess.c \
		   cops_wakelock.c
# ipc folder
COPSD_SRCS	+= cops_ipc_caif.c \
		   cops_ipc_common.c
# srv folder
COPSD_SRCS	+= cops_srv_common.c
# storage folder
ifeq ($(COPS_SET_STORAGE), cspsa)
COPSD_SRCS	+= cops_storage_cspsa.c
else
COPSD_SRCS	+= cops_storage_fs.c
endif
# tapp folder
COPSD_SRCS	+= cops_tapp_common.c \
		   cops_tapp_secwrld.c
# ../shared folder
COPSD_SRCS	+= cops_error.c \
		   cops_fd.c \
		   cops_shared_util.c \
		   cops_socket.c \
		   cops_log.c
# ../shared/ta folder
COPSD_SRCS	+= cops_data_imei.c \
		   cops_data_parameter.c \
		   cops_gstate.c \
		   cops_tapp_sipc.c \
		   cops_data_lock.c \
		   cops_data_sipc.c \
		   cops_sipc_message.c
COPSD_OBJ_DIR	:= $(OUT_DIR)
COPSD_OBJS	:= $(addprefix $(COPSD_OBJ_DIR)/, \
		     $(patsubst %.c, %.o, $(COPSD_SRCS)))
COPSD_INCLUDES 	:= $(CURDIR) \
		   $(CURDIR)/ipc \
		   $(CURDIR)/storage \
		   $(CURDIR)/srv \
		   $(CURDIR)/tapp \
		   $(CURDIR)/../cops-api \
		   $(CURDIR)/../cops-api/include \
		   $(CURDIR)/../shared \
		   $(CURDIR)/../shared/ta \
		   $(CURDIR)/../../bass_app/teec/include \
		   $(FORESTROOT_DIR)/kernel/include \
		   $(FORESTROOT_DIR)/kernel/arch/$(ARCH)/include \
		   $(FORESTROOT_DIR)/out/kernel/arch/arm/include/generated
ifeq ($(COPS_SET_STORAGE), cspsa)
COPSD_INCLUDES	+= $(CSPSA_PATH)
endif
ifeq ($(STE_PLATFORM), u8500)
COPSD_INCLUDES += $(TOC_PATH)
endif

COPSD_CFLAGS	:= $(addprefix -I, $(COPSD_INCLUDES)) $(CFLAGS) \
		   -D__LBP__ -DCOPS_LOG_FILE=$(COPS_LOG_FILE) \
		   -D__LBP__ -DCOPS_BACKUP_COPS_LOG_FILE=$(BACKUP_COPS_LOG_FILE) \
		   -DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH) \
		   -DCOPS_STORAGE_DIR=$(COPS_STORAGE_DIR) \
		   -DCOPS_ENABLE_LOG_TO_FILE -DCOPS_WAKELOCK
ifeq ($(STE_PLATFORM), u8500)
COPSD_CFLAGS	+= -DCOPS_BOOTPART_SUPPORT
COPSD_CFLAGS	+= -DCOPS_TOCACCESS_SUPPORT
COPS_USE_GP_TEEV0_17 ?= true
endif

ifeq ($(COPS_USE_GP_TEEV0_17), true)
COPSD_CFLAGS	+= -DCOPS_USE_GP_TEEV0_17
endif

COPSD_LLIBS     :=
COPSD_LFLAGS	:= -L$(OUT_DIR)/../cops-api/ -lcops \
		   -L$(OUT_DIR)/../../bass_app/teec -ltee
ifeq ($(COPS_SET_STORAGE), cspsa)
COPSD_LFLAGS	+= -L$(CSPSALIBDIR) -lcspsa
endif
ifeq ($(STE_PLATFORM), u8500)
COPSD_LFLAGS    += -L$(TOC_PATH)
COPSD_LLIBS     += -ltocparser
endif
COPSD_BINARY	:= $(OUT_DIR)/$(PACKAGE_NAME)

build-copsd: $(COPSD_OBJ_DIR) check-libs $(COPSD_BINARY)
# SECURITY_PATH is set in product/common/ux500.mak, if that is set, we know that
# this Makefile was called from ux500.mak initially. For normal X86 for example,
# we don't run this code.
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/$(PACKAGE_NAME) $(COPSD_BINARY) 755 0 0
	@$(PACKAGE_DIR) /etc 755 0 0
	@$(PACKAGE_DIR) /etc/cops 755 0 0
ifeq ($(COPS_SET_STORAGE), fs)
	@$(PACKAGE_FILE) /etc/cops/COPS_DATA_0.csd $(CURDIR)/COPS_DATA_0.csd 755 0 0
endif
endif

$(COPSD_BINARY): $(COPSD_OBJS)
	@echo "  LD      $@"
	@$(CC) -o $@ $(COPSD_LFLAGS) $^ $(COPSD_LLIBS)
	@echo ""

$(COPSD_OBJ_DIR)/%.o: %.c
	@echo "  CC      $<"
	@$(CC) $(COPSD_CFLAGS) -c $< -o $@

check-libs: $(COPSD_OBJ_DIR)
	$(if $(wildcard $(OUT_DIR)/../../bass_app/teec/libtee.so),,\
	  $(error libtee.so is missing, build bass_app first.))
	$(if $(wildcard $(OUT_DIR)/../cops-api/libcops.so),,\
	  $(error libcops.so missing, build cops-api first.))
	$(if $(wildcard $(CSPSALIBDIR)/libcspsa.so),,\
	  $(error libcspsa.so missing, build cspsa first.))

# Target just to create a temporary obj-folder
$(COPSD_OBJ_DIR):
	@[ -d $@ ] || mkdir -p $@

################################################################################
# Place relevant ta files in target filesystem
################################################################################
copy-ta:
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /lib/tee 755 0 0
	$(foreach f, \
		$(notdir $(wildcard $(CURDIR)/../ta/*cops_ta*$(subst u,,$(STE_PLATFORM))*.ssw)), \
		$(call copy-one-ssw-file,$f))
endif

define copy-one-ssw-file
	$(PACKAGE_FILE) /lib/tee/$(1) $(CURDIR)/../ta/$(1) 755 0 0;
endef

################################################################################
# Cleaning up configuration
################################################################################
clean:
	$(RM) $(OUT_DIR)
