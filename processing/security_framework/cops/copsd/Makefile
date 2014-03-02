#############################################
#                                           #
#          Generic COPS daemon              #
#                                           #
# copsd can be set in three different modes #
# 1. COPS_DAEMON (typically on APE CPU)     #
# 2. COPS_MODEM (typically on Access CPU)   #
# 3. COPS_RELAY (typically on Data CPU)     #
#                                           #
#############################################

COPS_SET_DAEMON ?= daemon

$(info COPS_SET_DAEMON = $(COPS_SET_DAEMON))

ifeq ($(COPS_SET_DAEMON), daemon)
 COPS_SET_SECURE_WORLD = emul
 COPS_ENABLE_SILENT_REBOOT_SUPPORT ?= false
 COPS_SET_STORAGE = fs
else ifeq ($(COPS_SET_DAEMON), modem)
 COPS_SET_SECURE_WORLD = local
 COPS_SET_STORAGE = none
 COPS_ENABLE_SILENT_REBOOT_SUPPORT ?= false
else ifeq ($(COPS_SET_DAEMON), relay)
 COPS_SET_SECURE_WORLD = emul
 COPS_SET_STORAGE = none
 COPS_ENABLE_SILENT_REBOOT_SUPPORT ?= false
else
$(error No COPS Daemon type selected!)
endif

DEBUG = 2
#TARGET=native
WARNS = 3
COPS_ROOT = ..
USE_COPS_SHARED = 1
USE_COPS_API = 1
SRCS += \
	cops_debug.c \
	cops_main.c \
	ipc/cops_ipc_common.c \
	tapp/cops_tapp_common.c \
	cops_router.c \
	cops_wakelock.c \
	cops_msg_handler.c \
	srv/cops_srv_common.c \
	storage/cops_bootpartitionhandler.c \
	../shared/cops_sipc.c \
	../shared/cops_log.c

COPS_LOG_FILE ?= ./cops.log
BACKUP_COPS_LOG_FILE ?= ./backup_cops.log
COPS_SOCKET_PATH ?= ./cops.socket

CPPFLAGS += \
	-I. \
	-I../cops-api \
	-I./ipc \
	-I./storage \
	-I./tapp \
	-I./srv \
	-I../shared

CPPFLAGS += -DCOPS_LOG_FILE=$(COPS_LOG_FILE)
CPPFLAGS += -DBACKUP_COPS_LOG_FILE=$(BACKUP_COPS_LOG_FILE)
CPPFLAGS += -DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH)
CPPFLAGS += -DCOPS_STORAGE_DIR=$(CURDIR)/
LDADD += -lssl
CLEANFILES += cops.socket indata.taf_id.* outdata.taf_id.* *.d *.o gstate.bin
PROG = copsd

######################################################
#                                                    #
#       Configuration of TOC access handling         #
#                                                    #
######################################################
SRCS += storage/cops_tocaccess_emul.c

#########################################
#                                       #
#       Configure IPC mechanism         #
#                                       #
# modemsipc - default for U8500         #
# emul      - default for ubuntu        #
#                                       #
#########################################
COPS_SET_IPC ?= emul
$(info COPS_SET_IPC = $(COPS_SET_IPC))
ifeq ($(COPS_SET_IPC), emul)
 SRCS += ipc/cops_ipc_modemsipc.c
 CPPFLAGS += -DCOPS_IPC_EMUL
 CPPFLAGS += -DSIPC_CLIENT=/dev/ttyS3
 CPPFLAGS += -DSIPC_SERVER=/dev/ttyS3
 CPPFLAGS += -DCOPS_8500
else ifeq ($(COPS_SET_IPC), modemsipc)
 SRCS += ipc/cops_ipc_modemsipc.c
 CPPFLAGS += -DSIPC_CLIENT=/dev/sec
else ifeq ($(COPS_SET_IPC), caif)
 SRCS += ipc/cops_ipc_caif.c
 CPPFLAGS += -DSIPC_CLIENT=cops_modem
else
$(error No IPC mechanism has been set!)
endif

######################################
#                                    #
#      Enable logging to file        #
#                                    #
######################################
ifdef COPS_ENABLE_LOG_TO_FILE
$(info COPS_ENABLE_LOG_TO_FILE is defined)
COPS_LOG_FILE ?= /cops.log
$(info COPS_LOG_FILE = $(COPS_LOG_FILE))
BACKUP_COPS_LOG_FILE ?= /backup_cops.log
$(info BACKUP_COPS_LOG_FILE = $(BACKUP_COPS_LOG_FILE))
CPPFLAGS += -DCOPS_ENABLE_LOG_TO_FILE \
	-DCOPS_LOG_FILE=$(COPS_LOG_FILE) \
	-DBACKUP_COPS_LOG_FILE=$(BACKUP_COPS_LOG_FILE)
endif

#############################################
#
# Configuration of silent reboot support
#
#############################################
COPS_ENABLE_SILENT_REBOOT_SUPPORT ?= false
$(info COPS_ENABLE_SILENT_REBOOT_SUPPORT = $(COPS_ENABLE_SILENT_REBOOT_SUPPORT))
ifeq ($(COPS_ENABLE_SILENT_REBOOT_SUPPORT), true)
SHMNETLNK_PATH = $(COPS_ROOT)/../../../access_services/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
CPPFLAGS += -DCOPS_SILENT_REBOOT_SUPPORT
CPPFLAGS += -I$(SHMNETLNK_PATH)
LDADD += -llibshmnetlnk -L$(SHMNETLNK_PATH)
endif

######################################################
#                                                    #
#       Configuration of storage solution            #
#                                                    #
# cspsa - store the cops data in the CSPSA/Trim Area #
# fs    - store the cops data in the file system     #
#                                                    #
######################################################
COPS_SET_STORAGE ?= fs
$(info COPS_SET_STORAGE = $(COPS_SET_STORAGE))
ifeq ($(COPS_SET_STORAGE), cspsa)
CSPSA_PATH = $(COPS_ROOT)/../../../storage/parameter_storage/cspsa
SRCS += storage/cops_storage_cspsa.c
CPPFLAGS += -I$(CSPSA_PATH)
LDADD+= -lcspsa -L$(CSPSA_PATH)
else ifeq ($(COPS_SET_STORAGE), fs)
SRCS += storage/cops_storage_fs.c
else ifeq ($(COPS_SET_STORAGE), none)
 #COPS_Modem and COPS_RELAY don't use flash
else
$(error No valid storage solution has been set!)
endif

######################################################
#                                                    #
#       Configuration of bootpartition handling      #
#                                                    #
# Only supports using emulation in this build        #
#                                                    #
######################################################
CPPFLAGS += -DCOPS_BOOTPART_SUPPORT
CPPFLAGS += -DBOOTPARTITION_DEV=bootpartition.bin
CPPFLAGS += -DBOOTPARTITION_RO_SYSFSNODE=bootpartition_sysfs_rwflag.bin
CPPFLAGS += -DBOOTPARTITION_ROLOCK_SYSFSNODE=bootpartition_sysfs_rolock.bin
###################################################
#                                                 #
#   Configuration of secure world solution        #
#                                                 #
# emul    - run as emulated in user space         #
#           Only supported on ubuntu (not target) #
# secure  - run in secure world                   #
# stubbed - stubb all replies from secure world   #
#                                                 #
###################################################
COPS_SET_SECURE_WORLD ?= emul
$(info COPS_SET_SECURE_WORLD = $(COPS_SET_SECURE_WORLD))
ifeq ($(COPS_SET_SECURE_WORLD), emul)
ifdef COPS_RUNNING_ON_TARGET
$(error Emulated secure world not supported!)
endif
COPS_TAPP_TEST_FILE ?= tapp_test
CPPFLAGS += -DCOPS_TAPP_EMUL
CPPFLAGS += -DCOPS_TAPP_TEST_PATH=\"$(CURDIR)/../\"
SRCS += tapp/cops_tapp_emul.c
SRCS +=	cops_mac.c
else ifeq ($(COPS_SET_SECURE_WORLD), secure)
LDADD += -lteec -L$(COPS_ROOT)/../bass_app/teec/
SRCS += tapp/cops_tapp_secwrld.c
SRCS +=	cops_mac.c
else ifeq ($(COPS_SET_SECURE_WORLD), stubbed)
SRCS += tapp/cops_tapp_stubbed.c
CPPFLAGS += -DCOPS_TAPP_EMUL
CPPFLAGS += -DCOPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
SRCS +=	cops_mac.c
else ifeq ($(COPS_SET_SECURE_WORLD), local)
SRCS += tapp/cops_tapp_local.c
SRCS +=	cops_mac_local.c
SRCS += ../shared/ta/cops_gstate.c
CPPFLAGS += -DCOPS_TAPP_EMUL
CPPFLAGS += -DCOPS_TAPP_LOCAL
else
$(error No valid secure world solution has been set!)
endif

IGNORE_DEFAULT_INSTALL=1

include $(COPS_ROOT)/common.mk

# Overriding the common install directive.
.PHONY: install
install: $(PROG)
	$(INSTALL) -d $(DESTDIR)$(sbindir)
	$(INSTALL) -d $(DESTDIR)$(sysconfdir)/rc.d/init.d
ifeq ($(COPS_SET_SECURE_WORLD), emul)
ifndef COPS_RUNNING_ON_TARGET
	#$(INSTALL) $(COPS_ROOT)/$(COPS_TAPP_TEST_FILE) $(DESTDIR)$(sbindir)/tapp_test
else
	$(error Emulated secure world not supported!)
endif
else ifeq ($(COPS_SET_SECURE_WORLD), secure)
	$(INSTALL) -d $(DESTDIR)$(libdir)/tee
	$(INSTALL) $(COPS_ROOT)/ta/cops_ta.ssw $(DESTDIR)$(libdir)/tee/cops_ta.ssw
endif
	# copsd
	$(INSTALL) $(PROG) $(DESTDIR)$(sbindir)/$(PROG)
	# For now this file is copied every time even though it's not used
	# when building COPS with CSPSA support
	$(INSTALL) COPS_DATA_0.csd $(DESTDIR)$(sysconfdir)/COPS_DATA_0.csd

# creates the files needed when running copsd with bootpartition support
all:
	@echo Creating bootpartition support emulation files
	@dd bs=256 count=1 if=/dev/zero of=$(COPS_ROOT)/copsd/bootpartition.bin 2>/dev/null
	@echo -n rw >$(COPS_ROOT)/copsd/bootpartition_sysfs_rolock.bin
	@echo -n 1 >$(COPS_ROOT)/copsd/bootpartition_sysfs_rwflag.bin
