#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifneq ($(PLATFORM),x86-linux)
  CFLAGS   += -fvisibility=hidden
  CXXFLAGS += -fvisibility=hidden
endif

ifneq ($(MMENV_SYSTEM), android)
  LDSOFLAGS += -pthread -lrt
endif

EECONF=nmfHostOnlyEE
ifeq ($(findstring x86,$(PLATFORM)),)
  ifeq ($(findstring 5500,$(PLATFORM)),)
    EECONF=nmfDistributedEE
    NMF_LD_FLAGS = -lnmf
  endif
endif

SOLIBNAMES        += nmfee
SOEXTRALIBS_nmfee  = $(BUILD_INTERMEDIATES_DIR)/../hostee/$(NMF_REPO_DIR)/$(MMPLATFORM)/$(EECONF).lib
FLAGS_nmfee        = $(NMF_LD_FLAGS)
INSTALL_SOLIB     += nmfee

$(INSTALL_LIB_DIR)/libnmfee.a: $(BUILD_INTERMEDIATES_DIR)/../hostee/$(NMF_REPO_DIR)/$(MMPLATFORM)/$(EECONF).lib
	$(MMINSTALL) $< $@

todo: solib install

ifeq ($(BUILD_ARM_CODE),)
install:
else
install: $(INSTALL_LIB_DIR)/libnmfee.a
endif

SOLIBNAMES           += nmftimer
SOEXTRALIBS_nmftimer  = $(BUILD_INTERMEDIATES_DIR)/../hostee/$(NMF_REPO_DIR)/$(MMPLATFORM)/timerWrapped.lib
FLAGS_nmftimer        = $(NMF_LD_FLAGS) -lnmfee -L$(LIB_DIR)
INSTALL_SOLIB        += nmftimer

$(INSTALL_LIB_DIR)/libnmftimer.a: $(BUILD_INTERMEDIATES_DIR)/../hostee/$(NMF_REPO_DIR)/$(MMPLATFORM)/timerWrapped.lib
	$(MMINSTALL) $< $@

todo: solib install

ifeq ($(BUILD_ARM_CODE),)
install:
else
install: $(INSTALL_LIB_DIR)/libnmftimer.a
endif

## Fix for backward compatibility on components performing an include of timer/timerWrapped.hpp
## from what used to be in $(COMPONENT_TOP_DIR)/api/timer/timerWrapped.hpp
## Following files needs to be fixed to remove fix (They will have to be fixed for RPM in any case)
# audio/cscall/nmf/host/composite/cscall/test/src/main.cpp:#include <timer/timerWrapped.hpp>
# audio/cscall/nmf/host/composite/cscalltest1/test/src/main.cpp:#include <timer/timerWrapped.hpp>
# audio/cscall/proxy/CsCallNmfHost_ProcessingComp.h:#include <timer/timerWrapped.hpp>
# imaging/hsmcamera/include/camera.h:#include "timer/timerWrapped.hpp"
## keep this one in todo: and not install: as for binary packages api is installed
todo: $(COMPONENT_TOP_DIR)/api/timer/timerWrapped.hpp

$(COMPONENT_TOP_DIR)/api/timer/timerWrapped.hpp:
	mkdir -p $(COMPONENT_TOP_DIR)/api/timer
	cp -f $(BUILD_INTERMEDIATES_DIR)/../hostee/$(NMF_REPO_DIR)/$(MMPLATFORM)/timerWrapped.hpp $@

include $(MM_MAKEFILES_DIR)/SharedRules.mk
