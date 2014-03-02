# -*- Mode: Makefile -*-
# vim:syntax=make:
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifndef MMSYSTEM
MMSYSTEM=x
endif
ifndef MMBOARD
MMBOARD=x
endif
CORE=mmdsp


include ../../make.ost_conf
OST_COMPONENT_ID = $(OST_COMPONENT_ID_NMF_DSP)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_dsp
OST_SRC_DIR = tu_ost_test/comp1 tu_ost_test/comp2
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

CFLAGS += -DSTM_TRACE_IP -g -DEXECUTION_CORE=1
CFLAGS += -I$(OST_COMPONENT_INC)/autogentraces -I$(INSTALL_HEADER_DIR)

COMPONENTS= tu_ost_test.comp1 tu_ost_test.comp2

SRCPATH += $(COMPONENT_TOP_DIR)
SRCPATH += $(MM_NMF_COMP_SRC_MMDSP_DIR)
SRCPATH += $(MM_NMF_COMP_SRC_MMDSP_DIR)/tu_ost_test/api

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk



