# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
CORE=arm
MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include ../vcs/Make.config


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
CPPFLAGS+= -g
CPPFLAGS+= -I../vcs/inc
VPATH+= src inc
ifeq ($(STE_SOFTWARE),android)
CPPFLAGS+=-DVCS_ANDROID
endif
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
CPPFLAGS+=-DVCS_MMPROBE
endif
ifeq ($(VCSLOG), 1)
CPPFLAGS+=-DVCS_LOG
endif

SOLIBNAMES += speech_processing
LIBOBJ_speech_processing     = speech_proc_vcs ste_speech_proc_omx
EXTRALIBS_speech_processing += ste_vcs drc speech_enhancement spl tty comfortnoise
CPPFLAGS += -I$(MMROOT)/audio/mmprobe/api

# Link with mmprobe if feature enabled in BoardConfig.mk
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
FLAGS_speech_processing += -lmmprobe
endif

INSTALL_SOLIB   = speech_processing
INSTALL_HEADER  = omx_ste_speech_proc.h

# WARNING todo target is MANDATORY (exe and/or lib)
# uninstall is to be sure to install our libspeech_processing.so in mmbuildout
# as is may already exist and in this case makefile may not overwritte it
todo: uninstall solib 

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
