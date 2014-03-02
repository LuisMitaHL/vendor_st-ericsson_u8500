#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
MMBOARD=x
MMSYSTEM=x

# This will make use of RVCT under Linux and allow to deliver pre-build library
#USE_OPT_COMPILER=true

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -DRELEASE -D__RELEASE 
ASMFLAGS +=  -mfpu=neon

LIBNAME   = ste_aiq_tools
INSTALL_LIB = libste_aiq_tools.a

LIBOBJ = AIQ_UYUV422Itld_conversions \
         AIQ_UYUV422Itld_rotations \
         AIQ_YUV420MB_rotations \
         AIQ_YUV420P_rotations \
         AIQ_image_cropping \
         AIQ_YUV420MB_conversions \
         AIQ_image_scaling

ifneq ($(PLATFORM),x86-linux)
LIBOBJ += \
         AIQ_RGB565_conversions \
         AIQ_YUV420MB_rotations_neon \
         AIQ_YUV420P_rotation_neon \
         AIQ_YUV420SP_rotations_neon \
         AIQ_asm_kernels
endif

INSTALL_HEADER = $(subst inc/,,$(shell find inc -type f -name "*.h" ))

CPPFLAGS+= -I$(MM_HEADER_DIR)
CPPFLAGS+= -I${MEDIUM_DIR}/$(BASE_REPOSITORY)

todo: lib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
