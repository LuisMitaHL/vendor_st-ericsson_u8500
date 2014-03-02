# -*- Mode: Makefile -*-
# vim:syntax=make:

# Note: CORE macro must be placed before Make.nmf.config
CORE=arm

include make.ost_conf
include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

ifdef DEBUG
CFLAGS    = -g -O0
CPPFLAGS += -DDEBUG
CPPFLAGS += -D__DEBUG
CPPFLAGS += -DOSI_DEBUG
CPPFLAGS += -DFETCH_FILES_FROM_SDCARD
else
CPPFLAGS += -DRELEASE
CPPFLAGS += -D__RELEASE
endif

ifdef FETCH_FILES_FROM_SDCARD
CPPFLAGS += -DFETCH_FILES_FROM_SDCARD
endif

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarTuningLoader -DCONFIG_DEBUG_AUTOVAR

ITF_PREFIX = tuningloader

MMINSTALL_ARGS += -n=/ee/api/trace.hpp \
                  -n=/ee/api/panic.idt.h

# TODO remove once dependency on sensor is cut
CPPFLAGS += -I$(MM_NMF_REPO_BOARD_DIR)

SHAREPATH += $(MM_NMF_COMP_SRC_HOSTEE_DIR)/share

CXXFLAGS  += -I$(MM_HEADER_DIR)
CPPFLAGS  += -Ituningloader_core/inc
CPPFLAGS  += -Ituningloader_core/ccal/ccal_lib/include

COMPONENTS = tuningloader_wrp

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk
