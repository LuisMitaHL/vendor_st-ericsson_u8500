#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
MMSYSTEM=x
MMBOARD =x
INCLUDE_LINUX_HEADERS=yes

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

INSTALL_HEADERDIR = inc
VPATH+= src

ifdef DEBUG
	CPPFLAGS += -D_DEBUG
	CPPFLAGS +=-g -O3
	OBJ_EXT   =odebug
	FORCE_EXPORT_OF_SOURCES=1
	DBG_SUF   =_debug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	DBG_SUF   =
endif

CPPFLAGS+= -Iinc -Iinc\Component 

LIBOBJ = Img_EnsWrapper  ImgCallbackThread Img_ComponentManager Img_EnsWrapper_services Img_omxloader Img_EnsWrapper_Shared
	
#	Img_EnsWrapper_DumpOMXparameter-$(PLATFORM) \
#	Img_EnsHwRm Img_EnsWrapper_Rme 
#	Img_EnsWrapper_Construct \
#	Img_EnsWrapper_OSTtrace \


ifeq ($(MMCOMPILER),android)
  LIBOBJ += Img_EnsWrapper_DumpMemInfo Img_mapinfo
else
  CXXFLAGS += -std=gnu++0x
endif
#no debug version because of need of single version of wrapper
LIBNAME          =ste_img_enswrapper
#INSTALL_LIB      =lib${LIBNAME}.a

SOLIBNAME1          = $(LIBNAME)
SOLIBNAMES          = $(SOLIBNAME1)
INSTALL_SOLIB       = ${SOLIBNAMES}
LIBOBJ_$(SOLIBNAME1)= $(LIBOBJ)
FLAGS_$(SOLIBNAME1)+= -lst_img_processing_thread -lst_img_osal -lpthread -lrt  -ldl -losttrace  -lst_mmhwbuffer
FLAGS_$(SOLIBNAME1)+= -lst_rme -lst_omxilosalservices -lst_img_osal -lste_osi_toolbox$(DBG_SUF)  -lste_omx_toolbox$(DBG_SUF) -lstelpcutils
ifneq ($(MMPROCESSOR),x86)
  FLAGS_$(LIBNAME) +=-lstelpcutils  
else
  FLAGS_$(LIBNAME) +=
endif

#-lst_rme -lst_rme$(DBG_SUF)
PERL5LIB := $(PERL5LIB):$(PWD)/perllibs
export PERL5LIB

IMG_ENSWRAPPER_DUMPOMXPARAMETER_SRC:=$(BUILD_TMP_DIR)/src/Img_EnsWrapper_DumpOMXparameter-$(PLATFORM).cpp
IMG_ENSWRAPPER_DUMPOMXPARAMETER_LOG:=$(BUILD_TMP_DIR)/GenerateOMXdebug.log

DEP_DEP   := $(IMG_ENSWRAPPER_DUMPOMXPARAMETER_SRC)
TOBECLEAN := $(IMG_ENSWRAPPER_DUMPOMXPARAMETER_LOG) $(IMG_ENSWRAPPER_DUMPOMXPARAMETER_SRC)
VPATH     += $(BUILD_TMP_DIR)/src

todo: install_source install_loader solib 
.PHONY: install_source install_loader solib

ifdef FORCE_EXPORT_OF_SOURCES
install_source:
	@echo " img_enswrapper install source code in $(INSTALL_ROOTFS_DIR)$(PWD)!!!"
	mkdir -p $(INSTALL_ROOTFS_DIR)$(PWD)
	if  test ! -d $(INSTALL_ROOTFS_DIR)$(PWD)/src; then ln -fs $(PWD)/src $(INSTALL_ROOTFS_DIR)$(PWD)/src; fi
else
install_source:
	@echo "source code not exported !!!"
endif

install_loader:
	@echo "Install imaging omxloaders file into  $(INSTALL_ROOTFS_DIR)$(STE_COMPONENT_LOADER_PATH)/$(OMX_LOADERS_FILE)"
	$(MMINSTALL) $(OMX_LOADERS_FILE) $(INSTALL_ROOTFS_DIR)$(STE_COMPONENT_LOADER_PATH)/$(OMX_LOADERS_FILE)


install: install_loader install_source

#install specific loader file
OMX_LOADERS_FILE=omxloaders
ifeq ($(MMCOMPILER),android)
  STE_COMPONENT_LOADER_PATH=/system
else
  STE_COMPONENT_LOADER_PATH=
endif



# lib
## GenerateOMXdebug uses MM_HEADER_BASEDIR to search for headers
## and not MMROOT any more as not relevant for RPM build
export MM_HEADER_BASEDIR
export MMBUILDIN
export MM_IDT_DIR
export COMPONENT_TOP_DIR

$(IMG_ENSWRAPPER_DUMPOMXPARAMETER_SRC):
	@echo "  Generating "$(notdir $@)
	$(VERCHAR)mkdir -p $(BUILD_TMP_DIR)/src
	$(VERCHAR)GenerateOMXdebug $@ > $(IMG_ENSWRAPPER_DUMPOMXPARAMETER_LOG) 2>&1

test:
	@echo "No Test Available for img_enswrapper component"

include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e "$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)"
