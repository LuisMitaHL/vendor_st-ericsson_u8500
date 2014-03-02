#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

#------------------------------------------------
# Warning: Following variables must be defined 
#
# CPPFLAGS       : preprocessing directive for nmf toolset
# SRCPATH        : sources directories (separator is ;)
# INTERMPCITFS   : mpc to mpc interfaces
# USER2MPCITFS   : host-user to mpc interfaces
# MPC2USERITFS   : mpc to host-user interfaces
# KERNEL2MPCITFS : host-kernel to mpc interfaces
# MPC2KERNELITFS : mpc to host-kernel interfaces
# ASYNCITFS      : asynchronous interfaces
# COMPONENTS     : nmf components
# ITF_PREFIX     : stub/skel file prefix
#----------------------------------------------

# OST trace compiler
include $(MM_MAKEFILES_DIR)/SharedTraceCompiler.mk

ifneq ($(CORE),mmdsp)
  ## Ensure highest optimisation level is used for ARM
  ## For DSP each component explicit the CFLAGS to be used
  CFLAGS     := $(CFLAGS)   $(OPTIMIZ_CFLAGS)
  CXXFLAGS   := $(CXXFLAGS) $(OPTIMIZ_CFLAGS)
endif

# Add CPPFLAGS for the variant we are building
CPPFLAGS += $(MM_VARIANT_CPPFLAGS$(BUILDING_MM_VARIANT))

SRCPATH:=$(shell echo "$(SRCPATH)" | sed 's/ /;/g')
SHAREPATH:=$(shell echo "$(SHAREPATH)" | sed 's/ /;/g')

## This VPATH use is a hugly fix is to be able to install NMF sources when
## components names is based on some directories names being parent of current
## directory. VPATH below will allow to consider those directories
NMF_COMP_SEARCH_DIRS:=. .. ../.. ../../..
VPATH += $(NMF_COMP_SEARCH_DIRS)

NMF_COMP_DEF_DIRS := $(subst .,/,$(INSTALL_COMPONENTS))
NMF_COMP_DEF_EXTS += .type .idt .idl .idt .itf
NMF_COMP_DEF_DIR_EXTS += .conf .adl 

DEFAULT_NMF_RULES_ALL_TARGET := interfaces components install

ifeq ($(CORE),mmdsp)

  REPOSITORY_DST=$(INSTALL_NMF_REPO_BOARD_DIR)$(BUILDING_MM_VARIANT)
  ifeq ($(BUILD_MMDSP_NMF),bin)
    NMF_RULES_ALL_TARGET := install
    STUB_TARGETS         := true
  else
    NMF_RULES_ALL_TARGET := $(DEFAULT_NMF_RULES_ALL_TARGET)
  endif

  #################### Rules to install the NMF sources

  ## From the given component to build, search for the NMF extensions for this component
  ## and install them if existing. We are also looking into the NMF_COMP_SEARCH_DIRS
  ## as NMF sources location is not always clean ...
  INSTALL_NMF_COMP_SRC_MMDSP += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
				  $(addprefix $(path)/, \
				    $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
				      $(foreach ext,$(NMF_COMP_DEF_EXTS), \
				        $(addsuffix $(ext),$(cmp))))))

  INSTALL_NMF_COMP_SRC_MMDSP += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
				  $(addprefix $(path)/, \
				    $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
				      $(addprefix $(cmp)/, \
				        $(foreach ext,$(NMF_COMP_DEF_DIR_EXTS), \
				          $(addsuffix $(ext),$(notdir $(cmp))))))))
  ifdef INSTALL_NMF_COMP_SRC
    INSTALL_NMF_COMP_SRC_MMDSP += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
	   			    $(addprefix $(path)/, \
				      $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
					$(foreach dir,/inc /src /include, \
				          $(addsuffix $(dir),$(cmp))))))
  endif

  ## Get only existing files
  INSTALL_NMF_COMP_SRC_MMDSP:=$(wildcard $(INSTALL_NMF_COMP_SRC_MMDSP))

  ## The relative path were just here to look for existing files, we need to remove them
  ## for make to use VPATH and get destination directory as we want it
  INSTALL_NMF_COMP_SRC_MMDSP:=$(subst ../,,$(INSTALL_NMF_COMP_SRC_MMDSP))
ifneq ($(MM_BUILD_FOR_PACKAGING),true)
  INSTALL_NMF_COMP_SRC_MMDSP:=$(subst $(subst /.,,$(BUILD_INTERMEDIATES_DIR)),,$(INSTALL_NMF_COMP_SRC_MMDSP))
endif
  ## All files/dirs we'll install as NMF MMDSP sources
  NMF_COMP_SRC_MMDSP_INSTALL += $(addprefix $(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/$(INSTALL_NMF_COMP_SRC_MMDSP_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_MMDSP))

else

  REPOSITORY_DST=$(REPOSITORY_HOST)
  ifeq ($(BUILD_ARM_NMF),src)
    NMF_RULES_ALL_TARGET := $(DEFAULT_NMF_RULES_ALL_TARGET)
  else
    ## Only perform install when 'bin' or '' (no build) requested
    NMF_RULES_ALL_TARGET := install
    STUB_TARGETS         := true
  endif

  ifndef NMFCONFC_NO_ARCHIVE
    NMFCONFC_FLAGS+= --archive
  endif
  ifndef NMFCONFC_NO_CPP
    NMFCONFC_FLAGS+= --cpp
  endif

  #################### Rules to install the NMF sources

  ## From the given component to build, search for the NMF extensions for this component
  ## and install them if existing. We are also looking into the NMF_COMP_SEARCH_DIRS
  ## as NMF sources location is not always clean ...
  INSTALL_NMF_COMP_SRC_HOSTEE += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
				  $(addprefix $(path)/, \
				    $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
				      $(foreach ext,$(NMF_COMP_DEF_EXTS), \
				        $(addsuffix $(ext),$(cmp))))))

  INSTALL_NMF_COMP_SRC_HOSTEE += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
				  $(addprefix $(path)/, \
				    $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
				      $(addprefix $(cmp)/, \
				        $(foreach ext,$(NMF_COMP_DEF_DIR_EXTS), \
				          $(addsuffix $(ext),$(notdir $(cmp))))))))

  ifdef INSTALL_NMF_COMP_SRC
    INSTALL_NMF_COMP_SRC_HOSTEE += $(foreach path,$(NMF_COMP_SEARCH_DIRS),\
	   			     $(addprefix $(path)/, \
				       $(foreach cmp,$(NMF_COMP_DEF_DIRS), \
					 $(foreach dir,/inc /src /api, \
				           $(addsuffix $(dir),$(cmp))))))
  endif

  ## Get only existing files
  INSTALL_NMF_COMP_SRC_HOSTEE:=$(wildcard $(INSTALL_NMF_COMP_SRC_HOSTEE))

  ## The relative path were just here to look for existing files, we need to remove them
  ## for make to use VPATH and get destination directory as we want it
  INSTALL_NMF_COMP_SRC_HOSTEE:=$(subst ../,,$(INSTALL_NMF_COMP_SRC_HOSTEE))

  ## All files/dirs we'll install as NMF HOSTEE sources
  NMF_COMP_SRC_HOSTEE_INSTALL += $(addprefix $(INSTALL_NMF_COMP_SRC_HOSTEE_DIR)/$(INSTALL_NMF_COMP_SRC_HOSTEE_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_HOSTEE))

endif

NMF_COMP_SRC_COMMON_INSTALL += $(addprefix $(INSTALL_NMF_COMP_SRC_COMMON_DIR)/$(INSTALL_NMF_COMP_SRC_COMMON_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_COMMON))

################################################################################

SELF    = $(firstword $(MAKEFILE_LIST))
MKFLAGS = $(foreach tmp, $(MAKEFLAGS), $(addsuffix \",$(addprefix \",$(shell echo $(tmp) | grep =))))

define do_it_for_variant
  @if [ "$(CORE)" = "mmdsp" ] ; then if [ ! -z $(MM_VARIANT) ] ; then if [ -z $(BUILDING_MM_VARIANT) ] ; then \
    $(MAKE) -f $(SELF) $@ $(MKFLAGS) BUILDING_MM_VARIANT=$(MM_VARIANT) ; fi ; fi ; fi
endef

################################################################################

all: $(OST_TRACE_MARKER) $(TMPDIR) $(PREBUILD_COMPONENTS) $(NMF_RULES_ALL_TARGET) $(EXTRA_ALL)
	$(do_it_for_variant)

components: $(COMPONENTS)

$(COMPONENTS):
	@echo "  NMFCONFC $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $@))"
	$(VERCHAR)$(NMFCONFC) $(NMFCONFC_FLAGS) $(NMFCONFC_FLAGS_$@) --tmpoutput=$(TMPDIR) $@

interfaces:
ifneq ($(ASYNCITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(ASYNCITFS)))"
	$(VERCHAR)$(NMFITFC) --tmpoutput=$(TMPDIR) -asynchronous $(ASYNCITFS)
endif
ifneq ($(USER2MPCITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(USER2MPCITFS)))"
	$(VERCHAR)$(NMFITFC) --tmpoutput=$(TMPDIR) -user2mpc -n $(ITF_PREFIX) $(USER2MPCITFS)
endif
ifneq ($(MPC2USERITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(MPC2USERITFS)))"
	$(VERCHAR)$(NMFITFC)  --tmpoutput=$(TMPDIR) -mpc2user -n $(ITF_PREFIX) $(MPC2USERITFS)
endif
ifneq ($(INTERMPCITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(INTERMPCITFS)))"
	$(VERCHAR)$(NMFITFC)  --tmpoutput=$(TMPDIR) -mpc2mpc -n $(ITF_PREFIX) $(INTERMPCITFS)
endif
ifneq ($(KERNEL2MPCITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(KERNEL2MPCITFS)))"
	$(VERCHAR)$(NMFITFC)  --tmpoutput=$(TMPDIR) -host2mpc -n $(ITF_PREFIX) $(KERNEL2MPCITFS)
endif
ifneq ($(MPC2KERNELITFS),)
	@echo "  NMFITFC  $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(MPC2KERNELITFS)))"
	$(VERCHAR)$(NMFITFC)  --tmpoutput=$(TMPDIR) -mpc2host -n $(ITF_PREFIX) $(MPC2KERNELITFS)
endif
ifneq ($(ASYNCITFS_CPP),)
	@echo "  NMFITFC   $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(ASYNCITFS_CPP)))"
	$(VERCHAR)$(NMFITFC) --cpp --tmpoutput=$(TMPDIR) -asynchronous $(ASYNCITFS_CPP)
endif
ifneq ($(USER2MPCITFS_CPP),)
	@echo "  NMFITFC   $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(USER2MPCITFS_CPP)))"
	$(VERCHAR)$(NMFITFC) --cpp --tmpoutput=$(TMPDIR) -user2mpc -n $(ITF_PREFIX)_cpp $(USER2MPCITFS_CPP)
endif
ifneq ($(MPC2USERITFS_CPP),)
	@echo "  NMFITFC   $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(MPC2USERITFS_CPP)))"
	$(VERCHAR)$(NMFITFC) --cpp --tmpoutput=$(TMPDIR) -mpc2user -n $(ITF_PREFIX)_cpp $(MPC2USERITFS_CPP)
endif
ifneq ($(ITFS_HEADER),)
	@echo "  NMFITFC   $(BUILDING_MM_VARIANT) $(subst $(PRODUCT_TOP_DIR),,$(abspath $(ITFS_HEADER)))"
	$(VERCHAR)$(NMFITFC) --cpp -o $(BUILD)/host -itf -n $(ITF_PREFIX) $(ITFS_HEADER)
endif

clean: uninstall
ifeq ($(STUB_TARGETS),)
	$(VERCHAR)rm -rf $(TMPDIR) $(BUILD)
endif
	$(do_it_for_variant)

realclean: clean
ifeq ($(STUB_TARGETS),)
	$(VERCHAR)rm -rf $(REALCLEAN_DIRS) 
endif
	$(do_it_for_variant)

install-all: $(EXTRA_INSTALL) $(DICO_INSTALL) $(NMF_COMP_SRC_MMDSP_INSTALL) $(NMF_COMP_SRC_HOSTEE_INSTALL) $(NMF_COMP_SRC_COMMON_INSTALL)
	@echo "  MMINSTALL $(subst $(BUILD_INTERMEDIATES_DIR)/,,$(subst $(PRODUCT_TOP_DIR),,$(abspath $(BUILD))))"
	$(VERCHAR)$(MMINSTALL) -d $(BUILD) $(REPOSITORY_DST)

install: install-all
	$(do_it_for_variant)

uninstall: $(EXTRA_UNINSTALL)
	@echo "  MMUNINSTALL $(subst $(BUILD_INTERMEDIATES_DIR)/,,$(subst $(PRODUCT_TOP_DIR),,$(abspath $(BUILD))))"
	$(MMINSTALL) -r -d $(BUILD) $(REPOSITORY_DST)
	$(do_it_for_variant)

################################################################################

$(TMPDIR):
ifeq ($(STUB_TARGETS),)
	$(VERCHAR)$(MKDIR) $@
endif

$(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/$(INSTALL_NMF_COMP_SRC_MMDSP_PREFIX)/autogentraces/%: $(OST_INC_AUTOGEN)/%
	@echo "  MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

$(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/$(INSTALL_NMF_COMP_SRC_MMDSP_PREFIX)%: %
	@echo "  MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

$(INSTALL_NMF_COMP_SRC_HOSTEE_DIR)/$(INSTALL_NMF_COMP_SRC_HOSTEE_PREFIX)%: %
	@echo "  MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

$(INSTALL_NMF_COMP_SRC_COMMON_DIR)/$(INSTALL_NMF_COMP_SRC_COMMON_PREFIX)%: %
	@echo "  MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

#---------------------------------
# to debug
#---------------------------------
debug:
	$(foreach v,$(V), $(warning $v = $($v)))	

#----------------
# Debug target
#----------------
p-%:
	@echo "$*=$($*)"

.PHONY: clean debug realclean all components interfaces $(COMPONENTS)

ifdef DUMP_VARIABLES
  $(info )
  $(info --SharedNmfRules.mk--)
  $(info MYMMSYSTEM=$(MYMMSYSTEM))
  $(info MMSYSTEM=$(MMSYSTEM))
  $(info MMDSPPROCESSOR=$(MMDSPPROCESSOR))
  $(info MAKECMDGOALS=$(MAKECMDGOALS))
  $(info CORE=$(CORE))
  $(info VPATH=$(VPATH))
  $(info MODULE=$(MODULE))
  $(info COMPONENTS=$(COMPONENTS))
  $(info INSTALL_COMPONENTS=$(INSTALL_COMPONENTS))
  $(info MMINSTALL=$(MMINSTALL))
  $(info MMINSTALL_ARGS=$(MMINSTALL_ARGS))
  $(info )
  $(info CPPFLAGS=$(CPPFLAGS))
  $(info MM_VARIANT_CPPFLAGS$(BUILDING_MM_VARIANT)=$(MM_VARIANT_CPPFLAGS$(BUILDING_MM_VARIANT)))
  $(info SRCPATH=$(SRCPATH))
  $(info INSTALL_NMF_REPO_DIR=$(INSTALL_NMF_REPO_DIR))
  $(info NMFCONFC=$(NMFCONFC))
  $(info NMFCONFC_FLAGS=$(NMNMFCONFC_FLAGSFCONFC))
  $(info BUILD=$(BUILD))
  $(info TMPDIR=$(TMPDIR))
  $(info )
  $(info INSTALL_NMF_COMP_SRC_MMDSP=$(INSTALL_NMF_COMP_SRC_MMDSP))
  $(info NMF_COMP_SRC_MMDSP_INSTALL=$(NMF_COMP_SRC_MMDSP_INSTALL))
  $(info INSTALL_NMF_COMP_SRC_HOSTEE=$(INSTALL_NMF_COMP_SRC_HOSTEE))
  $(info NMF_COMP_SRC_HOSTEE_INSTALL=$(NMF_COMP_SRC_HOSTEE_INSTALL))
  $(info INSTALL_NMF_COMP_SRC_COMMON=$(INSTALL_NMF_COMP_SRC_COMMON))
  $(info NMF_COMP_SRC_COMMON_INSTALL=$(NMF_COMP_SRC_COMMON_INSTALL))
  $(info )
  $(info EXTRA_UNINSTALL=$(EXTRA_UNINSTALL))
  $(info BUILD_INTERMEDIATES_DIR=$(BUILD_INTERMEDIATES_DIR))
  $(info --SharedNmfRules.mk--)
  $(info )
endif
