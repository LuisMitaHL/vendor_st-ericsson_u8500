#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

## This lib is not specific to a given board version but only to the CPU I'm building for
MMBOARD=x
## This lib is not specific to a given system but only the compilation technology used 
## (GCC or RVCT for GNU, RVCT) 
MMSYSTEM=x
## Request to use the compilation technology providing best performance
## (RVCT with GNU compatibility for now to build objects using RVCT but linking with GCC)
USE_OPT_COMPILER=true

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

## Directory to install under your media include dir: $(MMROOT)/multimedia/<media>/include
INSTALL_HEADERDIR= include
## To install specific header files !! VPATH may also be used if this header is in a subdirectory
## in this case the directory structure will be replicated in the media install header dir
INSTALL_HEADER = 

## GNU VPATH may be altered of all sources are not in this directory
VPATH+= include source

## CPPFLAGS may be altered (CFLAGS should not be from the makefiles as most often used to 
## alter them from command line)
ifeq ($(DEBUG),1)
CPPFLAGS     += -DDEBUG
endif

## The list of C/C++ files without their extension to make up the static (.a) lib
## All objects of the lib will be compiled for PIC to later be part of a .so lib
LIBOBJ        := makefiles-tests-lib  makefiles-tests-lib-CC
## The name ot the library to be created (will end up like lib$(LIBNAME).$(LIB_EXT))
LIBNAME       := ste_makefiles_tests

## If you want your resulting library to be installed in you media library folder for the current target
## use the following line - this is really stupid way of requesting the lib to be installed - FIXME
## the library will be installed into $(MMROOT)/multimedia/<media>/lib/<MMPLATFORM> in order for other
## components to use them
INSTALL_LIB   := lib$(LIBNAME).$(LIB_EXT)

## Request library building to be made
todo: lib

## Include the generic rules to perform the work
include $(MM_MAKEFILES_DIR)/SharedRules.mk


