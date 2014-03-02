# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
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

LIBNAME = resampling
LIBOBJ  = resample_init_fast_hamaca resample_init_fast_hamaca_M4 resample_table12x resample_table44_48 resample_table44_48LR resample_table48_44 resample_table16_48_low_latency resample_table48_16_low_latency resample_table8_16_low_latency resample_table16_8_low_latency resample_table_incr_offset resample_table44_48H resample_table48_44H resample_table12xH resample_table7_5H resample_table7_9H resample_top_fixin_fixout  resample_ML_block_fixin_fixout resample_ML_block_fixin_fixout_M4 resample_ML_filter_block_fixin_fixout resample_ML_filter_block_fixin_fixout_bt resample_ML_filter_block_fixin_fixout_ca9   

INSTALL_HEADER+= resample.h resample_max_delay.h

INSTALL_HEADER_PREFIX_DIR:=libeffects/libresampling/include/

INSTALL_LIB = libresampling.a

VPATH=../include

CPPFLAGS+=-I../include -DNDEBUG

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(MMCOMPILER),rvct4gnu)
CPPFLAGS+= -DARM -DNEON
ifeq ($(SUPPORT_CORTEXM4_STANDALONE),true)
CFLAGS+= --cpu=Cortex-M4 --fpu=SoftVFP -O3 -Otime --asm
CPPFLAGS+= -UNEON -DM4_LL_MSEC -DOPT_M4 
endif
endif
#(to get the txt file containing the assemply code) CFLAGS+=--asm --interleave
else	
ifeq ($(MMPROCESSOR),arm11)
ifeq ($(MMCOMPILER),rvct4gnu)
#CPPFLAGS+= -DARM 
endif
endif
endif
#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

test cleantest:
	@echo No Test...
 
