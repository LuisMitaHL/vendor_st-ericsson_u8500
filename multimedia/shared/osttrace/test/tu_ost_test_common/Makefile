#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

#INSTALL_NMF_COMP_SRC_COMMON := $(wildcard tu_ost_test/api/*.idt tu_ost_test/api/*.itf)
#INSTALL_NMF_COMP_SRC_MMDSP += $(wildcard tu_ost_test/api/*.idt  tu_ost_test/api/*.itf)
INSTALL_NMF_COMP_SRC_MMDSP += tu_ost_test/api
SRCPATH += $(MM_NMF_COMP_SRC_MMDSP_DIR)
SRCPATH += $(MM_NMF_COMP_SRC_MMDSP_DIR)/tu_ost_test/api
SRCPATH += $(COMPONENT_TOP_DIR)/test/tu_ost_test_common/tu_ost_test/api

ifneq ($(findstring x86,$(MYMMBOARD)),x86)

ITF_PREFIX = tu_ost_test_common
USER2MPCITFS_CPP += tu_ost_test.api.gentrace tu_ost_test.api.traceinit tu_ost_test.api.add tu_ost_test.api.configure
MPC2USERITFS_CPP +=  tu_ost_test.api.uniop
ASYNCITFS += tu_ost_test.api.uniop
INTERMPCITFS += tu_ost_test.api.uniop tu_ost_test.api.add
endif

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk
