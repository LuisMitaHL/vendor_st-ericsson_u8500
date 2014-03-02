# -*- Mode: Makefile -*-
# vim:syntax=make:

# Note: CORE macro must be placed before SharedNmfConfig.mk
CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

#COMPONENTS= tuningloader_wrp_template 
INSTALL_COMPONENTS = tuningloader_wrp_template \
		tuningloader

## Request sources to be installed 
INSTALL_NMF_COMP_SRC=true

## Install share directory content
INSTALL_NMF_COMP_SRC_HOSTEE += share/tuningloader_types.idt

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

