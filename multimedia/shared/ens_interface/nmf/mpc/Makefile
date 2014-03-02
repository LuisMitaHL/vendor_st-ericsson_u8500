# -*- Mode: Makefile -*-
# vim:syntax=make:
include ../../make.ost_conf
# KStEnsOstTraceNmfMpc  = KStUidCommonDefine017 = 0x2003064C = 537069132
OST_COMPONENT_ID = 537069132
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_mpc
OST_SRC_DIR = fsm
CPPFLAGS+=-I$(COMPONENT_TOP_DIR)/inc/autogentraces
INSTALL_DICO+=ENS_INTERFACE_nmf_mpc.xml

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

ITF_PREFIX = ens

SRCPATH += $(COMPONENT_TOP_DIR)
SRCPATH += $(COMPONENT_TOP_DIR)/nmf/mpc
SRCPATH += $(COMPONENT_TOP_DIR)/nmf/common
SRCPATH += $(COMPONENT_TOP_DIR)/nmf/mpc/common

INTERMPCITFS=\
	emptythisbuffer \
	fillthisbuffer

ASYNCITFS= \
	emptythisbuffer \
	fillthisbuffer \
	eventhandler \
	sendcommand \
	fsminit \
	postevent

USER2MPCITFS_CPP= shared_emptythisbuffer\
	shared_fillthisbuffer\
	sendcommand \
	fsminit \
	dummy

MPC2USERITFS_CPP= shared_emptythisbuffer\
	shared_fillthisbuffer\
	eventhandler \

fsm = \
	fsm.generic \
	fsm.component

shared_buf = \
	nmf.mpc.shared_bufin \
	nmf.mpc.shared_bufout

COMPONENTS = $(fsm) $(shared_buf)

INSTALL_COMPONENTS = $(fsm) $(shared_buf)
## Below line will install .type .idt .idl .idt .itf while above install .conf .adl
INSTALL_COMPONENTS += fsm.component.component fsm.generic.generic

## Request sources to be installed along with .conf
INSTALL_NMF_COMP_SRC=true

INSTALL_NMF_COMP_SRC_MMDSP += common 

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

