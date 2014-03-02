# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------

CPPFLAGS += -DMMU_OFFSET

CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

SHAREPATH=../../share

CPPFLAGS += -I$(MM_HEADER_DIR)
CPPFLAGS += -I$(MM_NMF_REPO_BOARD_DIR)
CPPFLAGS += -I$(INSTALL_HEADER_BASEDIR)

CPPFLAGS += -Isw_3A/inc

# Keep some debug info overwise T32 cannot associate the sources
CXXFLAGS += -g

ITFS_HEADER=	\

COMPONENTS=sw_3A_wrp

ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG
CFLAGS    = -g -O0
CXXFLAGS += -g -O0
OBJ_EXT   = odebug
else
#CPPFLAGS += -DRELEASE -D__RELEASE
endif

# CR337836 Activate dynamic trace
 CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarsw3a -DCONFIG_DEBUG_AUTOVAR

export CPPFLAGS
export CXXFLAGS

NMFCONFC_FLAGS= "--cpp --archive"

custom_install:
ifeq ($(MMSYSTEM),linux)
ifeq ($(BUILD_GCC_CODE),true)
	@if test ! -d  $(IMG_REPOSITORY_HOST) ; then mkdir -p $(IMG_REPOSITORY_HOST); echo ***Creating directory $(IMG_REPOSITORY_HOST) ; fi 
	armar -r $(IMG_REPOSITORY_HOST)/sw_3A_wrp_a.a $(MEDIUM_DIR)/sw3A/nmf/host/$(BASE_REPOSITORY_HOST)/sw_3A_wrp.a
endif
endif

custom_uninstall:
ifeq ($(MMSYSTEM),linux) 
ifeq ($(BUILD_GCC_CODE),true)
	@echo "***Removing lib $(IMG_REPOSITORY_HOST)/sw_3A_wrp_a.a"
	@if test -e  $(IMG_REPOSITORY_HOST)/sw_3A_wrp_a.a ; then rm -f $(IMG_REPOSITORY_HOST)/sw_3A_wrp_a.a; echo ***Remove done ; fi 
endif
endif

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

