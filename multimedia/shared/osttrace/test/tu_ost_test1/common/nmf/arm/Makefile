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
CORE=arm

include ../../make.ost_conf
OST_COMPONENT_ID = $(OST_COMPONENT_ID_NMF_ARM)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_arm
OST_SRC_DIR = tu_ost_test/dec_arm
#OST_COMPONENT_INC = ./inc
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

SRCPATH += ../mmdsp
SRCPATH += $(MM_NMF_COMP_SRC_MMDSP_DIR)/

CPPFLAGS += -I$(INSTALL_HEADER_DIR) -I$(OST_COMPONENT_INC)/autogentraces
CPPFLAGS += -I$(REPOSITORY) -I$(SHARED_REPOSITORY) -I$(REPOSITORY)/osttrace
CPPFLAGS += -g

SRCPATH += .
SRCPATH += $(COMPONENT_TOP_DIR)/..

ifneq ($(findstring x86,$(MYMMBOARD)),x86) 
COMPONENTS= tu_ost_test.network \
						tu_ost_test.network2
else
COMPONENTS= tu_ost_test.network2
endif
include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk


