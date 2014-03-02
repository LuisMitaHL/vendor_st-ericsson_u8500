# -*- Mode: Makefile -*-
# vim:syntax=make:
CORE=arm
MMSYSTEM=x
MMBOARD=x
include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

.DEFAULT_GOAL:=all

## img_network is both an HOSTEE and MPC NMF component so get ENS NMF MPC sources
SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)
SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/common

## NMF MPC part inside this SW component are needed
SRCPATH+=$(COMPONENT_TOP_DIR)/nmf/mpc
SHAREPATH+=$(COMPONENT_TOP_DIR)/share

CPPFLAGS+= -I$(MM_HEADER_DIR)
CPPFLAGS+= -I$(MM_HEADER_BASEDIR)/shared
CPPFLAGS+= -I$(MM_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/
CPPFLAGS+= -I$(MM_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/host

#ENS post version 2011.35.0 introduces an API break. When this define is active, we compile for versions
#2011.35.0 or prior. The API break adds a new event OMX_EventCmdReceived to the Event enum.
# CPPFLAGS += -DENS_2011_35_0_PRIOR

CXXFLAGS+=-g -O0

COMPONENTS= ext_img_network 
# the following are provided by NMF, but not nmf-proxy interfaces, so generate them ourselves
ITFS_HEADER= timer.api.timer timer.api.alarm

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

#NMFCONFC_FLAGS=--cpp --archive
#export NMFCONFC_FLAGS
