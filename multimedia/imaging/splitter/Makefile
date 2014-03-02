# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk


CPPFLAGS += -I $(MM_NMF_COMP_SRC_HOSTEE_DIR)/nmf/host
CPPFLAGS += -I$(MM_HEADER_DIR)
CPPFLAGS += -I $(MM_HEADER_DIR)/openmax_processor_wrp

CPPFLAGS += -DUSE_BUFFERSHARING_BROADCAST
#CPPFLAGS += -DUSE_BUFFERSHARING_SEQUENTIAL

ifdef DEBUG
	CPPFLAGS += -DDEBUG -D__DEBUG  -DOSI_DEBUG
	LIBNAME   = st_omxsplitter_debug
	SOLIBNAME   = st_omxsplitter_debug
	CFLAGS=-g -O0
	OBJ_EXT =odebug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	LIBNAME   = st_omxsplitter
	SOLIBNAME   = st_omxsplitter
endif

LIBOBJ   = splitter_proxy splitter_process

INSTALL_HEADER =
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))



ifneq ($(MYMMSYSTEM), symbian)
ifneq ($(PLATFORM),x86-linux)
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES = $(SOLIBNAME)
LIBOBJ_$(SOLIBNAMES) = ste_omx_ens_component
IMAGE_LIBS = $(LIBNAME) st_wrapper_openmax
# WA as no immediate completion for ifm 
IFM_LIB = $(wildcard $(foreach mmpath, $(MMSEARCHPLAT), $(MM_LIB_DIRNAME)/$(mmpath)/libst_ifm.a ))
IMAGING_NMF_HOST_COMPONENTS = splitter_wrp openmax_processor_wrp

SOLIBS_${SOLIBNAMES} += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_${SOLIBNAMES}= $(IMAGE_LIBS)
PLATFORM_LIST = cortexA9-linux-gnu-$(MYMMBOARD) cortexA9-linux-gnu-x cortexA9-x-rvct-x cortexA9-x-gnu-$(MYMMBOARD) cortexA9-x-rvct4gnu-x cortexA9-x-gnu-x
LIB_DIR_LIST += $(foreach mmpath, $(PLATFORM_LIST), $(addprefix -L,$(MM_LIB_DIRNAME)/$(mmpath)))
SOLIB_DIR_LIST =  $(foreach mmpath, $(PLATFORM_LIST), $(addprefix -L,$(MMBUILDIN)/$(ROOTFS_DIR_NAME)/$(mmpath)$(COMPONENT_PATH)))

FLAGS_$(SOLIBNAMES) += $(IFM_LIB) $(LIB_DIR_LIST) $(SOLIB_DIR_LIST) 
FLAGS_$(SOLIBNAMES) += $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -llos 

INSTALL_SOLIB = ${SOLIBNAMES}
endif
endif

ifeq ($(MYMMSYSTEM), symbian)
solib: 
	@echo solib, nothing to do!
endif


MMVALIDROOT = usr/share/mm-valid

#WARNING todo target is MANDATORY (exe and/or lib)
all todo: lib solib ramdisk
ifneq ($(MYMMSYSTEM),android)
install: ramdisk

# Prepare all scripts to be copied in the Linux ramdisk
ramdisk:
	$(MMINSTALL) -d $(COMPONENT_TOP_DIR)/mmte_script $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/splitter/mmte_script

uninstall clean: clean_ramdisk
clean_ramdisk:
	$(MMINSTALL) -r -d $(COMPONENT_TOP_DIR)/mmte_script $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/splitter/mmte_script

else
ramdisk:
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#ifeq ($(MMSYSTEM),symbian)
#NO_VARIANT=true
#include $(MEDIUM_DIR)/symbian_makerules/mmh.mk
#endif
