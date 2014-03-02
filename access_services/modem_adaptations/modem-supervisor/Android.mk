ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)

#####################################################################
# libmsup: modem-supervisor client library                          #
#####################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := libmsup
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Wall -Werror -Wextra

LOCAL_SRC_FILES := \
	dgram-sock.c \
	msup-client.c

include $(BUILD_STATIC_LIBRARY)

#####################################################################
# modem-supervisor: modem-supervisor executable                     #
#####################################################################

include $(CLEAR_VARS)

# Change this flag to true to enable test
MODEM_LOG_RELAY:=true
 
LOCAL_CFLAGS += -c -Wall -Os -D _GNU_SOURCE
LOCAL_C_INCLUDES += $(STORAGE_PATH)/boot_area/toc
LOCAL_C_INCLUDES += $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
LOCAL_C_INCLUDES += $(PROCESSING_PATH)/security_framework/bass_app/include
LOCAL_C_INCLUDES += $(TOOLS_PATH)/platform/modem_log_relay

LOCAL_SHARED_LIBRARIES += libbassapp libtee libshmnetlnk libutils
LOCAL_STATIC_LIBRARIES += libtocparser

ifeq ($(MODEM_LOG_RELAY), true)
LOCAL_C_INCLUDES += vendor/st-ericsson/tools/platform/modem_log_relay
LOCAL_CFLAGS += -DMODEM_LOG_RELAY
LOCAL_STATIC_LIBRARIES += libmlr
endif

LOCAL_MODULE:= modem-supervisor
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	fwhandler.c \
	modem-sup.c \
	t_stamps.c \
	dgram-sock.c \
	msup-server.c

include $(BUILD_EXECUTABLE)

endif  # TARGET_SIMULATOR != true

