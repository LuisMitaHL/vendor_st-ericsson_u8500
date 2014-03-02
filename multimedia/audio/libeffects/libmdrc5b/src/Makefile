# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
#CORE=mmdsp
MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
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

LIBNAME = mdrc5b
LIBOBJ  = mdrc5b mdrc5b_rom mdrc5b_util mdrc5b_dsp mdrc5b_compressor mdrc5b_limiter mdrc5b_filters
INSTALL_LIB=libmdrc5b.a

endif


ifeq ($(MMCOMPILER),rvct4gnu)

LIBNAME = mdrc5b_CA9
LIBOBJ  = mdrc5b mdrc5b_util_CA9 mdrc5b_compressor_CA9 mdrc5b_limiter_CA9 mdrc5b_filters
INSTALL_LIB=libmdrc5b_CA9.a

endif


ifeq ($(MMPROCESSOR),x86_mmdsp)

LIBNAME = mdrc5b
LIBOBJ  = mdrc5b mdrc5b_rom mdrc5b_util mdrc5b_dsp mdrc5b_compressor mdrc5b_limiter mdrc5b_filters
INSTALL_LIB=libmdrc5b.a

endif

INSTALL_HEADER+= mdrc5b.h mdrc5b_options.h mdrc5b_filters.h debug_samples.h mdrc5b_filters_design.h

INSTALL_HEADER_PREFIX_DIR:=libeffects/libmdrc5b/include/

VPATH=../include

CPPFLAGS+=-I../include -DNDEBUG 

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

