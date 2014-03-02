#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(MULTIMEDIA_SET_PLATFORM),u9500)
  STE_PLATFORM:=u8500
else
  STE_PLATFORM:=$(MULTIMEDIA_SET_PLATFORM)
endif

## MM modules content are from Android SI source tree
override SW_VARIANT=android

#### SW_VARIANT as used by the MM modules always check for android even if building for lbp
#### STE_SOFTWARE is introduce to compensate this issue and has expected meaning of SW_VARIANT
ifeq ($(STE_SOFTWARE),)
  STE_SOFTWARE:=$(SW_VARIANT)
endif

ifeq ($(STE_PLATFORM),u8500)
  ifeq ($(STE_SOFTWARE),android)
    PLATFORM=u8500_v2-android
  endif
  ifeq ($(STE_SOFTWARE),lbp)
    PLATFORM=u8500_v2-linux
  endif
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS=u9540
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS+=l8540
endif

ifeq ($(STE_PLATFORM),u5500)
  ifeq ($(STE_SOFTWARE),android)
    PLATFORM=svp5500_v1-android
  endif
  ifeq ($(STE_SOFTWARE),lbp)
    PLATFORM=svp5500_v1-linux
  endif
endif

ifeq ($(STE_PLATFORM),u9540)
  ifeq ($(STE_SOFTWARE),android)
    PLATFORM=u9540_v1-android
  endif
  ifeq ($(STE_SOFTWARE),lbp)
    PLATFORM=u9540_v1-linux
  endif
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS=u8500
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS+=l8540
endif

ifeq ($(STE_PLATFORM),l8540)
  ifeq ($(STE_SOFTWARE),android)
    PLATFORM=l8540_v1-android
  endif
  ifeq ($(STE_SOFTWARE),lbp)
    PLATFORM=l8540_v1-linux
  endif
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS=u8500
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS+=u9540
endif

ifeq ($(STE_PLATFORM),x86)
  PLATFORM=$(STE_SOFTWARE)
  BUILD_MMDSP_ALTERNATE_STE_PLATFORMS=
endif


STE_MM_C_INCLUDES := \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/shared/omxil \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/shared \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/linux \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/audio \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/audio/audiolibs/common/include \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/audio/audiolibs/fake_dsptools \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/imaging \
  $(PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/video

STE_MM_SHARED_LIBRARIES := \
  libste_ens_audio_samplerateconv.so \
  libalsactrl.so \
  libomxil-bellagio.so \
  ste_omxcomponents/libste_audio_mixer.so \
  libstelpcutils.so
