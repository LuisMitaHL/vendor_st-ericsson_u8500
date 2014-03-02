# vim:syntax=make:
# -*- Mode: Makefile -*-
MMSYSTEM=x
MMBOARD =x

INCLUDE_LINUX_HEADERS=yes
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifdef DEBUG
	CPPFLAGS   += -D_DEBUG
	CPPFLAGS   += -g -O3
	OBJ_EXT     =odebug
	DBG_SUF     =_debug
else
	CPPFLAGS   += -g -D_RELEASE 
	DBG_SUF     =
endif
EXENAME     = wom_nonregr$(DBG_SUF)
EXEOBJ      = wom_nonregr
INSTALL_BIN =$(EXENAME)

#WARNING todo target is MANDATORY (exe and/or lib solib)
todo: exe

#solib part
LIBOBJ_$(EXENAME) = $(LIBOBJ)
# don't work FLAGS_$(EXENAME) += -lpthread  -lste_osi_toolbox$(DBG_SUF) -lste_img_enswrapper$(DBG_SUF) 
EXTRA_LIBS+=  -lste_osi_toolbox$(DBG_SUF) -lste_img_enswrapper -lste_omx_toolbox$(DBG_SUF) -lste_img_ens$(DBG_SUF) -lstelpcutils -lste_wom$(DBG_SUF)

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e '$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)'

