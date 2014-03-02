#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# OST trace compiler
include $(MM_MAKEFILES_DIR)/SharedTraceCompiler.mk

ARCH_HAS_SOLIB=true

ifeq ($(CORE),mmdsp)
  ARCH_HAS_SOLIB=false
  ifeq ($(BUILD_MMDSP_PROC),bin)
    STUB_BUILD_TARGETS:=null
  endif
else
  ifeq ($(USE_OPT_COMPILER),true)
    ifeq ($(BUILD_ARM_PROC),bin)
      STUB_BUILD_TARGETS:=install
    endif
  else
    ifeq ($(BUILD_ARM_CODE),bin)
      STUB_BUILD_TARGETS:=install
    else
      ifeq ($(BUILD_ARM_CODE),)
        STUB_BUILD_TARGETS:=null
      endif
    endif
  endif
endif

################################################################################

ifdef TARGET_OUT_INTERMEDIATES
  ## In order to support generating all build files in an alternate directory
  ## the BUILD_INCLUDES variables used only by some imaging components
  ## is hacked to add the path where expected content has been generated
  BUILD_INCLUDES += -I $(TARGET_OUT_INTERMEDIATES)/imaging/wrapper_openmax/nmf/host/$(NMF_TMP_DIR)/$(MMPLATFORM)/openmax_processor_wrp/src
endif

################################################################################

ifeq ($(USE_OPT_COMPILER),true)
  CC:=$(OPT_CC)
  CXX:=$(OPT_CXX)
  ASM:=$(OPT_ASM)
  OPTIMIZ_CFLAGS:=$(OPT_OPTIMIZ_CFLAGS)
else
  SYSTEM_LDFLAGS:=$(FIXED_LDFLAGS)
endif

ifeq ($(CFLAGS),)
  CFLAGS := -g $(OPTIMIZ_CFLAGS)
endif

ifeq ($(MMCOMPILER),flexcc)

  CFLAGS+=$(ACFLAGS)

  ifeq ($(BINUTILS),1)
  endif

  ifdef STACK
    STACKPARAMS+= stack=$(STACK)
  endif

  ifdef SSTACK
    STACKPARAMS+= sstack=$(SSTACK)
  endif

  ifneq ($(MODE16),0)
    EXELDFLAGS+=-mode16
    CFLAGS+= -mode16
  endif

  EXELDFLAGS+= $(STACKPARAMS)
  CFLAGS    += $(STACKPARAMS)

  export MCPROJECT
endif

ifeq ($(MYMMBOARD),x86_mmdsp)
  ifeq ($(MODE16),0)
    EXTRA_LIBS+=$(BITTRUE_LIB24)
  else # MODE16
    EXTRA_LIBS+=$(BITTRUE_LIB16)
  endif # MODE16
endif
ifeq ($(MYMMBOARD),x86_cortexA9)
    EXTRA_LIBS+=$(BITTRUE_LIB)
endif

################################################################################
# Generic Rules definition
################################################################################

ALL_OBJ:=

PREREQUISITES = $(DEP_DEP) $(OST_TRACE_MARKER) $(HEADER_INSTALL)

.PHONY: all install install_lib install_solib install_bin lib mylib solib \
        mysolib exe myexe uninstall clean realclean header todo \
        mytodo trace_compiler debug

.SUFFIXES:

#--------------------------
# Directory Rules
#--------------------------

ALL_DIR = $(OBJ_DIR) $(DEP_DIR) $(BIN_DIR) $(LIB_DIR)

$(ALL_DIR):
	$(VERCHAR)$(MKDIR) $@

#-------------------------------------
# Header
#-------------------------------------
FOUND_HEADER  = $(notdir $(wildcard $(addsuffix /*.h*,$(INSTALL_HEADERDIR))))
FOUND_HEADER += $(notdir $(wildcard $(addsuffix /*.inl,$(INSTALL_HEADERDIR))))

HEADER_INSTALL    = $(addprefix $(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR), $(INSTALL_HEADER) $(FOUND_HEADER))
HEADER_INSTALL   += $(PATCHED_HEADERS)

#-------------------------------------
# NMF components sources
#-------------------------------------
NMF_COMP_SRC_MMDSP_INSTALL  += $(addprefix $(INSTALL_NMF_COMP_SRC_MMDSP_DIR)/$(INSTALL_NMF_COMP_SRC_MMDSP_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_MMDSP))

NMF_COMP_SRC_HOSTEE_INSTALL += $(addprefix $(INSTALL_NMF_COMP_SRC_HOSTEE_DIR)/$(INSTALL_NMF_COMP_SRC_HOSTEE_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_HOSTEE))

NMF_COMP_SRC_COMMON_INSTALL += $(addprefix $(INSTALL_NMF_COMP_SRC_COMMON_DIR)/$(INSTALL_NMF_COMP_SRC_COMMON_PREFIX), \
                                $(INSTALL_NMF_COMP_SRC_COMMON))

################################################################################

ifeq ($(VERBOSE),2)
  $(foreach f,$(wordlist 2,$(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)),$(info ==> Include $f))
  $(info === STUB_BUILD_TARGETS=$(STUB_BUILD_TARGETS))
endif

#-------------------------------------
# All target
#-------------------------------------
ifeq ($(STUB_BUILD_TARGETS),)
## Normal rule
all: $(ALL_DIR) mytodo $(EXTRA_ALL) $(DICO_INSTALL)
endif
ifeq ($(STUB_BUILD_TARGETS),install)
## Just perform install (precompiled ARM libs)
all: install
endif
ifeq ($(STUB_BUILD_TARGETS),null)
## Stub build
all: header
endif

SELF    = $(firstword $(MAKEFILE_LIST))
MKFLAGS = $(foreach tmp, $(MAKEFLAGS), $(addsuffix \",$(addprefix \",$(shell echo $(tmp) | grep =))))

mytodo:
	$(VERYVERCHAR)$(MAKE) $(MAKE_OPTIONS) -f $(SELF) -r todo $(MKFLAGS)

################################################################################

#---------------------------------
# Static Library rules
#---------------------------------

## Specific LIBNAME case
LIBRARY      = $(addprefix $(LIB_DIR)/$(LIB_DIR_NAME), $(addsuffix .$(LIB_EXT), $(LIBNAME)))
LIBLIST     += $(LIBRARY)
STATIC_OBJ   = $(addprefix $(OBJ_DIR)/, $(addsuffix .$(OBJ_EXT), $(LIBOBJ)))
ALL_OBJ     += $(LIBOBJ)

$(LIBRARY): $(STATIC_OBJ)
	@echo "  $(AR_NAME)     $(REL_BUILD_INTERMEDIATES_DIR) $(notdir $@)"
	$(VERCHAR)$(AR) $(AR_OPTION) $@ $+

##### Makeing use of eval to build alternate libnames ....
## LIBNAME_name, LIBOBJ_name,

define LIBNAME_template
LIBRARY_$(1)    = $$(addprefix $$(LIB_DIR)/$$(LIB_DIR_NAME), $$(addsuffix .$$(LIB_EXT), $$(LIBNAME_$(1))))
LIBLIST        += $$(LIBRARY_$(1))
STATIC_OBJ_$(1) = $$(addprefix $$(OBJ_DIR)/, $$(addsuffix .$$(OBJ_EXT), $$(LIBOBJ_$(1))))
ALL_OBJ        += $$(LIBOBJ_$(1))

$$(LIBRARY_$(1)): $$(STATIC_OBJ_$(1))
	@echo "  $$(AR_NAME)      $$(REL_BUILD_INTERMEDIATES_DIR) $$(notdir $$@)"
	$$(VERCHAR)$$(AR) $$(AR_OPTION) $$@ $$+
endef

ifdef LIBNAME_2
  LIBNAMES+= 2
endif
ifdef LIBNAME_3
  LIBNAMES+= 3
endif
ifdef LIBNAME_4
  LIBNAMES+= 4
endif

$(foreach libname,$(LIBNAMES),$(eval $(call LIBNAME_template,$(libname))))

ifeq ($(STUB_BUILD_TARGETS),)
lib: $(LIB_DIR) $(OBJ_DIR) $(DEP_DIR) $(LIB_INC_DIR) $(PREREQUISITES)
	$(VERYVERCHAR)$(MAKE) $(MAKE_OPTIONS) -f $(SELF) -r $(MKFLAGS) mylib install_lib

mylib: $(LIBLIST)
else
lib mylib:
endif

################################################################################

#---------------------------------
# Dynamic Library rules
#---------------------------------

LDSOFLAGS += $(addprefix -L,$(wildcard $(LIBS_PATH)))

##### Making use of eval to build alternate libnames ....
## define the following variables to build a shared lib:
##   OBJS_soname, LIBS_soname, FLAGS_soname

define SOLIBNAME_template
SOLIBRARY_$(1)   += $$(addprefix $(LIB_DIR)/$(LIB_DIR_NAME), $$(addsuffix .so, $(1)))
SOLIBS_$(1)      += $$(foreach lib,$$(LIBS_$(1)), \
                       $$(firstword $$(wildcard $$(foreach path, $$(LIBS_PATH),\
                          $$(addprefix $$(path)/$(LIB_DIR_NAME),$$(addsuffix .$$(LIB_EXT),$$(lib)))))))
SOEXTRALIBS_$(1) += $$(foreach lib,$$(EXTRALIBS_$(1)), \
                       $$(firstword $$(wildcard $$(foreach path, $$(LIBS_PATH),\
                         $$(addprefix $$(path)/$(LIB_DIR_NAME),$$(addsuffix .$$(LIB_EXT),$$(lib)))))))
SOLIB_OBJ_$(1)    = $$(addprefix $$(OBJ_DIR)/, $$(addsuffix .$$(OBJ_EXT), $$(LIBOBJ_$(1))))
SOLIBLIST        += $$(SOLIBRARY_$(1))

ifeq ($(findstring mysolib,$(MAKECMDGOALS)),mysolib)
  ifeq ($$(CHECK_EXTRALIBS_$(1)),true)
    REQ_EXTRALIBS_$(1) = $$(notdir $$(foreach lib,$$(EXTRALIBS_$(1)), \
                            $$(addprefix $(LIB_DIR_NAME),$$(addsuffix .$$(LIB_EXT),$$(lib)))))
    FND_EXTRALIBS_$(1) = $$(notdir $$(SOEXTRALIBS_$(1)))
    NOT_EXTRALIBS_$(1) = $$(filter-out $$(FND_EXTRALIBS_$(1)),$$(REQ_EXTRALIBS_$(1)))
    ifdef VERBOSE
      $$(info REQ_EXTRALIBS_$(1)=$$(REQ_EXTRALIBS_$(1)))
      $$(info FND_EXTRALIBS_$(1)=$$(FND_EXTRALIBS_$(1)))
    endif
    ifneq ($$(NOT_EXTRALIBS_$(1)),)
      $$(error $$(NOT_EXTRALIBS_$(1)) not found)
    endif
    REQ_LIBS_$(1) = $$(notdir $$(foreach lib,$$(LIBS_$(1)),$$(addprefix $(LIB_DIR_NAME),$$(addsuffix .$$(LIB_EXT),$$(lib)))))
    FND_LIBS_$(1) = $$(notdir $$(SOLIBS_$(1)))
    NOT_LIBS_$(1) = $$(filter-out $$(FND_LIBS_$(1)),$$(REQ_LIBS_$(1)))
    ifdef VERBOSE
      $$(info REQ_LIBS_$(1)=$$(REQ_LIBS_$(1)))
      $$(info FND_LIBS_$(1)=$$(FND_LIBS_$(1)))
    endif
    ifneq ($$(NOT_LIBS_$(1)),)
      $$(error $$(NOT_LIBS_$(1)) not found)
    endif
  endif
endif

ALL_OBJ += $$(LIBOBJ_$(1))

$$(SOLIBRARY_$(1)): $$(SOLIB_OBJ_$(1)) $$(SOEXTRALIBS_$(1)) $$(SOLIBS_$(1))
	@echo "  $(COMPILER_NAME) LD $$(REL_BUILD_INTERMEDIATES_DIR)/$$(notdir $$@)"
	$$(VERCHAR)$$(LD_SO) $$(FIXED_LDSOFLAGS) -o $$@ $$(SOLIB_OBJ_$(1)) \
	$(PRE_SOLIBS) $$(SOEXTRALIBS_$(1)) $(POST_SOLIBS) \
        $(PRE_LIBS) $$(SOLIBS_$(1)) $(POST_LIBS) \
        $$(FLAGS_$(1)) $$(LDSOFLAGS) $(SYSTEM_LDFLAGS) $$(TARGET_LIBGCC)
endef

$(foreach solibname,$(SOLIBNAMES),$(eval $(call SOLIBNAME_template,$(solibname))))

ifeq ($(STUB_BUILD_TARGETS),null)
  ## Reuse ARCH_HAS_SOLIB variable to stub so build in case not requested
  ARCH_HAS_SOLIB=false
endif

ifeq ($(ARCH_HAS_SOLIB),true)

solib: $(LIB_DIR) $(OBJ_DIR) $(DEP_DIR) $(LIB_INC_DIR) $(PREREQUISITES)
	$(VERYVERCHAR)$(MAKE) $(MAKE_OPTIONS) -f $(SELF) -r mysolib install_solib $(MKFLAGS)

mysolib: $(SOLIBLIST)

else

solib mysolib:

endif

################################################################################

#---------------------------------
# Executable Rules
#---------------------------------

ifdef MEMORYCFG
  ifeq ($(MMCOMPILER),rvct)
    EXELDFLAGS+=--scatter=$(MEMORYCFG)
  endif
  ifeq ($(MMCOMPILER),flexcc)
    EXELDFLAGS+=memcfg=$(MEMORYCFG)
  endif
endif

EXELDFLAGS+=$(LDFLAGS) $(FIXED_LDEXEFLAGS)

ifeq ($(COMPILER_FLAVOR),gcc)
  EXELDFLAGS+=$(addprefix -L,$(wildcard $(LIBS_PATH)))
endif

## Specific EXE case not using _name rule
EXE       = $(addprefix $(BIN_DIR)/, $(addsuffix $(EXE_EXT),$(EXENAME)))
EXELIST   = $(EXE)
EXE_OBJ   = $(addprefix $(OBJ_DIR)/, $(addsuffix .$(OBJ_EXT),$(EXEOBJ)))
MYLINKLIBS=$(foreach lib, $(LIBS), $(firstword $(foreach tmp, $(LIBS_PATH), $(wildcard $(tmp)/$(lib)) $(wildcard $(lib)))))
MYEXTRA_GROUP_LIBS=$(foreach lib, $(EXTRA_GROUP_LIBS), $(firstword $(foreach tmp, $(LIBS_PATH), $(wildcard $(tmp)/$(lib)) $(wildcard $(lib)))))
ALL_OBJ += $(EXEOBJ)

ifeq ($(findstring myexe,$(MAKECMDGOALS)),myexe)
  ifneq ($(words $(MYLINKLIBS)),$(words $(LIBS)))
    ifdef VERBOSE
      $(warning LIBS       = $(LIBS))
      $(warning MYLINKLIBS = $(MYLINKLIBS))
    endif
    $(error error $(filter-out $(notdir $(MYLINKLIBS)), $(notdir $(LIBS))) not found )
  endif
endif

$(EXE): $(EXE_OBJ) $(MYLINKLIBS)
	@echo "  $(COMPILER_NAME) LD $(REL_BUILD_INTERMEDIATES_DIR)/$(notdir $@)"
	$(VERCHAR)$(LD_EXE) $(EXELDFLAGS) -o $@ $(TARGET_CRTBEGIN_O) $(EXE_OBJ) $(PRE_LIBS) \
                    $(addprefix $(LD_PRE),$(MYLINKLIBS)) $(EXE_LDFLAGS) \
                    $(EXTRA_LIBS) $(POST_LIBS) $(MYEXTRA_GROUP_LIBS) $(SYSTEM_LDFLAGS) $(TARGET_CRTEND_O)
ifdef POST_BUILD
	$(VERCHAR)$(POST_BUILD)
endif

##### Makeing use of eval to build alternate exe ....
define EXENAME_template
  EXE_$(1)        = $$(addprefix $$(BIN_DIR)/, $$(addsuffix $$(EXE_EXT),$$(EXENAME_$(1))))
  EXELIST        += $$(EXE_$(1))
  EXE_OBJ_$(1)    = $$(addprefix $$(OBJ_DIR)/, $$(addsuffix .$$(OBJ_EXT),$$(EXEOBJ_$(1))))
  MYLINKLIBS_$(1) = $$(foreach lib, $$(LIBS_$(1)), $$(firstword $$(foreach tmp, $$(LIBS_PATH), $$(wildcard $$(tmp)/$$(lib)) $$(wildcard $$(lib)))))
  MYEXTRA_GROUP_LIBS_$(1) = $$(foreach lib, $$(EXTRA_GROUP_LIBS_$(1)), $$(firstword $$(foreach tmp, $$(LIBS_PATH), $$(wildcard $$(tmp)/$$(lib)) $$(wildcard $$(lib)))))

  ifeq ($$(findstring myexe,$$(MAKECMDGOALS)),myexe)
    ifneq ($$(words $$(MYLINKLIBS_$(1))),$$(words $$(LIBS_$(1))))
      ifdef VERBOSE
        $$(warning LIBS_$(1)       = $$(LIBS_$(1)))
        $$(warning MYLINKLIBS_$(1) = $$(MYLINKLIBS_$(1)))
      endif
      $$(error error $$(filter-out $$(notdir $$(MYLINKLIBS_$(1))), $$(notdir $$(LIBS_$(1)))) not found )
    endif
  endif

  ALL_OBJ += $$(EXEOBJ_$(1))

  $$(EXE_$(1)): $$(EXE_OBJ_$(1)) $$(MYLINKLIBS_$(1))
	@echo "  $(COMPILER_NAME) LD $$(REL_BUILD_INTERMEDIATES_DIR)/$$(notdir $$@)"
	$$(VERCHAR)$$(LD_EXE) $$(EXELDFLAGS) -o $$@ $$(TARGET_CRTBEGIN_O) $$(EXE_OBJ_$(1)) $$(PRE_LIBS) \
                     $$(addprefix $$(LD_PRE),$$(MYLINKLIBS_$(1))) $$(EXE_LDFLAGS_$(1)) \
                     $$(EXTRA_LIBS) $$(POST_LIBS) $$(MYEXTRA_GROUP_LIBS_$(1)) $(SYSTEM_LDFLAGS) $$(TARGET_CRTEND_O)
  ifdef POST_BUILD
	$$(VERCHAR)$$(POST_BUILD_$(1))
  endif

endef

ifdef EXENAME_2
  EXENAMES += 2
  EXE_LDFLAGS_2 = $(EXE2_LDFLAGS)
endif
ifdef EXENAME_3
  EXENAMES += 3
  EXE_LDFLAGS_3 = $(EXE3_LDFLAGS)
endif
ifdef EXENAME_4
  EXENAMES += 4
  EXE_LDFLAGS_4 = $(EXE4_LDFLAGS)
endif

$(foreach exename,$(EXENAMES),$(eval $(call EXENAME_template,$(exename))))

ifeq ($(STUB_BUILD_TARGETS),)
exe: $(BIN_DIR) $(OBJ_DIR) $(DEP_DIR) $(PREREQUISITES)
	$(VERYVERCHAR)$(MAKE) $(MAKE_OPTIONS) -f $(SELF) -r myexe install_bin $(MKFLAGS)

myexe : $(EXELIST)
else
exe myexe:
endif

################################################################################

#---------------------------------
# Generic Rules
#---------------------------------

# Static objects
$(OBJ_DIR)/%.$(OBJ_EXT) : %.c
	@echo "  $(COMPILER_NAME) C      $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MKDIR) $(dir $@) $(dir $(DEP_DIR)/$*.d)
ifneq ($(MMCOMPILER),flexcc)
	$(VERCHAR)$(CC) $(CPPFLAGS) $(CFLAGS) $(NOLINK) -D__MYFILE__=\"`echo $(abspath $<) | sed -r "s,(.*)/multimedia/(.*),\2,g"`\" $(abspath $<) -o $@ $(DEPS_FLAGS)$(DEP_DIR)/$*.d
else
 ## MMDSP compiler is buggy right now and does not support dep file generation while compiling object - do it in 2 phases
	$(VERCHAR)mcpp $(CPPFLAGS) $(abspath $<) -MM -MF $(DEP_DIR)/$*.d -MT $@
	$(VERCHAR)$(CC) $(CPPFLAGS) $(CFLAGS) $(NOLINK) -D__MYFILE__=\"`echo $(abspath $<) | sed -r "s,(.*)/multimedia/(.*),\2,g"`\" $(abspath $<) -o $@
endif

$(OBJ_DIR)/%.$(OBJ_EXT) : %.cpp
	@echo "  $(COMPILER_NAME) C++    $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MKDIR) $(dir $@) $(dir $(DEP_DIR)/$*.d)
	$(VERCHAR)$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(NOLINK) -D__MYFILE__=\"`echo $(abspath $<) | sed -r "s,(.*)/multimedia/(.*),\2,g"`\" $(abspath $<) -o $@ $(DEPS_FLAGS)$(DEP_DIR)/$*.d

$(OBJ_DIR)/%.$(OBJ_EXT) : %.s
	@echo "  $(COMPILER_NAME) ASM    $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MKDIR) $(dir $@)
	$(VERCHAR)$(ASM) $(ASMFLAGS) $(abspath $<) -o $@

#---------------------------------
# Dependency files management
#---------------------------------

DEPEND_FILES = $(addprefix $(DEP_DIR)/, $(addsuffix .d,$(sort $(ALL_OBJ))))

EXISITING_DEPEND_FILES = $(wildcard $(DEPEND_FILES))

-include $(EXISITING_DEPEND_FILES)

#---------------------------------
# Install rules
#---------------------------------

ifeq ($(STUB_BUILD_TARGETS),null)
## build stub so also stubbing install -> installing headers only
install: header $(EXTRA_INSTALL) $(DICO_INSTALL)
else
install: header $(EXTRA_INSTALL) $(DICO_INSTALL) install_lib install_solib install_bin
endif

header: $(HEADER_INSTALL)
nmfsrc: $(NMF_COMP_SRC_MMDSP_INSTALL) $(NMF_COMP_SRC_HOSTEE_INSTALL)  $(NMF_COMP_SRC_COMMON_INSTALL)

install_lib: $(addprefix $(INSTALL_LIB_DIR)/,$(INSTALL_LIB))
ifeq ($(ARCH_HAS_SOLIB),true)
install_solib: $(addprefix $(INSTALL_SOLIB_DIR)/$(INSTALL_SOLIB_PREFIX_DIR)/$(LIB_DIR_NAME),$(addsuffix .so,$(INSTALL_SOLIB)))
else
install_solib:
endif
install_bin: $(addprefix $(INSTALL_BIN_DIR)/,$(addsuffix $(EXE_EXT),$(INSTALL_BIN)))

$(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR)%: %
	@echo "  MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

$(INSTALL_LIB_DIR)/%.$(LIB_EXT):
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR) $(notdir $*.$(LIB_EXT))"
	$(VERCHAR)$(MMINSTALL) $(LIB_DIR)/$*.$(LIB_EXT) $@

$(INSTALL_SOLIB_DIR)/$(INSTALL_SOLIB_PREFIX_DIR)/%.so:
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR) $(notdir $*.so)"
	$(VERCHAR)$(MMINSTALL) $(LIB_DIR)/$*.so $@

$(INSTALL_BIN_DIR)/%$(EXE_EXT):
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR) $(notdir $*$(EXE_EXT))"
	$(VERCHAR)$(MMINSTALL) $(BIN_DIR)/$*$(EXE_EXT) $@

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
# uninstall rules
#---------------------------------
uninstall: $(EXTRA_UNINSTALL)
	$(VERCHAR)$(RM) $(HEADER_INSTALL) $(addprefix $(INSTALL_LIB_DIR)/,$(INSTALL_LIB)) $(addprefix $(INSTALL_SOLIB_DIR)/$(INSTALL_SOLIB_PREFIX_DIR)/,$(INSTALL_SOLIB)) $(addprefix $(INSTALL_BIN_DIR)/,$(INSTALL_BIN))

#---------------------------------
# to debug
#---------------------------------
debug:
$(foreach v,$(V), $(warning $v = $($v)))

#----------------
# Clean target
#----------------
ifeq ($(STUB_BUILD_TARGETS),)
## Normal cleaning rules
clean: uninstall
	@echo "  CLEAN     $(REL_BUILD_INTERMEDIATES_DIR)"
ifneq ($(OBJ_DIR),)
	$(VERCHAR)$(RM) $(OBJ_DIR)
endif
ifneq ($(BIN_DIR),)
	$(VERCHAR)$(RM) $(BIN_DIR)
endif
ifneq ($(DEP_DIR),)
	$(VERCHAR)$(RM) $(DEP_DIR)
endif
ifneq ($(LIB_DIR),)
	$(VERCHAR)$(RM) $(LIB_DIR)
endif

realclean: clean
	@echo "  REALCLEAN $(REL_BUILD_INTERMEDIATES_DIR)"
	$(VERCHAR)$(RM) $(TOBECLEAN) $(HEADER_INSTALL) $(OBJ_DIR) $(BIN_DIR) $(DEP_DIR) $(LIB_DIR)

endif #ifeq ($(STUB_BUILD_TARGETS),)

ifeq ($(STUB_BUILD_TARGETS),install)
clean realclean: uninstall
endif
ifeq ($(STUB_BUILD_TARGETS),null)
clean realclean:
endif

#----------------
# Debug target
#----------------
p-%:
	@echo "$*=$($*)"

ifdef DUMP_VARIABLES
  $(info )
  $(info -------------------------------------- SharedRules.mk --------------------------------------)
  $(info MYMMSYSTEM    =$(MYMMSYSTEM))
  $(info MMSYSTEM      =$(MMSYSTEM))
  $(info MMDSPPROCESSOR=$(MMDSPPROCESSOR))
  $(info CORE=$(CORE))
  $(info USE_OPT_COMPILER=$(USE_OPT_COMPILER))
  $(info BUILD_MMDSP_PROC=$(BUILD_MMDSP_PROC))
  $(info BUILD_ARM_PROC=$(BUILD_ARM_PROC))
  $(info STUB_BUILD_TARGETS=$(STUB_BUILD_TARGETS))
  $(info )
  $(info LIB_DIR=$(LIB_DIR))
  $(info OBJ_DIR=$(OBJ_DIR))
  $(info DEP_DIR=$(DEP_DIR))
  $(info ALL_DIR=$(ALL_DIR))
  $(info )
  $(info LIB_INC_DIR=$(LIB_INC_DIR))
  $(info LIBS_PATH=$(LIBS_PATH))
  $(info HEADER_INSTALL=$(HEADER_INSTALL))
  $(info )
  $(info VPATH=$(VPATH))
  $(info CFLAGS=$(CFLAGS))
  $(info CPPFLAGS=$(CPPFLAGS))
  $(info )
  $(info LIBLIST=$(LIBLIST))
  $(info SOLIBLIST=$(SOLIBLIST))
  $(info EXELIST=$(EXELIST))
  $(info )
  $(info BASE_REPOSITORY=$(BASE_REPOSITORY))
  $(info HOST_NMF_SEARCH_DIRS=$(HOST_NMF_SEARCH_DIRS))
  $(info )
  $(info DEPEND_FILES=$(DEPEND_FILES))
  $(info EXISITING_DEPEND_FILES=$(EXISITING_DEPEND_FILES))
  $(info -------------------------------------- SharedRules.mk --------------------------------------)
  $(info )
endif

