#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

CORE=arm
MMBOARD=x
MMSYSTEM=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS     += -I$(REPOSITORY_HOST) -I$(SHARED_REPOSITORY_HOST) -I$(COMPONENT_TOP_DIR)/include -I$(MM_HEADER_DIR) -I$(INSTALL_HEADER_DIR)

## VPATH must remain relative
VPATH        += src/linux ../include
LIBOBJ        = osttrace
LIBNAME       = osttrace
INSTALL_LIB   = libosttrace.a

INSTALL_HEADER  = OpenSystemTrace_ste.h osttrace.h osttrace_p.h

todo: lib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

.PHONY: copy_release copy stream_link doc init.ate mmgdb_script_audio_dsp mmgdb_script_video_dsp init.ate test

.SECONDARY:
