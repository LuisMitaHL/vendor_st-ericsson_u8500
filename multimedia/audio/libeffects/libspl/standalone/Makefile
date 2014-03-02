# vim:syntax=make:
# -*- Mode: Makefile -*-

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
#         For multiple executable, suffix these to variable with _1, _2 ...  
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
ifeq ($(STANDALONE_COMPILATION),true)
STACK=2560
SSTACK=1024

CFLAGS         =-O3

EXENAME = SPL
EXEOBJ  = get_param standalone_spl 
INSTALL_BIN=SPL


ifeq ($(MMPROCESSOR), cortexA9)
ifeq ($(MMCOMPILER),rvct4gnu)
ifeq ($(SUPPORT_CORTEXM4_STANDALONE),true)
EXEOBJ = systick get_param standalone_spl
CFLAGS= --fpu=SoftVFP  --cpu=Cortex-M4  -O3 -Otime --asm -g 
FIXED_LDEXEFLAGS += --fpu=SoftVFP --scatter=scatter_m4.scat --info=unused 
CPPFLAGS+= -DARM -UNEON -DCORTEX_M4
else
EXEOBJ+= retarget_CA9 Init_CA9 InitCache_CA9 Benchmark_CA9
FIXED_LDEXEFLAGS += --scatter=scatter_CA9.scat --entry=0x2000
CPPFLAGS+= -DARM -DNEON
endif
endif
else
ifeq ($(MMPROCESSOR), arm11)
ifeq ($(MMCOMPILER),rvct4gnu)
#EXEOBJ = retarget_ARM11 diagnostic Init_ARM11   heap_ARM11 stack_ARM11 vectors_ARM11 Benchmark_ARM11
#FIXED_LDEXEFLAGS +=  --noremove --scatter=8820_scatter.scf  
#CPPFLAGS+= -DARM 
endif
endif
endif




LIBS= libSPL.a

ifeq ($(MMPROCESSOR), cortexA9)
ifeq ($(SUPPORT_CORTEXM4_STANDALONE),true)
else
LIBS+= ../../../tools/ARM/pmu/lib/cortexA9-x-rvct4gnu-x/libpmu.a
endif
endif


CPPFLAGS+=-I../include

#WARNING todo target is MANDATORY (exe and/or lib)
todo: exe
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

test: 
TEST_TAG =$(MMDSPPROCESSOR)
TESTLISTS= ../test/testlist.lst
DATABASE = misc_std
include $(MM_MAKEFILES_DIR)/AudioTest.mk
