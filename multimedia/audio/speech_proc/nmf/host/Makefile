# -*- Mode: Makefile -*-
# vim:syntax=make:
CORE=arm
MMSYSTEM=x
MMBOARD=x

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)/../..
endif

## OST config
include $(COMPONENT_TOP_DIR)/make.ost_conf
# KStUidAudioDefine101 = 0x20030572 = 537068914
OST_COMPONENT_ID = 537068914
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_host
OST_SRC_DIR  = composite speechcontroller dl_wrapper ul_wrapper time_align
INSTALL_DICO = $(OST_DICO_NAME).xml

## Common Config
include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

SRCPATH+=$(COMPONENT_TOP_DIR)/nmf/common

ITF_PREFIX = speech_proc

SHAREPATH+=$(COMPONENT_TOP_DIR)/inc
SHAREPATH+=$(COMPONENT_TOP_DIR)/proxy
SHAREPATH+=$(INSTALL_HEADER_DIR)/include

CXXFLAGS+=-g -O3

INTERMPCITFS=

USER2MPCITFS= 

MPC2USERITFS= 

KERNEL2MPCITFS=

MPC2KERNELITFS=

ASYNCITFS=

ITFS_HEADER=speech_proc.nmf.host.common.configure \
            speech_proc.nmf.common.configure_time_align \
            speech_proc.nmf.host.common.sidetone \
            hst.pcmprocessings.wrapper.configure \
            samplerateconv.nmfil.host.effect.configure \


ifeq (u5500,$(filter u5500%,$(STE_PLATFORM)))
  CPPFLAGS+=-DUNIX -DARM
  COMPONENTS= speech_proc.nmf.host.composite.speech_proc
else
ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
  CPPFLAGS+=-DUNIX -DARM
  COMPONENTS= speech_proc.nmf.host.composite.speech_proc
else
  CPPFLAGS+=-DUSE_HYBRID_COMPONENT
  COMPONENTS= speech_proc.nmf.host.composite.speech_proc_hybrid
endif
endif


include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk


test cleantest:
	@echo "no test"
