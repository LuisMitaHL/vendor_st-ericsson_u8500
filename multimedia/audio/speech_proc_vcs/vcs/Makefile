# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
CORE=arm
MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk



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
#LOCALLIBS: list of local libraries to be linked with executable
#
#----------------------------
include ./Make.config

CPPFLAGS += -I$(MMROOT)/audio/mmprobe/api

VPATH+= src inc

LIBOBJ  = vcs_rx vcs_tx vcs_drc vcs_setx vcs_serx vcs_common vcs_queue \
	vcs_ctmrx vcs_ctmtx vcs_circbuff vcs_plugin vcs_comfortnoise

ifeq ($(VCS_SINE_GENERATION_ENABLE),true)
CPPFLAGS+=-DVCS_SINE_GENERATION
LIBOBJ  += vcs_sine
endif
LIBNAME = ste_vcs

INSTALL_LIB     = libste_vcs.a
INSTALL_HEADER  = 

#WARNING todo target is MANDATORY (exe and/or lib)
todo:   lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
test cleantest:
	@echo "no test"




.PHONY: test
