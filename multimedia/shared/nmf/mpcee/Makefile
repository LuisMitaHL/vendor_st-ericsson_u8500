#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(strip $(PLATFORM)),)
  $(error Please define PLATFORM variable environment)
endif
MMDSP_CODE_IS_BOARD_SPECIFIC:=true
CORE=mmdsp

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

ifneq ($(findstring x86,$(MMENV_BOARD)),x86)

#
# Compilation flags
#
export CFLAGS       = -O3 -g $(FIXED_CFLAGS) sstack=768
export BC_CFLAGS    = -O4 -g $(FIXED_CFLAGS)

## NMF MPC runtime entities
MPC_PRELOAD    := preload_mpcee.txt
MPC_COMPONENTS := synchronous_hsem synchronous_lsem hybrid_hsem hybrid_lsem
TO_INSTALL     := $(addprefix $(INSTALL_NMF_REPO_BOARD_DIR)/, $(addsuffix .elf,$(MPC_COMPONENTS)) $(addsuffix .elf4nmf,$(MPC_COMPONENTS)) $(MPC_PRELOAD))
BUILD_OUTPUT   := $(addprefix $(NMF_REPO_DIR)/$(MMDSPCORE)/, $(addsuffix .elf,$(MPC_COMPONENTS)) $(addsuffix .elf4nmf,$(MPC_COMPONENTS)))

################################################################################

all: $(BUILD_OUTPUT) install

FORCE:
ifeq ($(MYMMDSPCORE),mmdsp_8540_v1)
  DSPCORE = mmdsp_9540_v1
else
  DSPCORE = $(MYMMDSPCORE)
endif

$(BUILD_OUTPUT): FORCE
ifeq ($(BUILD_MMDSP_NMF),src)
	$(NMF_TOOLS_DIR)/nmfconfc \
				--executable \
				--target=$(TARGET) \
				--srcpath="$(NMF_TOOLS_DIR)/api/mpcee;$(COMPONENT_TOP_DIR)/src;$(COMPONENT_TOP_DIR)/src/$(DSPCORE)" \
				--tmpoutput=$(COMPONENT_TOP_DIR)/build/$(NMF_TMP_DIR)/$(MMDSPCORE) \
				-o $(COMPONENT_TOP_DIR)/$(NMF_REPO_DIR)/$(MMDSPCORE) \
				$(MPC_COMPONENTS)
endif

realclean clean: uninstall
	rm -rf $(COMPONENT_TOP_DIR)/build/$(NMF_TMP_DIR)/$(MMDSPCORE)
ifeq ($(BUILD_MMDSP_NMF),src)
	rm -rf $(COMPONENT_TOP_DIR)/$(NMF_REPO_DIR)/$(MMDSPCORE)
endif


$(INSTALL_NMF_REPO_BOARD_DIR)/$(MPC_PRELOAD): src/$(DSPCORE)/$(MPC_PRELOAD)
	$(MMINSTALL) $< $@

$(INSTALL_NMF_REPO_BOARD_DIR)/%: $(NMF_REPO_DIR)/$(MMDSPCORE)/%
	$(MMINSTALL) $< $@

install: $(TO_INSTALL)
	$(MMINSTALL) -d src $(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/mpcee

uninstall:
	rm -f $(TO_INSTALL)
	$(MMINSTALL) -d -r src $(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/mpcee

else # ifneq ($(findstring x86,$(MMENV_BOARD)),x86)


all realclean clean install uninstall:

endif # ifneq ($(findstring x86,$(MMENV_BOARD)),x86)

