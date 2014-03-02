# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)/..
endif

## OST config
include $(COMPONENT_TOP_DIR)/make.ost_conf
# KStUidAudioDefine100 = 0x20030571 = 537068913
OST_COMPONENT_ID = 537068913
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR =
#INSTALL_DICO = $(OST_DICO_NAME).xml
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

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
CPPFLAGS+= -I$(REPOSITORY) -I$(COMPONENT_TOP_DIR)/inc

CPPFLAGS+= -I${AUDIO_REPOSITORY_HOST}/speech_proc_nmf_host_composite_speech_proc/src

VPATH+= $(REPOSITORY)/host

LIBOBJ   =  SpeechProcComponent SpeechProcNmf_ProcessingComp speechprocFactoryMethod SpeechProc_PcmPort SpeechProc_ControlPort

ifneq (u5500,$(filter u5500%,$(STE_PLATFORM)))
ifneq ($(findstring x86,$(MMPROCESSOR)),x86)
  CPPFLAGS+=-DUSE_HYBRID_COMPONENT
  LIBOBJ  += speech_proc_cpp-user2mpc-stubs speech_proc_cpp-mpc2user-skels SpeechProcHybridBuffer
endif
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_speech_proc_host_debug
INSTALL_LIB = libst_speech_proc_host_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug
else
LIBNAME     = st_speech_proc_host
INSTALL_LIB = libst_speech_proc_host.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

INSTALL_HEADER  =  # speech_proc.h SpeechProcNmfHostMpc_ProcessingComp.h SpeechProcComponent.h


#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
test cleantest:
	@echo No test to run

