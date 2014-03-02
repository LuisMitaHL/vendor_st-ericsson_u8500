#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

INSTALL_HEADERDIR= include

OMXDEBUG_SRC:=$(BUILD_TMP_DIR)/OMXdebug.cpp
OMXDEBUG_LOG:=$(BUILD_TMP_DIR)/GenerateOMXdebug.log

DEP_DEP   := $(OMXDEBUG_SRC)
TOBECLEAN := $(OMXDEBUG_SRC) $(OMXDEBUG_LOG)
VPATH     += $(BUILD_TMP_DIR)

SOLIBNAMES += stelpcutils
EXTRALIBS_stelpcutils += osttrace trace los
LIBOBJ_stelpcutils = \
	stelp_log \
	stelp_fatal \
	stelp_time \
	OMXdebug \
	OMXdebug_specific

ifneq ($(STE_SOFTWARE),android)
  FLAGS_stelpcutils += -lpthread -lrt
endif

CPPFLAGS = $(FIXED_CPPFLAGS) \
  -DFREQ_SCALING_ALLOWED \
  -I$(MMROOT)/shared/utils/include \
  -I$(MMROOT)/shared/omxilosalservices_api \
  -I$(MMROOT)/shared/ens_interface/nmf/common \
  -I$(MMROOT)/shared/ens_interface/include \
  -I$(MMROOT)/shared/ens_interface/omxil \
  -I$(MMROOT)/shared/omxil \
  -I$(MMROOT)/linux/trace/api \
  -I$(MMROOT)/audio/drc/proxy \
  -I$(MMROOT)/audio/afm/proxy/include \
  -I$(MMROOT)/audio/audio_chipset_apis \
  -I$(MMROOT)/audio/speech_proc_vcs/wrapper/inc \
  -I$(MMROOT)/video/video_chipset_apis \
  -I$(MMROOT)/video/vfm/include \
  -I$(MMROOT)/audio/comfortnoise/proxy \
  -I$(MMROOT)/shared/ste_shai/include \
  -I$(MMROOT)/shared/ens_interface/nmf/mpc/nmf_repository/$(MMDSPCORE)

ifeq ($(MYMMSYSTEM),android)
  CPPFLAGS += \
    -I$(ANDROID_BUILD_TOP)/hardware/libhardware/include \
    -I$(ANDROID_BUILD_TOP)/frameworks/base/native/include \
    -I$(ANDROID_BUILD_TOP)/frameworks/native/include/media/hardware \
    -I$(ANDROID_BUILD_TOP)/frameworks/native/include
endif

PERL5LIB=
PERL5LIB := $(PWD)/perllibs:$(PERL5LIB)
export PERL5LIB

## GenerateOMXdebug uses MMROOT to search for headers and searh for files in source tree
## not the installed headers as enswrapper build early
export MMROOT
export MMDSPCORE
export MYMMSYSTEM

$(OMXDEBUG_SRC): GenerateOMXdebug
	@echo "  Generating "$(notdir $@)
	$(VERCHAR)mkdir -p $(BUILD_TMP_DIR)
	$(VERCHAR)GenerateOMXdebug $@ > $(OMXDEBUG_LOG) 2>&1

INSTALL_SOLIB = stelpcutils

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

