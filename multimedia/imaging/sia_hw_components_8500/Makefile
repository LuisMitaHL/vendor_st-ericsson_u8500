# -*- Mode: Makefile -*-
# vim:syntax=make:
ifeq ($(NORMAL_BUILD),yes)
DIRECTORIES= nmf proxy

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk
else
#@echo = "$(COMPONENT_TOP_DIR)"
include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

ifeq ($(MMSYSTEM),symbian)
all:	
	make NORMAL_BUILD=yes $@
else ifeq ($(MMSYSTEM),android)
all:
	make NORMAL_BUILD=yes $@
else
all:
	make NORMAL_BUILD=yes $@ 
endif

install uninstall realclean clean:
	    make NORMAL_BUILD=yes $@

PATCH_DIR=${COMPONENT_TOP_DIR}/nmf/mpc/nmf_repository/

endif




#print PATCH_DIR
