# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

# Default to best guess value if not set
ifndef OST_COMPONENT_INC
  ifndef COMPONENT_TOP_DIR
    $(error COMPONENT_TOP_DIR not defined)
  endif
  OST_COMPONENT_INC=$(COMPONENT_TOP_DIR)/inc
endif

CPPFLAGS += -I$(MM_NMF_COMP_SRC_MMDSP_DIR)/ens_interface/autogentraces
CPPFLAGS += -I$(MM_NMF_COMP_SRC_MMDSP_DIR)/afm/autogentraces
CPPFLAGS += -I$(MM_NMF_COMP_SRC_MMDSP_DIR)/libeffects/autogentraces
CPPFLAGS += -I$(MM_NMF_COMP_SRC_COMMON_DIR)/ens/autogentraces

export SRCPATH
export SHAREPATH
export CPPFLAGS
export CFLAGS
export LDFLAGS
export TARGET
export BUILD

SRCPATH:=$(shell echo "$(SRCPATH)" | sed 's/ /;/g')

COMP    = $(MM_TOOLS_DIR)/comp
RMSDIFF = $(MM_TOOLS_DIR)/rmsdiff
BUILD   = build

NMFCONFC_FLAGS=--pic

all:
	$(NMFCONFC) $(NMFCONFC_FLAGS) test

clean:
	rm -rf build _nmf_tmpdir .mmdsp_history $(TOBECLEAN)
	@echo "Cleaned"

.PHONY: all clean

