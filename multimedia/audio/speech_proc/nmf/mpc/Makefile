# -*- Mode: Makefile -*-
# vim:syntax=make:

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)/../..
endif

## OST config
include $(COMPONENT_TOP_DIR)/make.ost_conf
# KStUidAudioDefine102 = 0x20030573 = 537068915
OST_COMPONENT_ID = 537068915
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_mpc
OST_SRC_DIR = time_align dsp_port
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

ifneq (u5500,$(filter u5500%,$(STE_PLATFORM)))
ifneq ($(findstring x86,$(MMPROCESSOR)),x86)

# just for hst.common.pcmsettings
SRCPATH+=$(MM_NMF_COMP_SRC_HOSTEE_DIR)/afm

ITF_PREFIX = speech_proc

CFLAGS=-g -O3
SHAREPATH+=$(COMPONENT_TOP_DIR)/inc
SHAREPATH+=$(COMPONENT_TOP_DIR)/nmf/common
CPPFLAGS+=-DFORCE_MONO_REFERENCE

MPC2USERITFS_CPP=speech_proc.nmf.common.pcmsettings \
                 speech_proc.nmf.common.hybrid_emptythisbuffer \
                 speech_proc.nmf.common.hybrid_fillthisbuffer 	

USER2MPCITFS_CPP=speech_proc.nmf.common.configure_time_align \
	             speech_proc.nmf.mpc.dsp_port.configure \
                 speech_proc.nmf.common.hybrid_emptythisbuffer \
                 speech_proc.nmf.common.hybrid_fillthisbuffer \
		         hst.common.pcmsettings 

COMPONENTS= speech_proc.nmf.mpc.time_align \
	        speech_proc.nmf.mpc.dsp_port \
            speech_proc.nmf.mpc.hybrid_buffer.arm2mpc \
            speech_proc.nmf.mpc.hybrid_buffer.mpc2arm 

ASYNCITFS=
endif # MMPROCESSOR == x86
endif # u5500

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

