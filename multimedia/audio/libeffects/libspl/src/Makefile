# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
CORE=mmdsp
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

LIBNAME = SPL
LIBOBJ  = spl spl_M4 spl_util spl_util_M4 spl_dsp spl_dsp_M4 spl_rom spl_rom_M4 
INSTALL_LIB=libSPL.a

CPPFLAGS+=-I../include -DNDEBUG



ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(MMCOMPILER),rvct4gnu)
ifeq ($(SUPPORT_CORTEXM4_STANDALONE),true)
CFLAGS+= -DARM -UNEON --fpu=SoftVFP --cpu=Cortex-M4 -O3 -Otime --asm
else
CPPFLAGS+= -DARM -DNEON
endif
endif
#(to get the txt file containing the assemply code) CFLAGS+=--asm --interleave
else	
ifeq ($(MMPROCESSOR),arm11)
ifeq ($(MMCOMPILER),rvct4gnu)
CPPFLAGS+= -DARM 
endif
endif
endif



#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

