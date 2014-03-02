# vim:syntax=make:
# -*- Mode: Makefile -*-
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

INLST    =$(wildcard testlist_$(PLATFORM).in)
OUTLST   =$(wildcard testlist_$(PLATFORM).out)

ifeq ($(INLST),)
all clean test cleantest realclean:
	@echo No testlist for PLATFORM $(PLATFORM)
else
TEST_TAG =$(PLATFORM)
DATABASE =omx
LSF_QUEUE=reg
TESTFLAGS=-f

include $(MM_MAKEFILES_DIR)/AudioTest.mk

all: test
realclean clean: cleantest

endif
