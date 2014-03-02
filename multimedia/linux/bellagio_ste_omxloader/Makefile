#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

OMX_COMPONENT_PATH:=$(COMPONENT_PATH)/$(OMX_COMPONENTS_DIR)/

# The directory to take OMX .so from (default value provided as can be set from the environment)
CPPFLAGS += -DOMX_COMPONENT_PATH='"$(OMX_COMPONENT_PATH)"'
CPPFLAGS += -I$(MMROOT)/linux/bellagio_omxcore/libomxil-bellagio/src

SOLIBNAMES+=ste_ensloader

ifeq ($(ENABLE_FEATURE_BUILD_HATS),true)
CPPFLAGS+= -DENABLE_FEATURE_BUILD_HATS
endif

LIBOBJ_ste_ensloader = ste_ensloader

EXTRALIBS_ste_ensloader = st_enswrapper st_ensfsm_host \
                          st_mmhwbuffer st_omxilosalservices \
                          osttrace st_ens st_rme

CHECK_EXTRALIBS_ste_ensloader=true

INSTALL_HEADER = ste_omx_ens_component_loader.h

ifneq ($(STE_SOFTWARE),android)
  FLAGS_ste_ensloader += -lpthread -lrt
endif

FLAGS_ste_ensloader += -ldl -lnmfee -llos -lstelpcutils $(NMF_CM_LIB) $(STM_TRACE_LIB)

ifeq ($(STE_SOFTWARE),android)
  FLAGS_ste_ensloader += -lbassapp
  FLAGS_ste_ensloader += -lhardware
endif

INSTALL_SOLIB=ste_ensloader

todo: lib solib

OMX_LOADERS_FILE=omxloaders
ifeq ($(MMCOMPILER),android)
  STE_COMPONENT_LOADER_PATH=/system
else
  STE_COMPONENT_LOADER_PATH=
endif

install_loader:
	echo $(COMPONENT_PATH)/libste_ensloader.so > $(OMX_LOADERS_FILE)
	$(MMINSTALL) $(OMX_LOADERS_FILE) $(INSTALL_ROOTFS_DIR)/$(STE_COMPONENT_LOADER_PATH)/$(OMX_LOADERS_FILE)

todo: install_loader
## Added to get it in Android binary build also.
install: install_loader

include $(MM_MAKEFILES_DIR)/SharedRules.mk

clean realclean: clean_loader

clean_loader:
	rm -f $(OMX_LOADERS_FILE)
