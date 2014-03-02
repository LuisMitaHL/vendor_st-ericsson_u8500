#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# Default to best guess value if not set
ifndef OST_COMPONENT_INC
  ifndef COMPONENT_TOP_DIR
    $(error COMPONENT_TOP_DIR not defined)
  endif
  OST_COMPONENT_INC=$(COMPONENT_TOP_DIR)/inc
endif

## Default to generate header files and dictionaries in source tree
OST_COMPONENT_INC:=$(abspath $(OST_COMPONENT_INC))
OST_COMPONENT_INC_OUT:=$(subst $(MMROOT),$(TARGET_OUT_INTERMEDIATES),$(OST_COMPONENT_INC))

## FIXME: when building packed components Trace compiler generated
## files are to be taken from source tree depending on the cases
ifeq ($(MM_BUILD_FOR_PACKAGING),false)
  ## Default build: generate everything in out tree 
  OST_COMPONENT_INC:=$(OST_COMPONENT_INC_OUT)
endif

OST_INC_AUTOGEN:=$(OST_COMPONENT_INC)/autogentraces

VPATH+=$(OST_COMPONENT_INC)
## Always change CPPFLAGS even if compiler invocation is not expected as some
## make files are making use of TC generated header file while not invoking it
CPPFLAGS:=-I$(OST_INC_AUTOGEN) $(CPPFLAGS)

ifeq ($(OST_COMPILER),1)          # Set by Makefile making use of trace compiler
ifneq ($(MMTRACE_COMPILER),false) # Flag to disable trace compiler

TC_GROUP_MAP = $(MM_HEADER_BASEDIR)/shared/ste_ost_group_map.h

OST_SRC_LIST = $(shell find $(OST_SRC_DIR) -name "*.c" -printf "`pwd`/%p\n" 2>/dev/null | sort)
OST_SRC_LIST += $(shell find $(OST_SRC_DIR) -name "*.cpp" -printf "`pwd`/%p\n" 2>/dev/null | sort)

USER_OST_COMPILER_OPTION += $(MY_OST_COMPILER_OPTION)
USER_OST_COMPILER_OPTION += $(EXTRA_OST_COMPILER_OPTION)

ifdef OST_TC_BYPASS
  OST_DICTIONARIES:=$(strip $(addsuffix _top,$(subst ::, ,$(OST_TOP_DICO_LIST))))
  ## Marker will be first top generated dictionay always in the intermediates folder
  OST_DICO_MARKER:=$(OST_COMPONENT_INC_OUT)/$(word 1,$(OST_DICTIONARIES)).tcdone
  OST_DICTIONARIES:=$(addprefix $(OST_INC_AUTOGEN)/,$(addsuffix .xml,$(OST_DICTIONARIES)))
else
  ifndef OST_DICO_NAME
    $(error OST_DICO_NAME not defined)
  endif
  #OST_DICO_NAME:=$(if $(OST_DICO_NAME),$(OST_DICO_NAME),$(OST_COMPONENT_NAME))
  OST_DICTIONARIES:=$(OST_INC_AUTOGEN)/$(OST_DICO_NAME).xml
  OST_DICO_MARKER:=$(OST_COMPONENT_INC_OUT)/$(OST_DICO_NAME).tcdone
endif

OST_TRACE_MARKER:=$(OST_INC_AUTOGEN)/OstTraceDefinitions_ste.h $(OST_DICTIONARIES) $(OST_DICO_MARKER)

## Force header files generation on first run
USER_OST_COMPILER_OPTION += $(if $(wildcard $(OST_DICO_MARKER)),,--defines=forceheader)

# ensure Trace compiler runs before NMF compiler
all: $(OST_TRACE_MARKER)

$(OST_INC_AUTOGEN)/OstTraceDefinitions_ste.h:
	$(VERCHAR)mkdir -p $(OST_INC_AUTOGEN)
	@echo "  TC        $(REL_BUILD_INTERMEDIATES_DIR) GenTraceDefinition"
	$(VERCHAR)perl $(TRACECOMPILERDIR)/tracecompiler.pl \
                    --defines=mediaroot=$(MEDIUM_DIR) \
                    --inc=$(OST_INC_AUTOGEN) \
                    --defines=grpmapfile=$(TC_GROUP_MAP) \
                    $(USER_OST_COMPILER_OPTION) \
                    --action="GenTraceDefinition"

$(OST_DICTIONARIES) $(OST_DICO_MARKER): $(OST_SRC_LIST)
	$(VERCHAR)mkdir -p $(OST_INC_AUTOGEN) $(OST_COMPONENT_INC_OUT)
ifndef OST_TC_BYPASS
	@echo "  TC        $(REL_BUILD_INTERMEDIATES_DIR) $(OST_DICO_NAME)"
	$(VERCHAR)perl $(TRACECOMPILERDIR)/tracecompiler.pl \
                    --defines=grpmapfile=$(TC_GROUP_MAP) \
                    --defines=mediaroot=$(MEDIUM_DIR) \
                    --id=$(OST_COMPONENT_ID) \
                    --name=$(OST_DICO_NAME) \
                    --inc=$(OST_INC_AUTOGEN) \
                    --defines=strippath \
                    $(USER_OST_COMPILER_OPTION) \
                    $(OST_SRC_LIST)
endif
ifdef OST_TOP_DICO_LIST
ifneq ($(MYMMSYSTEM),symbian)
	@echo "  TC        $(REL_BUILD_INTERMEDIATES_DIR) GenTopDictionary"
	$(VERCHAR)perl $(TRACECOMPILERDIR)/tracecompiler.pl \
                    --defines=grpmapfile=$(TC_GROUP_MAP) \
                    --defines=mediaroot=$(MEDIUM_DIR) \
                    --inc=$(OST_INC_AUTOGEN) \
                    $(USER_OST_COMPILER_OPTION) \
                    $(foreach file, $(OST_TOP_DICO_LIST), $(addprefix --name=, $(file))) \
                    --action="GenTopDictionary"
endif #ifneq ($(MYMMSYSTEM),symbian)
endif #ifdef OST_TOP_DICO_LIST
        ## In case dico cannot be generated because source files are missing generate empty one
	$(VERCHAR)touch $@

clean realclean: ostclean

ifneq ($(findstring x86,$(MYMMBOARD)),x86)
  DICO_INSTALL += install_dico
endif

EXTRA_UNINSTALL += ostuninstall

install_dico: $(addprefix $(INSTALL_DICO_DIR)/,$(INSTALL_DICO))

OST_CLEAN := $(OST_INC_AUTOGEN) $(OST_DICO_MARKER)

ostclean:
	@echo "  REALCLEAN OST"
	$(VERCHAR)rm -rf $(OST_CLEAN) ; true

ostuninstall:
	$(VERCHAR)$(RM) $(wildcard $(addprefix $(INSTALL_DICO_DIR)/,$(INSTALL_DICO)))

$(INSTALL_DICO_DIR)/%.xml: $(OST_INC_AUTOGEN)/%.xml
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR)/$*.xml"
	$(VERCHAR)$(MMINSTALL) -i -f $< $@
	$(VERCHAR)sed -i -r "s,(<path val=\")(.*)/multimedia/(.*),\1$(MMROOT)/\3,g" $@

.PHONY: ostclean install_dico

.DELETE_ON_ERROR:

ifdef DUMP_VARIABLES
  $(info )
  $(info --SharedTraceCompiler.mk--)
  $(info OST_INC_AUTOGEN='$(OST_INC_AUTOGEN)')
  $(info OST_SRC_DIR='$(OST_SRC_DIR)')
  $(info OST_SRC_LIST='$(OST_SRC_LIST)')
  $(info OST_COMPONENT_INC='$(OST_COMPONENT_INC)')
  $(info OST_TRACE_MARKER='$(OST_TRACE_MARKER)')
  $(info OST_DICTIONARIES='$(OST_DICTIONARIES)')
  $(info OST_DICO_MARKER='$(OST_DICO_MARKER)')
  $(info )
endif

endif #ifeq ($(OST_COMPILER),1)
endif #ifneq ($(MMTRACE_COMPILER),false)
