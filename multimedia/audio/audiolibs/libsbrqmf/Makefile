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
LIBNAME = sbrqmf
LIBOBJ  = nopt_flt_analysis_qmf nopt_flt_synthesis_qmf\
	  nopt_bt_analysis_qmf nopt_bt_synthesis_qmf\
	  nopt_bt_ana_cossin_tab_32 nopt_bt_ana_cossin_tab_64\
	  nopt_bt_syn_cossin_tab_32 nopt_bt_syn_cossin_tab_64\
	  nopt_qmf_rom opt_flt_cossin_modulation\
	  opt_flt_analysis_qmf opt_flt_synthesis_qmf\
	  opt_bt_cossin_modulation opt_bt_analysis_qmf\
	  lib_filterbank\
	  lib_filterbank_CA9\
	  opt_bt_synthesis_qmf opt_qmf_rom opt_qmf_fwd_mod_rom

INSTALL_LIB = libsbrqmf.a

CPPFLAGS+=-DNDEBUG 


ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CPPFLAGS+=-DNEON 
CFLAGS+= -Otime
endif
endif

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(PPROFILER_CA9),true)
CFLAGS+=--gnu-instrument
endif
endif

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(MMCOMPILER),gnu)
#CFLAGS+= -mfloat-abi=softfp -mfpu=neon -mfp=vfp
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

