# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

# Add dedicated flags for algo
ALGO_FLAGS += -DRER_FOURCC_RGB888

CPPFLAGS+=$(ALGO_FLAGS)
export ALGO_FLAGS


#Special flags for trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarwomDemoCpt -DCONFIG_DEBUG_AUTOVAR

DBG_SUF=_debug
ALGO_LIB_ROOT  =ste_womDemoCpt_algo
ifdef DEBUG
	DBG_SUF     =_dbg
	CPPFLAGS   += -D_DEBUG
	SOLIBNAME   = ste_omxwomDemoCpt$(DBG_SUF)
	CFLAGS      =-g -O0
	OBJ_EXT     =odebug
	ALGO_LIB    =$(ALGO_LIB_ROOT)$(DBG_SUF)
else
	DBG_SUF     =
	CPPFLAGS   += -DRELEASE 
	SOLIBNAME   = ste_omxwomDemoCpt
	ALGO_LIB    =$(ALGO_LIB_ROOT)
endif

SOLIBOBj = 

INSTALL_HEADER = OMX_extensions/womDemoCpt_extensions.h
INSTALL_SOLIB  = $(SOLIBNAMES)

#WARNING todo target is MANDATORY (exe and/or lib)
todo: solib

# We are generating an OMX component .so - request to be installed in correct place
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk
# redefine output dir for imaging components
INSTALL_SOLIB_DIR     :=$(INSTALL_SOLIB_DIR)/ste_img_omxcomponents
SOLIBNAMES            :=$(SOLIBNAME) 
LIBOBJ_$(SOLIBNAME) += womDemoCpt_proxy womDemoCpt_process womDemoCpt_component bellagio_registry 
FLAGS_${SOLIBNAME}  += -llos -lpthread -lste_wom$(DBG_SUF)  -lste_img_ens$(DBG_SUF) -lste_img_enswrapper$(DBG_SUF) -lst_img_osal -lste_osi_toolbox$(DBG_SUF)  -lst_img_processing_thread -lstelpcutils
ifneq ($(MMCOMPILER),android)
FLAGS_ste_fakesink += -lrt
endif

#New rules for managing algo library
lib: lib_algo
install_lib install: install_algo
realclean uninstall: uninstall_algo

lib_algo:
	@echo Check first if algo lib is ok
	$(VERBCHAR)if test -f algo/Makefile; then\
		echo "make algo library" && $(MAKE) -C algo lib;\
	fi

install_algo:
ifeq ($(PLATFORM),x86-linux)
	@echo install $(ALGO_LIB_ROOT) library for x86
	$(VERBCHAR)if test ! -f algo/Makefile; then\
		$(MMINSTALL) $(COMPONENT_TOP_DIR)/algo/$(LIB_DIR_NAME)/x86/lib$(ALGO_LIB_ROOT).a $(INSTALL_LIB_DIRNAME)/x86/lib$(ALGO_LIB_ROOT).a;\
		$(MMINSTALL) $(COMPONENT_TOP_DIR)/algo/$(LIB_DIR_NAME)/x86/lib$(ALGO_LIB_ROOT).a $(INSTALL_LIB_DIRNAME)/x86/lib$(ALGO_LIB_ROOT)$(DBG_SUF).a;\
	fi
else
	@echo install $(ALGO_LIB_ROOT) library for Cortex A9
	$(VERBCHAR)if test ! -f algo/Makefile; then\
		$(MMINSTALL) $(COMPONENT_TOP_DIR)/algo/$(LIB_DIR_NAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT).a $(INSTALL_LIB_DIRNAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT).a;\
		$(MMINSTALL) $(COMPONENT_TOP_DIR)/algo/$(LIB_DIR_NAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT).a $(INSTALL_LIB_DIRNAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT)$(DBG_SUF).a;\
	fi
endif

uninstall_algo:
ifeq ($(PLATFORM),x86-linux)
	@echo uninstall $(ALGO_LIB_ROOT) library for x86
	$(RM) $(INSTALL_LIB_DIRNAME)/x86/lib$(ALGO_LIB_ROOT).a
	$(RM) $(INSTALL_LIB_DIRNAME)/x86/lib$(ALGO_LIB_ROOT)$(DBG_SUF).a
else
	@echo uninstall $(ALGO_LIB_ROOT) library for Cortex A9
	$(RM) -f $(INSTALL_LIB_DIRNAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT).a
	$(RM) $(INSTALL_LIB_DIRNAME)/cortexA9-x-rvct4gnu-x/lib$(ALGO_LIB_ROOT)$(DBG_SUF).a
endif

#for installing algo api headers in imaging dirs
$(INSTALL_HEADER_DIR)%:algo/api%
	@echo " algo api MMINSTALL $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	$(VERCHAR)$(MMINSTALL) $< $@

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e '$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)'

