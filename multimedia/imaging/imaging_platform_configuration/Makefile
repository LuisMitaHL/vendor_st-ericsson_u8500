# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------

ifdef IMG_SYMBIAN_PREBUILD

GENERATED_IMG_FILE=$(TARGET)
DEPENDANCE=

else

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

TOBECLEAN = include/ImgSelectedConfig.h

#VPATH+=include
INSTALL_HEADER = ImgSelectedConfig.h ImgConfig.h

GENERATED_IMG_FILE=include/ImgSelectedConfig.h
DEPENDANCE=Makefile
GENERATED_IMG_CONFIG := $(shell grep IMG_CONFIG $(GENERATED_IMG_FILE))
GENERATED_IMG_CONFIG := $(lastword $(GENERATED_IMG_CONFIG))
ifneq ($(GENERATED_IMG_CONFIG), $(IMG_CONFIG))
$(shell rm -f $(GENERATED_IMG_FILE))
$(warning removing generated old file)
endif
todo: include/ImgSelectedConfig.h install

endif # IMG_SYMBIAN_PREBUILD

#--------------------
# generate header file
#--------------------
$(GENERATED_IMG_FILE): $(DEPENDANCE)
	@echo "selected IMG_CONFIG: $(IMG_CONFIG)"
	@echo "/*****************************************************************************/" > $(GENERATED_IMG_FILE)
	@echo "/**                                                                            " >> $(GENERATED_IMG_FILE)
	@echo "* Copyright (C) ST-Ericsson SA 2010. All rights reserved.                      " >> $(GENERATED_IMG_FILE)
	@echo "* This code is ST-Ericsson proprietary and confidential.                       " >> $(GENERATED_IMG_FILE)
	@echo "* Any use of the code for whatever purpose is subject to                       " >> $(GENERATED_IMG_FILE)
	@echo "* specific written permission of ST-Ericsson SA.                               " >> $(GENERATED_IMG_FILE)
	@echo "*                                                                              " >> $(GENERATED_IMG_FILE)
	@echo "* \\brief   ImgSelectedConfig Version                                          " >> $(GENERATED_IMG_FILE)
	@echo "* \\author  ST-Ericsson                                                        " >> $(GENERATED_IMG_FILE)
	@echo "*/                                                                             " >> $(GENERATED_IMG_FILE)
	@echo "/*****************************************************************************/" >> $(GENERATED_IMG_FILE)
	@echo "// FILE GENERATED, DO NOT EDIT" >> $(GENERATED_IMG_FILE)
	@echo "                              " >> $(GENERATED_IMG_FILE)
ifdef IMG_CONFIG
	@echo "$(GENERATED_IMG_FILE) file is generated"
	@echo "#define IMG_CONFIG $(IMG_CONFIG)" >> $(GENERATED_IMG_FILE)
else
	@echo "IMG_CONFIG is not defined !!"
	@rm -f $(GENERATED_IMG_FILE)
	@false
endif


ifndef IMG_SYMBIAN_PREBUILD
#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

clean realclean: clean_generated

clean_generated:
	rm -f $(GENERATED_IMG_FILE)

endif # IMG_SYMBIAN_PREBUILD
