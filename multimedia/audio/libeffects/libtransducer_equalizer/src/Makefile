# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
#-------------------------------
# Default Config
#-------------------------------
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

ifeq ($(MMCOMPILER),flexcc)

CFLAGS=-g -O3 # -O4 may generate bug in mmdsp version because of pointers which are not restricted
LIBNAME = transducer_equalizer
LIBOBJ  = transducer_equalizer_src
INSTALL_LIB=libtransducer_equalizer.a

endif


ifeq ($(MMCOMPILER),rvct4gnu)

LIBNAME = transducer_equalizer_CA9
LIBOBJ  = transducer_equalizer_src_CA9
INSTALL_LIB=libtransducer_equalizer_CA9.a

endif

INSTALL_HEADER+= transducer_equalizer_alloc.h transducer_equalizer.h transducer_equalizer_CA9.h effect_common.h

INSTALL_HEADER_PREFIX_DIR:=libeffects/libtransducer_equalizer/include/

VPATH=../include

CPPFLAGS+=-I../include -DNDEBUG

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

