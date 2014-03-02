# -*- Mode: Makefile -*-
# vim:syntax=make:
#------------------------------
# Platform config ( UNIX/WIN )
#------------------------------
#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../#XXX no `/` at the end (win32 pb)

MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
include $(MM_MAKEFILES_DIR)/AudioAlgoConfig.mk
#--------------------------
# Local variable
#--------------------------

#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#LDPATH   : Search path for linked libs.(see default value in Make.rules)
#LDLIBS   : List of library name without lib prefix or any suffix
#SHLDLIBS : List of library name without lib prefix or any suffix.
#           Overides LDLIBS for shared libs if specified
#
#NOSTARTFILES : to be defined to generate .so with -nostartfiles
#
#DEPOBJ   : List of object files. (to generate dependencies).
#DEP_DEP  : Dependencies of dependency files.
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#HPATH    : Search Path for header file to be linked. WARNING only one path is possible (see default value in Make.rules)
#HEADER   : List of link to be generated in lib/include
#                       (header files visible to everyone)
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#
#----------------------------
LIBNAME = fft
LIBOBJ  = fft fft_bt fft_ramxy fft_ramxy_bt real_fft_ramxy\
	  real_fft_ramxy_bt fft_ramxy_input_br_bt\
	  fft_ramxy_scaled fft_ramxy_scaled_bt fft_ramxy_scaled_2_bt\
	  fft_ramxy_dyn_scale_bt fft_ramxy_dyn_scale_input_br_bt\
	  fft_ramxy_scaled_input_br_bt buffer_bit_reverse\
	  fft_table\
	  fft_interleaved_table fft_table_1024\
	  fft_interleaved_table_1024 fft_table_2048\
	  lib_fft\
	  fft_interleaved_table_2048\
	  fft16

INSTALL_LIB = libfft.a
DEPOBJ=$(LIBOBJ)

CPPFLAGS += -DOPTIMIZE -DNDEBUG

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CFLAGS+= -Otime 
endif
endif

ifeq ($(MMCOMPILER),flexcc)
CFLAGS = -g -O4
endif


ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(PPROFILER_CA9),true)
CFLAGS+=--gnu-instrument
endif
endif



#WARNING todo target is MANDATORY
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
ifeq ($(MMCOMPILER),flexcc)
NO_OPTIM= fft_ramxy_bt real_fft_ramxy_bt fft_ramxy_input_br_bt\
	  fft_ramxy_scaled_bt fft_ramxy_scaled_2_bt \
	  fft_ramxy_dyn_scale_bt fft_ramxy_dyn_scale_input_br_bt\
	  fft_ramxy_scaled_input_br_bt buffer_bit_reverse

NO_OPTIM_LIST = $(addprefix $(OBJ_DIR)/, $(addsuffix .$(OBJ_EXT),$(NO_OPTIM)))

#warning: does not change -fexpert -O3 order, test fails if options switched ??
$(NO_OPTIM_LIST): CFLAGS= -g -fexpert -O3
endif

