# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x

## The MMDSP code is board specific
##MMDSP_CODE_IS_BOARD_SPECIFIC:=true

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := ..
endif

include ../make.ost_conf

# 537072983 is the ID reseverd for the audiocodec proxy
OST_COMPONENT_ID = $(KUidDicAudiocodecProxy)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
INSTALL_DICO=$(OST_DICO_NAME).xml


include $(MM_MAKEFILES_DIR)/AudioConfig.mk

CPPFLAGS+= -I$(MM_HEADER_DIR)/volctrl


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

ITF_PREFIX = audiocodec

CPPFLAGS+= -I$(REPOSITORY) -Icommon
CPPFLAGS += -I../inc -Iinclude -Idebug -Iavsink -Iavsource -Ibtrx -Ibttx -Icommon -Iearpiece -Ifmrx -Ifmtx -Iihf -Imultimic -Ivibra

VPATH+= $(COMPONENT_TOP_DIR)/mpc/$(BASE_REPOSITORY)/host
VPATH+= debug avsink avsource btrx bttx common earpiece fmrx fmtx ihf multimic vibra

CPPFLAGS += -I$(MMROOT)/audio/alsactrl/include
CPPFLAGS += -lalsactrl

ifeq ($(STE_PLATFORM),u8500)
  CPPFLAGS += -DAUDIOCODEC_U8500
endif

ifeq ($(STE_PLATFORM),u9540)
  CPPFLAGS += -DAUDIOCODEC_U9540
endif

ifeq ($(STE_PLATFORM),u9540_ab8540)
  CPPFLAGS += -DAUDIOCODEC_U9540
endif

LIBOBJ = audiocodec_cpp-user2mpc-stubs audiocodec_cpp-mpc2user-skels \
	audiocodec_debug \
	NmfMpcDmaMsp NmfMpcBase NmfMpcInOut \
	AudiocodecBaseNmfMpc AudiocodecBase \
	BindContext \
	MultiMic AvSource FmRx BtRx \
	AvSink IntHandsFree Earpiece Vibra FmTx BtTx

LIBNAME     = st_audiocodec
INSTALL_LIB = libst_audiocodec.a

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
