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

ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

LD              := $(CROSS_COMPILE)ld
LDFLAGS         :=

RM              := rm -rf

OUT_DIR		?= $(FORESTROOT_DIR)/out
ifdef STE_PLATFORM
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/cops/cops-api
else
OUT_DIR		:= $(OUT_DIR)/cops/cops-api
endif

.PHONY: all build-cops_api clean

.SUFFIXES:
.SUFFIXES: .c .o

all: build-cops_api
################################################################################
# cops_api configuration
################################################################################
PACKAGE_NAME		:= libcops.so.1.0.0

# The vpath must declare all the folders where we are supposed to find the
# source files stated in COPSD_SRCS.
vpath %.c ../shared
vpath %.c ../shared/ta
vpath %.c ../copsd

# Path to the location of the socket and the log file.
COPS_SOCKET_PATH	?= /etc/cops/cops.socket
COPS_LOG_FILE		?= /etc/cops/cops.log
COPS_STORAGE_DIR	?= /etc/cops

# cops-api root folder.
COPS_API_SRCS		:= cops_api_client.c \
			   cops_api_connection.c \
			   cops_api_server.c
# ../shared folder
COPS_API_SRCS		+= cops_socket.c \
			   cops_fd.c \
			   cops_error.c \
			   cops_shared_util.c \
			   cops_sipc.c \
			   cops_log.c

# ../shared/ta folder
COPS_API_SRCS		+= cops_sipc_message.c \
			   cops_tapp_sipc.c

COPS_API_OBJ_DIR	:= $(OUT_DIR)
COPS_API_OBJS		:= $(addprefix $(COPS_API_OBJ_DIR)/, \
			     $(patsubst %.c, %.o, $(COPS_API_SRCS)))
COPS_API_INCLUDES 	:= $(CURDIR) $(CURDIR)/include $(CURDIR)/../shared \
			   $(CURDIR)/../shared/ta $(CURDIR)/../copsd
COPS_API_CFLAGS		:= $(addprefix -I, $(COPS_API_INCLUDES)) $(CFLAGS) \
			   -DCOPS_LOG_FILE=$(COPS_LOG_FILE) \
			   -DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH) \
			   -DCOPS_STORAGE_DIR=$(COPS_STORAGE_DIR) \
			   -DCOPS_TAPP_EMUL
COPS_API_LIBRARY	:= $(OUT_DIR)/$(PACKAGE_NAME)

build-cops_api: $(COPS_API_OBJ_DIR) $(COPS_API_LIBRARY)
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/lib 755 0 0
	@$(PACKAGE_FILE) /usr/lib/$(PACKAGE_NAME) $(COPS_API_LIBRARY) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libcops.so.1 $(PACKAGE_NAME) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libcops.so $(PACKAGE_NAME) 755 0 0
	@$(PACKAGE_DIR) /lib 755 0 0
endif

ifeq ($(findstring libcops.so.1,$(wildcard *.so.1)), )
	@echo "creating symlink to $(COPS_API_LIBRARY)!"
	@ln -sf $(COPS_API_LIBRARY) $(OUT_DIR)/libcops.so.1
endif

ifeq ($(findstring libcops.so,$(wildcard *.so)), )
	@echo "creating symlink to $(COPS_API_LIBRARY)!"
	@ln -sf $(COPS_API_LIBRARY) $(OUT_DIR)/libcops.so
endif

$(COPS_API_LIBRARY): $(COPS_API_OBJS)
	@echo "  LD      $@"
	@$(CC) -shared -Wl,-soname,$(PACKAGE_NAME) -o $@ $+
	@echo ""

$(COPS_API_OBJ_DIR)/%.o: %.c
	@echo "  CC      $<"
	@$(CC) $(COPS_API_CFLAGS) -c $< -o $@

# Target just to create a temporary obj-folder
$(COPS_API_OBJ_DIR):
	@[ -d $@ ] || mkdir -p $@

################################################################################
# Cleaning up configuration
################################################################################
clean:
	$(RM) $(OUT_DIR)
