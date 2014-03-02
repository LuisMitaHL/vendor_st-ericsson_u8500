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

include $(COMPONENT_TOP_DIR)/make.ost_conf
OST_COMPONENT_ID = $(shell expr $(OST_MODULE_ID) + 1 )
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR =
INSTALL_DICO = $(OST_DICO_NAME).xml
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
# do this so that the cscall stubs-skels find the C++ NMF interfaces first (instead of the ENS ones defined in C)
CPPFLAGS := -I$(AUDIO_REPOSITORY_MPC) $(CPPFLAGS)
CPPFLAGS+= -I$(REPOSITORY) -I../common/include

ifeq ($(NMF_ENV), linux)
CPPFLAGS+= -I../linux/audiomodem_driver/inc
endif

VPATH+= $(REPOSITORY)/host

LIBOBJ   =  CsCallComponent CsCall_PcmPort CsCallNmfHost_ProcessingComp cscallFactoryMethod CsCall_ControlPort

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_cscall_host_debug
INSTALL_LIB = libst_cscall_host_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug
else
LIBNAME     = st_cscall_host
INSTALL_LIB = libst_cscall_host.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

CPPFLAGS += $(EXTRA_CPPFLAGS)

ifeq ($(findstring u5500, $(PLATFORM)),u5500)
  CPPFLAGS+=-DMAI_PROTOCOL
endif


ifeq ($(findstring u9500,$(PLATFORM)), u9500)
  CPPFLAGS+=-DMAI_PROTOCOL
endif
INSTALL_HEADER  =  CsCallNmfHost_ProcessingComp.h CsCallComponent.h

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

