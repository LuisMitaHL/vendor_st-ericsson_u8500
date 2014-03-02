# -*- Mode: Makefile -*-
# vim:syntax=make:
CORE=arm
MMSYSTEM=x
MMBOARD=x

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)/../..
endif

include $(COMPONENT_TOP_DIR)/make.ost_conf
OST_COMPONENT_ID = $(shell expr $(OST_MODULE_ID) + 2 )
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_host
OST_SRC_DIR = composite cscallcontroller loopbackmodem protocolhandler ultester pcm_codec downlink uplink
INSTALL_DICO = $(OST_DICO_NAME).xml

## Common Config
include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

ITF_PREFIX = cscall

CPPFLAGS+=-DNVERBOSE
SHAREPATH+=$(COMPONENT_TOP_DIR)/common/include
SHAREPATH+=$(COMPONENT_TOP_DIR)/nmf/host/protocolhandler/inc
CXXFLAGS+=-g
# get platform specific flags defined in linux/build/Vars.xxx
CPPFLAGS += $(EXTRA_CPPFLAGS)

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  CPPFLAGS+=-DMMPROBE_ENABLED
endif

ifeq ($(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT),true)
  CPPFLAGS+=-DCSCALL_ENABLE_SILENT_REBOOT_SUPPORT
endif

ITFS_HEADER=cscall.nmf.host.protocolhandler.configure\
			cscall.nmf.host.uplink.configure\
			cscall.nmf.host.downlink.configure


ifeq ($(MYMMSYSTEM),symbian)
SRCPATH+=../../symbian
COMPONENTS= cscall.nmf.host.composite.cscall\
            cscall.nmf.host.composite.cscallLoopULDLAlg
ifneq (,$(wildcard composite/cscalltest1))
COMPONENTS+=cscall.nmf.host.composite.cscalltest1
endif

else ifeq ($(NMF_ENV), linux)
ifeq ($(SW_VARIANT),android)
CPPFLAGS+= -I$(MMROOT)/../access_services/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include/ -DUSE_LOGI -DNO_DTX_FLAG
endif
SRCPATH+=../../linux
COMPONENTS= cscall.nmf.host.composite.cscall\
	        cscall.nmf.host.composite.cscallLoopULDLAlg
ifneq (,$(wildcard composite/cscalltest1))
COMPONENTS+=cscall.nmf.host.composite.cscalltest1
endif


else
COMPONENTS= cscall.nmf.host.composite.cscallLoopULDLAlg\

endif
ifeq ($(MMPLATFORM), x86)
CPPFLAGS+= -DX86
endif

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

