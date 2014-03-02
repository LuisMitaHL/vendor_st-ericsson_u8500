################################################################################
# Global definitions and checks for correct setup to use bass_app              #
################################################################################
LOCAL_PATH := $(call my-dir)
BASS_APP_PATH := $(LOCAL_PATH)
CSPSA_PATH := $(STORAGE_PATH)/parameter_storage/cspsa

BASS_APP_DRM_KEY_SET_STORAGE ?= cspsa

# There are 5 supported levels, see include/debug.h. We want to have 2 as
# default.
BASS_APP_DEBUG_PRINT ?= 2

# The location of the log file when logging to file is enabled.
BASS_APP_LOG_FILE ?= \"/data/bass_app.log\"

SSW_OUT_PATH = /system/lib/tee

# Since there isn't any global flag that works on both Android and LBP build we
# construct one here and we name it to the same name as they do in LBP.
ifeq ($(findstring u8500, ${TARGET_PRODUCT}), u8500)
STE_PLATFORM := u8500
else ifeq ($(findstring u8400, ${TARGET_PRODUCT}), u8400)
STE_PLATFORM := u8400
else ifeq ($(findstring u8520, ${TARGET_PRODUCT}), u8520)
STE_PLATFORM := u8520
else ifeq ($(findstring u8420, ${TARGET_PRODUCT}), u8420)
STE_PLATFORM := u8420
else
STE_PLATFORM := unknown
endif

# The sign package base must be set in the products BoardConfig file. By the
# base we are meaning for example 8500 and NOT the full U8500_STE_R1E that are
# mentioned in the config.list files for example. This flag is used to select
# the correct ssw-files for the particular platform in use.
ifeq ($(strip $(BASS_APP_SET_SIGNING)),)
$(error BASS_APP_SET_SIGNING must be set to the sign package used in $(TARGET_PRODUCT) BoardConfig.mk)
endif

################################################################################
# Build libtee.so - TEE (Trusted Execution Environment) shared library         #
################################################################################
include $(CLEAR_VARS)
ifeq ($(BASS_APP_BUILD_DEBUG), true)
LOCAL_CFLAGS += -DDEBUG -O0 -g
endif

ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

LOCAL_CFLAGS += -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		-DBINARY_PREFIX=\"TEEC\" \
		-DTEEC_LOAD_PATH=\"$(SSW_OUT_PATH)\"

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES := teec/tee_client_api.c
LOCAL_SRC_FILES += $(call all-c-files-under, util)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/teec/include \
		    $(LOCAL_PATH)/include
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libtee
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

################################################################################
# Build libtee_static.a - TEE (Trusted Execution Environment) static library   #
################################################################################
include $(CLEAR_VARS)
ifeq ($(BASS_APP_BUILD_DEBUG), true)
LOCAL_CFLAGS += -DDEBUG -O0 -g
endif

ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

LOCAL_CFLAGS += -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		-DTEEC_LOAD_PATH=\"$(SSW_OUT_PATH)\"

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES := teec/tee_client_api.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/teec/include \
		    $(LOCAL_PATH)/include
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libtee_static
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

################################################################################
# Build libbassapp.so - Bass app shared library                                #
################################################################################
include $(CLEAR_VARS)
ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

LOCAL_CFLAGS += -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT)

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES += $(call all-c-files-under, libbassapp)
LOCAL_SRC_FILES += $(call all-c-files-under, util)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
		    $(LOCAL_PATH)/teec/include \
		    $(LOCAL_PATH)/libbassapp/include \
		    $(LOCAL_PATH)/test/include
ifeq ($(BASS_APP_DRM_KEY_SET_STORAGE), cspsa)
LOCAL_C_INCLUDES += $(CSPSA_PATH)
endif

LOCAL_SHARED_LIBRARIES := libtee
ifeq ($(BASS_APP_DRM_KEY_SET_STORAGE), cspsa)
LOCAL_SHARED_LIBRARIES += libcspsa
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libbassapp
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

################################################################################
# Production ssw-files                                                         #
################################################################################
# This section needs some explanation. Androids makefile system only support
# copy of predefined files/locations for example executables, shared libraries
# etc. The build system moves those kind of files into the correct location.
# However as we are using ssw-files and want those to be placed in
# /system/lib/tee we need to do a trick to make this happen. We setup a dummy
# target, here called ssw_dummy. When this is done and a build is in place,
# this dummy target is built, which in turn triggers the "dynamical" targets
# below. The day when Android supports a simple cp *.ssw to /system/lib/tee, we
# should rewrite this section completely.
include $(CLEAR_VARS)
LOCAL_PATH := $(BASS_APP_PATH)/libbassapp/ta
# Grab all ssw-files matching the sign package in use for the current platform
# with the full path.
SSW_FILES_WITH_PATH := $(wildcard $(LOCAL_PATH)/*$(subst u,,$(BASS_APP_SET_SIGNING))*.ssw)

# Remove the path, so only the filename.ssw exist.
SSW_FILES := $(subst $(LOCAL_PATH)/,,$(SSW_FILES_WITH_PATH))

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := ssw_dummy
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(SSW_OUT_PATH)

# Create the intermediate folder and create the ssw_dummy file in that folder.
THIS_INTERMEDIATES := $(call local-intermediates-dir)
$(shell mkdir -p $(THIS_INTERMEDIATES))
$(shell touch $(THIS_INTERMEDIATES)/$(LOCAL_MODULE))
include $(BUILD_PREBUILT)

# These are dynamic targets which well be executed when the dummy target above
# is being built.
$(LOCAL_BUILT_MODULE): $(addprefix $(LOCAL_MODULE_PATH)/, $(SSW_FILES))
$(LOCAL_MODULE_PATH)/%: $(LOCAL_PATH)/% | $(ACP)
	$(transform-prebuilt-to-target)

################################################################################
# Test ssw-files                                                               #
################################################################################
ifeq ($(BASS_APP_BUILD_TESTSUITE), true)
# The same kind of trick is used to copy the test ssw-files to /system/lib/tee,
# with the exception that we copy all available ssw-files.
include $(CLEAR_VARS)
LOCAL_PATH := $(BASS_APP_PATH)/test/ta
# Grab all ssw-files with the full path in the test/ta folder.
SSW_FILES_WITH_PATH := $(wildcard $(LOCAL_PATH)/*.ssw)

# Remove the path, so only the filename.ssw exist.
SSW_FILES := $(subst $(LOCAL_PATH)/,,$(SSW_FILES_WITH_PATH))

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := ssw_test_dummy
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(SSW_OUT_PATH)

# Create the intermediate folder and create the ssw_test_dummy file in that
# folder.
THIS_INTERMEDIATES := $(call local-intermediates-dir)
$(shell mkdir -p $(THIS_INTERMEDIATES))
$(shell touch $(THIS_INTERMEDIATES)/$(LOCAL_MODULE))
include $(BUILD_PREBUILT)

# These are dynamic targets which well be executed when the dummy target above
# is being built.
$(LOCAL_BUILT_MODULE): $(addprefix $(LOCAL_MODULE_PATH)/, $(SSW_FILES))
$(LOCAL_MODULE_PATH)/%: $(LOCAL_PATH)/% | $(ACP)
	$(transform-prebuilt-to-target)
endif

################################################################################
# Build bass_app_testsuite - Bass app's main tests                            #
################################################################################
ifeq ($(BASS_APP_BUILD_TESTSUITE), true)
include $(CLEAR_VARS)
ifeq ($(BASS_APP_BUILD_DEBUG), true)
LOCAL_CFLAGS += -DSTRESS_TEST_DEBUG -O0 -g
endif

ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

# Only valid to enable prior to post-boot, e.g. u-boot
ifeq ($(DRM_KEY_PRE_CONDITION_DRM_STATE_ENABLED), true)
LOCAL_CFLAGS += -DDRM_KEY_PRE_CONDITION_DRM_STATE_ENABLED
endif

# Create and make an upper case flag STE_PLATFORM_U8500 for example.
LOCAL_CFLAGS += -DSTE_PLATFORM_$(shell echo $(STE_PLATFORM) | tr a-z A-Z) \
		-DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		-DBINARY_PREFIX=\"TEST\"

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES := test/arb_table_info_test.c \
		   test/bass_app_test.c \
		   test/calc_digest_test.c \
		   test/check_payload_hash_test.c \
		   test/product_id_test.c \
		   test/product_config_test.c \
		   test/stress_test.c \
		   test/verify_signedheader_test.c \
		   test/test_tee_static_uuid.c \
		   test/nw_to_tee_test_data.c \
		   test/nw_to_tee_test.c
ifeq ($(BASS_APP_DRM_KEY_SET_STORAGE), cspsa)
LOCAL_SRC_FILES += test/drm_key_test.c
endif
LOCAL_SRC_FILES += $(call all-c-files-under, util)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
		    $(LOCAL_PATH)/libbassapp/include \
		    $(LOCAL_PATH)/teec/include \
		    $(LOCAL_PATH)/test/include \
		    $(LOCAL_PATH)/test/tee/include

LOCAL_SHARED_LIBRARIES := libbassapp libtee
LOCAL_MODULE := bass_app_testsuite
LOCAL_MODULE_TAGS := tests
include $(BUILD_EXECUTABLE)
endif
################################################################################
# Test TA2TA                                                                   #
################################################################################
ifeq ($(BASS_APP_BUILD_TESTSUITE), true)
include $(CLEAR_VARS)
ifeq ($(BASS_APP_BUILD_DEBUG), true)
LOCAL_CFLAGS += -DDEBUG -O0 -g
endif

ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

LOCAL_CFLAGS += -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		-DBINARY_PREFIX=\"TEST\"

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES := test/tee/ta2ta_test.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/teec/include \
		    $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libtee
LOCAL_MODULE := tee_ta2ta_test
LOCAL_MODULE_TAGS := tests
include $(BUILD_EXECUTABLE)
endif

################################################################################
# Build smcl_cmd - A tool to trigger SMCL functions                            #
################################################################################
ifeq ($(BASS_APP_BUILD_TESTSUITE), true)
include $(CLEAR_VARS)
ifeq ($(BASS_APP_BUILD_DEBUG), true)
LOCAL_CFLAGS += -DDEBUG -O0 -g
endif

ifeq ($(BASS_APP_LOG_TO_FILE), true)
LOCAL_CFLAGS += -DBASS_APP_LOG_FILE=$(BASS_APP_LOG_FILE)
endif

LOCAL_CFLAGS += -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		-DBINARY_PREFIX=\"SCMD\"

LOCAL_PATH:= $(BASS_APP_PATH)
LOCAL_SRC_FILES := test/smcl_cmd.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/test/include \
                    $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libbassapp
LOCAL_MODULE := smcl_cmd
LOCAL_MODULE_TAGS := tests
include $(BUILD_EXECUTABLE)
endif
