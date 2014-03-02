LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmlr
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Wall -Werror -Wextra

LOCAL_SRC_FILES := \
	dgramsk.c \
	libmlr.c

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES+= \
	$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/mal_util/include \
	$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalmon/include \
	$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libphonet/include \
	$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include \
	$(ACCESS_SERVICES_PATH)/call_network/call_network/libcn/include \
	$(PROCESSING_PATH)/security_framework/cops/cops-api/include \
	$(PROCESSING_PATH)/security_framework/bass_app/include \
	$(TOP)/modem/u8500/include/rmc_headers \
	$(TOP)/modem/u8500/include/dump_headers

LOCAL_CFLAGS:= -fno-short-enums -Wall -DSPLIT_TRACE_FILES -DFORBID_TOOL_TO_SET_DEST

LOCAL_SRC_FILES:= \
	modemlogrelay.c \
	libmon.c \
	coredump.c \
	utils.c \
	kerneldump.c \
	xfiledecoder.c \
	memfile.c \
	rilhandler.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libphonet \
	libmalmon \
	libcops \
	libcn \
	libshmnetlnk \
	libbassapp

LOCAL_STATIC_LIBRARIES += libmlr

LOCAL_MODULE:= modem_log_relay
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_MODULE := trace_auto.conf
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := DATA
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/misc

# Make a symlink from /etc/trace_auto.conf to /data/misc/mlr/trace_auto.conf
SYMLINKS := $(TARGET_OUT)/etc/trace_auto.conf
$(SYMLINKS): TRACE_AUTO_CONF := /data/misc/$(LOCAL_MODULE)
$(SYMLINKS): $(LOCAL_INSTALLED_MODULE) $(LOCAL_PATH)/Android.mk
	@echo "Symlink: $@ -> $(TRACE_AUTO_CONF)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf $(TRACE_AUTO_CONF) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
       $(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS)

include $(BUILD_PREBUILT)
