# vim:syntax=make:
# -*- Mode: Makefile -*-


#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD =x
INCLUDE_LINUX_HEADERS=yes


include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -I $(MM_HEADER_DIR)
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarImgValidImgEnsTest -DCONFIG_DEBUG_AUTOVAR
CPPFLAGS += -DIMGENS_TEST_CPT_API=

ifdef DEBUG
	CPPFLAGS += -DDEBUG -D__DEBUG  -D_DEBUG
	CPPFLAGS +=-g -O0
	OBJ_EXT =odebug
	DBG_SUF=_debug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	DBG_SUF=
endif
SOLIBNAME = st_omximgens_test


INSTALL_HEADER = 
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#WARNING todo target is MANDATORY (exe and/or lib)
todo: solib

ifneq ($(MYMMSYSTEM), symbian)
# We are generating an OMX component .so - request to be installed in correct place
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk
INSTALL_SOLIB_DIR:=$(INSTALL_SOLIB_DIR)/ste_img_omxcomponents
# redefine output dir for imaging components

SOLIBNAMES          :=$(SOLIBNAME) 
LIBOBJ_$(SOLIBNAME) += bellagio_integration imgenstest_demo
# imgenstest_component imgenstest_proxy imgenstest_processor

#FLAGS_${SOLIBNAME} += $(IFM_LIB) $(LIB_DIR_LIST) $(SOLIB_DIR_LIST)
FLAGS_${SOLIBNAME} += -llos -lpthread  -lste_img_ens$(DBG_SUF) -lste_img_enswrapper -lst_img_osal -lste_osi_toolbox$(DBG_SUF)  -lst_img_processing_thread -lstelpcutils
#-lste_wom$(DBG_SUF) 
#$(LDFLAGS_STE_ENS_OMX_COMPONENTS) 
ifneq ($(MMCOMPILER),android)
FLAGS_ste_imgens_test += -lrt
endif

INSTALL_SOLIB = $(SOLIBNAMES)
#endif 
endif


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e "$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)"


