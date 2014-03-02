#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

## Just as for the lib, VPATH may be used

## The executable name (linux targets will not add .exe extensions)
EXENAME      = makefiles-tests
## Files of C/C++ files to make up the executable
EXEOBJ       = makefiles-tests makefiles-tests-CC
## Request the executable to be install in the media platform bin directory:
## $(MMROOT)/multimedia/<media>/bin/<MMPLATFORM> 
INSTALL_BIN  = $(EXENAME)

## Any static library which should be used to makeup the executable
LIBS         = libste_makefiles_tests.a

## Add any specific LDFLAGS (mostly to link against shared library installed by some medias)
EXE_LDFLAGS  =

## FIXME - this should be part of the generic makefiles
## (adding standard location for each media taking care about platform compatibility)
LDFLAGS += $(foreach plat,$(COMPATPLAT) $(MMPLATFORM),-L$(MMROOT)/shared/lib/$(plat))

## Request to make the executable
todo: exe

## Include the generic makefiles rules
include $(MM_MAKEFILES_DIR)/SharedRules.mk

