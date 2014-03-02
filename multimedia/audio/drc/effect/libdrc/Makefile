# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x

# RVDS compiler optimization. In android this flag should be skypped
ifneq ($(SW_VARIANT),android)
USE_OPT_COMPILER=true
endif

include $(MM_MAKEFILES_DIR)/AudioAlgoConfig.mk

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
#
#MODE16   : Set this variable to compile in 16bit mode
#STACK    : Size of the user stack
#SSTACK   : Size of the supervisor stack
#
#LIBS: list of audiolibs libraries to be linked with executable 
#LIBS: list of local libraries to be linked with executable
#PCMLIBS  : list of pcmfile libraries to be linked with executable
#           Do not specify the lib prefix nor the suffix (ex: bitstream to link
#           libbitstream.[a/lib])
#
#----------------------------

LIBNAME = drc
LIBOBJ  = drc_create \
          drc_allocate \
          drc_destroy \
          drc_reset \
          drc_setparam \
          drc_setparam_util \
          drc_getparam \
          drc_execute \
          drc_execute_util \
          drc_fifo

CPPFLAGS   += -I../include
INSTALL_LIB = libdrc.a

ifdef DRC_DEBUG
CFLAGS  += -g -O0
CPPFLAGS+= -DDRC_DEBUG
endif

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(MMCOMPILER),rvct)
CFLAGS  += -O3 -Otime --vectorize --min_array_alignment=8
#CPPFLAGS+= -DARM -DNEON
endif
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

 
