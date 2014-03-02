ifeq ($(BOARD_USES_C_AUDIO_HAL),true)
#####################################################
#
#
#  Board Uses New Audio HAL=true
#  Use New Audio HAL implementated in C
#
####################################################

LOCAL_PATH := $(call my-dir)

## Definitions used by several targets. ##
##										##

###################################
# AHI files
#
###################################

#Location for sourcefiles needed for the ANM, AHI implementation
SRC_FOLDER_AHI := src/ahi
#Location of dbg specific files
SRC_FOLDER_DBG := src

# Source files needed to build Audio Network Manager.
SRC_FILES_ANM := \
	$(SRC_FOLDER_AHI)/ste_anm_ahi.c \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_output.c \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_input.c \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_admbase.c \
	$(SRC_FOLDER_DBG)/ste_anm_dbg.c \
	$(SRC_FOLDER_DBG)/ste_anm_util.c


###################################
# Audio Policy files
#
###################################
#Source folder for Audio Policy Manager
SRC_FOLDER_AP := src/policy

#Source filed for Audio Policy Manager
SRC_FILES_AP := \
	$(SRC_FOLDER_AP)/ste_anm_ap.c \
	$(SRC_FOLDER_AP)/ste_anm_ext_hal.c \
	$(SRC_FOLDER_AP)/ste_hal_a2dp.c \
	$(SRC_FOLDER_AP)/ste_hal_usb.c \
	$(SRC_FOLDER_DBG)/ste_anm_dbg.c \
	$(SRC_FOLDER_DBG)/ste_anm_util.c

###################################
# Flags
#
###################################

# Common flags
CFLAGS_COMMON_ANM := -DLOG_WARNINGS \
	-DLOG_ERRORS \
        -DSTE_VIDEO_CALL \
	-D_POSIX_SOURCE \
	-DUSE_CACHE_MECHANISM \
	-DALLOW_DUPLICATION

ifeq ($(A2DP_USES_STANDARD_ANDROID_PATH),true)
CFLAGS_COMMON_ANM += -DSTD_A2DP_MNGT
endif


# Headers from external packages.
EXTERNAL_INCLUDES_ANM := \
	$(MULTIMEDIA_PATH)/audio/adm/include \
	$(MULTIMEDIA_PATH)/shared/utils/include \
	hardware/libhardware/include \
	$(call include-path-for, audio-effects)

#######################################################################################################
#
# TARGET SHARED LIBRARY #
#
########################################################################################################

ifeq ($(strip $(BOARD_USES_LD_ANM)),true)

# Generate the audio_policy.PLATFORM shared library used by Audio HAL
include $(CLEAR_VARS)
LOCAL_MODULE := audio_policy.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libste_adm \
	libcutils \
	libstelpcutils \
	libutils \
	libmedia \
	libdl \
	libc

LOCAL_SRC_FILES := $(SRC_FILES_AP)

LOCAL_CFLAGS += $(CFLAGS_COMMON_ANM)
ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_CFLAGS += -DWITH_BLUETOOTH -DWITH_A2DP
endif

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(EXTERNAL_INCLUDES_ANM)

LOCAL_STATIC_LIBRARIES := \
	libmedia_helper

include $(BUILD_SHARED_LIBRARY)

#
# Build audio.primary.PLATFORM for Audio Hardware
#

include $(CLEAR_VARS)
LOCAL_MODULE := audio.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

#Android libraries to link against.
LOCAL_SHARED_LIBRARIES := \
	libste_adm \
	libcutils \
	libstelpcutils \
	libutils \
	libmedia \
	libhardware \
	liblog \
	libasound \
	libdl

LOCAL_SRC_FILES += \
	$(SRC_FILES_ANM)

LOCAL_CFLAGS += \
	$(CFLAGS_COMMON_ANM) \

LOCAL_C_INCLUDES += \
	$(EXTERNAL_INCLUDES_ANM) \
	$(LOCAL_PATH)/include

# Static libraries to link against. We also need
# the ADM client library.
LOCAL_STATIC_LIBRARIES += \
	libmedia_helper

#Build shared lib for host using the above variables
include $(BUILD_SHARED_LIBRARY)

endif #if BOARD_USES_LD_ANM = TRUE




else #BOARD_USES_C_AUDIO_HAL
#############################################################################
#
#
# BOARD USES OLD CPP AUDIO HAL
#
#
#############################################################################
LOCAL_PATH := $(call my-dir)


## Definitions used by several targets. ##
##										##

###################################
# AHI files
#
###################################

#Location for sourcefiles needed for the ANM, AHI implementation
SRC_FOLDER_AHI := legacy/src/ahi
#Location of dbg specific files
SRC_FOLDER_DBG := legacy/src

# Source files needed to build Audio Network Manager.
SRC_FILES_ANM := \
	$(SRC_FOLDER_AHI)/ste_anm_ahi.cc \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_output.cc \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_input.cc \
	$(SRC_FOLDER_AHI)/ste_anm_ahi_admbase.cc \
	$(SRC_FOLDER_DBG)/ste_anm_dbg.cc



###################################
# Audio Policy files
#
###################################
#Source folder for Audio Policy Manager
SRC_FOLDER_AP := legacy/src/policy

#Source filed for Audio Policy Manager
SRC_FILES_AP := \
	$(SRC_FOLDER_AP)/ste_anm_ap.cc \
    $(SRC_FOLDER_DBG)/ste_anm_dbg.cc


###################################
# Flags
#
###################################

# Common flags
CFLAGS_COMMON_ANM := -DLOG_WARNINGS \
	-DLOG_ERRORS \
	-D_POSIX_SOURCE

ifeq ($(A2DP_USES_STANDARD_ANDROID_PATH),true)
CFLAGS_COMMON_ANM += -DSTD_A2DP_MNGT
endif
# Host specific flags
CFLAGS_HOST_ANM :=


# Target specific flags.
CFLAGS_TARGET_ANM := \
	-DTARGET_BUILD

# Headers from external packages.
EXTERNAL_INCLUDES_ANM := \
	external/alsa-lib/include \
	$(MULTIMEDIA_PATH)/audio/adm/include \
	$(MULTIMEDIA_PATH)/shared/utils/include

#######################################################################################################
#
# TARGET SHARED LIBRARY #
#
########################################################################################################

ifeq ($(strip $(BOARD_USES_LD_ANM)),true)

#
# Build audio_policy.PLATFORM for Audio Policy Manager
#

# Make a libaudiopolicy which can be used by both audio_policy.PLATFORM and audio.primary.PLATFORM
include $(CLEAR_VARS)


LOCAL_SRC_FILES := $(SRC_FILES_AP)

LOCAL_CFLAGS += $(CFLAGS_COMMON_ANM)

#I don't follow android conventions for c++-files.
LOCAL_CPP_EXTENSION := .cc

LOCAL_MODULE := libaudiopolicy
LOCAL_MODULE_TAGS := optional

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_CFLAGS += -DWITH_BLUETOOTH -DWITH_A2DP
endif

LOCAL_STATIC_LIBRARIES := \
	libmedia_helper

LOCAL_SHARED_LIBRARIES := \
	libste_adm \
	libcutils \
	libstelpcutils \
	libutils \
	libmedia \
	libhardware_legacy \
	libdl \
	libc

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/legacy/include \
	$(EXTERNAL_INCLUDES_ANM)

include $(BUILD_SHARED_LIBRARY)

# Generate the audio_policy.PLATFORM shared library used by Audio HAL
include $(CLEAR_VARS)

LOCAL_MODULE := audio_policy.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_LIBRARIES := \
	libmedia_helper

LOCAL_SHARED_LIBRARIES := \
	libste_adm \
	libaudiopolicy \
	libcutils \
	libutils \
	libmedia \
	libhardware_legacy \
	libdl \
	libc

# Add static link to libaudiopolicy_legacy and expose
# it from this shared library
LOCAL_WHOLE_STATIC_LIBRARIES := \
	libaudiopolicy_legacy

include $(BUILD_SHARED_LIBRARY)

#
# Build audio.primary.PLATFORM for Audio Hardware
#

include $(CLEAR_VARS)
LOCAL_MODULE := audio.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

#Android libraries to link against.
LOCAL_SHARED_LIBRARIES := \
	libste_adm \
	libcutils \
	libstelpcutils \
	libutils \
	libmedia \
	libhardware \
	liblog \
	libasound \
	libdl \
	libaudiopolicy \

#I don't follow android conventions for c++-files.
LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES += \
	$(SRC_FILES_ANM)

LOCAL_CFLAGS += \
	$(CFLAGS_COMMON_ANM) \
	$(CFLAGS_TARGET_ANM)

LOCAL_C_INCLUDES += \
	$(EXTERNAL_INCLUDES_ANM) \
	$(LOCAL_PATH)/legacy/include

# Static libraries to link against. We also need
# the ADM client library.
LOCAL_STATIC_LIBRARIES += \
	libmedia_helper

# Add static link to libaudiohw_legacy and expose
# it from this shared library
LOCAL_WHOLE_STATIC_LIBRARIES := \
	libaudiohw_legacy

#Build an executable for host using the above variables
include $(BUILD_SHARED_LIBRARY)

endif #if BOARD_USES_LD_ANM = TRUE
endif #BOARD_USES_C_AUDIO_HAL
