# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/AudioFlags.mk

CPPFLAGS += -I$(INSTALL_HEADER_DIR)
CPPFLAGS += -I$(REPOSITORY) -I$(SHARED_REPOSITORY)
CPPFLAGS += -I$(AUDIO_REPOSITORY_HOST)
CPPFLAGS += -I$(AUDIO_REPOSITORY_HOST)/host
# we use MMROOT here since mmprobe does not export its headers
# (since out of the mm build system)
CPPFLAGS += -I$(MMROOT)/audio/mmprobe/api

CPPFLAGS+=-I$(COMPONENT_TOP_DIR)/inc/autogentraces
