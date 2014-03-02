# vim:syntax=make:
# -*- Mode: Makefile -*-
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#
#DEP_DEP  : Dependencies of dependency files. 
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#         For multiple executable, suffix these to variable with _1, _2 ...  
#
#----------------------------
MMSYSTEM=x
MMBOARD=x

include ../make.ost_conf
OST_COMPONENT_ID = $(OST_COMPONENT_ID_HOST)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_host_arm
OST_SRC_DIR = src
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

VPATH += src
VPATH += $(SHARED_REPOSITORY)/host 

ifneq ($(findstring x86,$(MYMMBOARD)),x86) 
LIBOBJ += common proxy tu_ost_test_common_cpp-mpc2user-skels tu_ost_test_common_cpp-user2mpc-stubs
else
LIBOBJ += common proxy	
endif
LIBNAME = st_osttest_common
INSTALL_LIB   = libst_osttest_common.a

todo: lib

include $(MM_MAKEFILES_DIR)/SharedRules.mk























































