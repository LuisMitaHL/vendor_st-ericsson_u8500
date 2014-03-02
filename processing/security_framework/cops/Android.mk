# Copyright 2011 ST-Ericsson

LOCAL_PATH:= $(call my-dir)
COPS_PATH := $(LOCAL_PATH)

COPS_SOCKET_PATH ?= /data/cops.socket
COPS_DEBUG ?= 2
COPS_LOG_FILE ?= /data/cops.log
BACKUP_COPS_LOG_FILE ?= /data/backup_cops.log

ifeq ($(COPS_DUMMY_TUNNEL_IMEI),true)
include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)

LOCAL_SRC_FILES := cops-api/cops_api_client_dummy.c

CN := $(ACCESS_SERVICES_PATH)/call_network/call_network
LOCAL_C_INCLUDES += $(CN)/libcn/include

LOCAL_C_INCLUDES += \
               $(LOCAL_PATH)/cops-api \
               $(LOCAL_PATH)/cops-api/include \
               $(LOCAL_PATH)/shared \
               $(KERNEL_HEADERS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcops
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += libcn
LOCAL_SHARED_LIBRARIES += libcn libcutils libutils
include $(BUILD_SHARED_LIBRARY)
else

##############################################
# Build COPS (COre Platform Security) Daemon #
##############################################

include $(CLEAR_VARS)

LOCAL_PATH := $(COPS_PATH)

$(info COPS_PLATFORM_TYPE = $(COPS_PLATFORM_TYPE))
ifeq ($(COPS_PLATFORM_TYPE), 8500)
SIPC_CLIENT ?= /dev/sec
LOCAL_SRC_FILES += copsd/ipc/cops_ipc_modemsipc.c
LOCAL_CFLAGS += -DCOPS_BOOTPART_SUPPORT \
		-DCOPS_WAKELOCK \
		-DCOPS_8500
COPS_USE_GP_TEEV0_17 ?= true
else
$(info No COPS_PLATFORM_TYPE found, defaulting to CAIF IPC)
LOCAL_SRC_FILES += copsd/ipc/cops_ipc_caif.c
SIPC_CLIENT ?= cops_relay
COPS_SYNC_FILE1 ?= /modemfs/HSI/Phone_FS/ifs/security/cops_relay
COPS_SYNC_FILE2 ?= /modemfs/C2C/Phone_FS/ifs/security/cops_relay
LOCAL_CFLAGS += -DCOPS_SYNC_FILE -DCOPS_SYNC_FILE1=$(COPS_SYNC_FILE1) -DCOPS_SYNC_FILE2=$(COPS_SYNC_FILE2)
COPS_USE_GP_TEEV0_17 ?= false
endif

LOCAL_SRC_FILES += \
	copsd/ipc/cops_ipc_common.c \
	copsd/cops_debug.c \
	copsd/cops_mac.c \
	copsd/cops_main.c \
	copsd/cops_router.c \
	copsd/cops_msg_handler.c \
	copsd/srv/cops_srv_common.c \
	copsd/tapp/cops_tapp_common.c \
	copsd/storage/cops_bootpartitionhandler.c \
	copsd/cops_wakelock.c

LOCAL_SRC_FILES += \
	shared/cops_error.c \
	shared/cops_fd.c \
	shared/cops_shared_util.c \
	shared/cops_socket.c \
	shared/cops_log.c

LOCAL_SRC_FILES += $(call all-c-files-under, shared/ta)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/copsd/ \
	$(LOCAL_PATH)/copsd/ipc/ \
	$(LOCAL_PATH)/copsd/srv/ \
	$(LOCAL_PATH)/copsd/storage/ \
	$(LOCAL_PATH)/copsd/tapp/ \
	$(LOCAL_PATH)/cops-api/ \
	$(LOCAL_PATH)/cops-api/include/ \
	$(LOCAL_PATH)/shared/ \
	$(LOCAL_PATH)/shared/ta/ \
	$(KERNEL_HEADERS)

LOCAL_CFLAGS += -DSIPC_CLIENT=$(SIPC_CLIENT)

COPS_ENABLE_SILENT_REBOOT_SUPPORT ?= true
$(info COPS_ENABLE_SILENT_REBOOT_SUPPORT = $(COPS_ENABLE_SILENT_REBOOT_SUPPORT))
ifeq ($(COPS_ENABLE_SILENT_REBOOT_SUPPORT), true)
LOCAL_CFLAGS += -DCOPS_SILENT_REBOOT_SUPPORT
ifeq ($(COPS_PLATFORM_TYPE), 8500)
LOCAL_C_INCLUDES += $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
LOCAL_SHARED_LIBRARIES += libshmnetlnk
else
LOCAL_C_INCLUDES += $(call include-path-for, dbus)
LOCAL_SHARED_LIBRARIES += libdbus
endif
endif

LOCAL_CFLAGS += -DCOPS_SHARED_MEM_WORKAROUND \
				-DCOPS_IN_ANDROID \
				-DCOPS_DEBUG=$(COPS_DEBUG) \
				-DCOPS_ENABLE_LOG_TO_FILE \
				-DCOPS_LOG_FILE=$(COPS_LOG_FILE) \
				-DBACKUP_COPS_LOG_FILE=$(BACKUP_COPS_LOG_FILE) \
				-DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH)

LOCAL_SHARED_LIBRARIES += libcops
LOCAL_MODULE := copsdaemon
LOCAL_MODULE_TAGS := optional
COPS_SET_STORAGE ?=cspsa
COPS_SET_SECURE_WORLD ?=secure

ifdef EXTENDED_SIPC_MSGS
LOCAL_CFLAGS += -DEXTENDED_SIPC_MSGS
endif


COPS_ENABLE_ARB_UPDATE ?= true

ifeq ($(COPS_ENABLE_ARB_UPDATE), true)
LOCAL_CFLAGS += -DCOPS_ENABLE_ARB_UPDATE
endif

# COPS Daemon storage solution
$(info COPS_SET_STORAGE = $(COPS_SET_STORAGE))
ifeq ($(COPS_SET_STORAGE), cspsa)
LOCAL_SRC_FILES += copsd/storage/cops_storage_cspsa.c
LOCAL_C_INCLUDES += $(STORAGE_PATH)/parameter_storage/cspsa
LOCAL_SHARED_LIBRARIES += libcspsa
else ifeq ($(COPS_SET_STORAGE), fs)
LOCAL_CFLAGS += -DCOPS_STORAGE_DIR=\"/data/misc/\"
LOCAL_SRC_FILES +=copsd/storage/cops_storage_fs.c
else
$(error No storage solution enabled)
endif

# COPS Daemon secure world solution
$(info COPS_SET_SECURE_WORLD = $(COPS_SET_SECURE_WORLD))
ifeq ($(COPS_SET_SECURE_WORLD), stubbed)
LOCAL_CFLAGS += -DCOPS_TAPP_EMUL
LOCAL_CFLAGS += -DCOPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
LOCAL_SRC_FILES += copsd/tapp/cops_tapp_stubbed.c
else ifeq ($(COPS_SET_SECURE_WORLD), secure)
$(info COPS_USE_GP_TEEV0_17 = $(COPS_USE_GP_TEEV0_17))
ifeq ($(COPS_USE_GP_TEEV0_17), true)
LOCAL_CFLAGS += -DCOPS_USE_GP_TEEV0_17
endif
LOCAL_C_INCLUDES += $(PROCESSING_PATH)/security_framework/bass_app/teec/include
LOCAL_SRC_FILES += copsd/tapp/cops_tapp_secwrld.c
LOCAL_SHARED_LIBRARIES += libtee
else
$(error No valid secure world solution has been set!)
endif

#COPS Daemon boothandling settings
#Only supporting using the real driver in Android build
LOCAL_CFLAGS += -DBOOTPARTITION_RO_SYSFSNODE=/sys/block/mmcblk0boot0/force_ro
LOCAL_CFLAGS += -DBOOTPARTITION_DEV=/dev/block/mmcblk0boot0
LOCAL_CFLAGS += -DBOOTPARTITION_ROLOCK_SYSFSNODE=/sys/block/mmcblk0boot0/ro_lock

# COPS Daemon toc access handling
ifeq ($(COPS_PLATFORM_TYPE), 8500)
LOCAL_C_INCLUDES += $(STORAGE_PATH)/boot_area/toc
LOCAL_STATIC_LIBRARIES += libtocparser
LOCAL_CFLAGS += -DCOPS_TOCACCESS_SUPPORT
endif
LOCAL_SRC_FILES += copsd/storage/cops_tocaccess.c

include $(BUILD_EXECUTABLE)

##################################
# copy cops datafile to root     #
# and copy tapp_test to /sys/bin #
##################################

ifeq ($(COPS_SET_STORAGE), fs)
include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)/copsd
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := COPS_DATA_0.csd
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/misc
include $(BUILD_PREBUILT)
endif

ifeq ($(COPS_SET_SECURE_WORLD), secure)
LOCAL_PATH := $(COPS_PATH)/ta
FILES := $(wildcard $(LOCAL_PATH)/*cops_ta*$(COPS_PLATFORM_TYPE)*.ssw)
FILES := $(subst $(LOCAL_PATH)/,,$(FILES))

include $(CLEAR_VARS)

LOCAL_MODULE := cops_ta
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/lib/tee

THIS_INTERMEDIATES := $(call local-intermediates-dir)
$(shell mkdir -p $(THIS_INTERMEDIATES))
$(shell touch $(THIS_INTERMEDIATES)/$(LOCAL_MODULE))

include $(BUILD_PREBUILT)

$(LOCAL_BUILT_MODULE): $(addprefix $(LOCAL_MODULE_PATH)/, $(FILES))
$(LOCAL_MODULE_PATH)/%: $(LOCAL_PATH)/% | $(ACP)
	$(transform-prebuilt-to-target)

endif

#################################
# Build COPS-API shared library #
#################################

include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)

LOCAL_SRC_FILES := \
		cops-api/cops_api_client.c \
		cops-api/cops_api_connection.c \
		cops-api/cops_api_server.c \
		shared/cops_fd.c \
		shared/cops_error.c \
		shared/cops_shared_util.c \
		shared/cops_socket.c \
		shared/cops_sipc.c \
		shared/cops_log.c \
		shared/ta/cops_sipc_message.c \
		shared/ta/cops_tapp_sipc.c

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/cops-api \
		$(LOCAL_PATH)/cops-api/include \
		$(LOCAL_PATH)/shared \
		$(LOCAL_PATH)/shared/ta \
		$(LOCAL_PATH)/copsd \
		$(KERNEL_HEADERS)

LOCAL_CFLAGS += -DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH) \
		-DCOPS_DEBUG=$(COPS_DEBUG) \
		-DCOPS_TAPP_EMUL

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcops
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

######################
# Build COPS command #
######################

include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)

LOCAL_SRC_FILES := tools/cops_cmd/cops_cmd.c

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/cops-api/ \
		$(LOCAL_PATH)/cops-api/include/ \
		$(LOCAL_PATH)/shared/ta \
		$(KERNEL_HEADERS)

LOCAL_CLFAGS += -DCOPS_DEBUG=$(COPS_DEBUG)
LOCAL_SHARED_LIBRARIES := libcops
LOCAL_MODULE := cops_cmd
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

######################
# Build COPS test #
######################
ifeq ($(COPS_TEST_BUILD_TEST), true)

LOCAL_CFLAGS += -DRUNNING_ON_HW

include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)

LOCAL_SRC_FILES := $(call all-c-files-under, test/cops_test)

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/cops-api/include/ \
		$(LOCAL_PATH)/test/cops_test/ \
		$(KERNEL_HEADERS)

LOCAL_CLFAGS += -DCOPS_DEBUG=$(COPS_DEBUG)
LOCAL_SHARED_LIBRARIES := libcops
LOCAL_MODULE := cops_test
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_PATH := $(COPS_PATH)

LOCAL_SRC_FILES := $(call all-c-files-under, tools/cops_data)

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/cops-api/include/ \
		$(LOCAL_PATH)/shared/ta/ \
		$(KERNEL_HEADERS)

LOCAL_SHARED_LIBRARIES := libcops
LOCAL_MODULE := cops_data
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
endif
endif

