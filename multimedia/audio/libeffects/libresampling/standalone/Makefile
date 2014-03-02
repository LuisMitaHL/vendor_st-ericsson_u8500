# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x

#remove this if checking cycles for CA9 target on board
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
ifeq ($(STANDALONE_COMPILATION),true)
CPPFLAGS+=-I../include

EXEOBJ=diagnostic
ifeq ($(MMPROCESSOR), cortexA9)
EXEOBJ+=  Profile
endif

EXEOBJ_2=SrcMultiChannel
ifeq ($(MMPROCESSOR), cortexA9)
ifeq ($(MMCOMPILER),rvct4gnu)
EXEOBJ = retarget_CA9 diagnostic Init_CA9 InitCache_CA9 Benchmark_CA9
EXEOBJ_2 = retarget_CA9 SrcMultiChannel Init_CA9 InitCache_CA9 Benchmark_CA9
FIXED_LDEXEFLAGS += --scatter=scatter_CA9.scat --entry=0x2000
CPPFLAGS+= -DARM -DNEON
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

EXENAME=diagnostic
EXENAME_2=SrcMultiChannel
LIBS+=libresampling.a  libvector.a libbtrue.a
ifeq ($(MMPROCESSOR), cortexA9)
LIBS+= ../../../tools/ARM/pmu/lib/cortexA9-x-rvct4gnu-x/libpmu.a
endif
LIBS_2+=libresampling.a  libvector.a libbtrue.a
INSTALL_BIN=diagnostic SrcMultiChannel
todo: exe
endif
	
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk


test: 
#	@perl verify.pl -fin48 -fout44;\
#	    if [ $$? -ne 0 ] ; then                    	\
#    echo "\n verify $@ FAILED \n"; exit 1;      	\
#    fi;											\
#    echo "verify $@ OK\n"

TEST_TAG =$(MMDSPPROCESSOR)
TESTLISTS= testlist.lst
DATABASE = misc_std
include $(MM_MAKEFILES_DIR)/AudioTest.mk

Clean:
	rm -f *.dcv
	rm -f *.prof
	rm -f *.pcv
	rm -f SRCRES.txt
	rm -f srclog.txt
	rm -f evaldist 
	rm -f convrt 
	rm -f singen 
	rm -f addsig 
	rm -f findmax
	rm -f srcres.txt
	rm -f float_sig.txt

