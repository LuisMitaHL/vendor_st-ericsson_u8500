#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(MULTIMEDIA_SET_PLATFORM),u9500)
  MKFLAGS_audio_cscall := EXTRA_CPPFLAGS='-DMAI_PROTOCOL=1 -DCSCALL_ENABLE_FEATURE_CAIF=1'
endif

