# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/AudioAlgoConfig.mk
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

TEST_TAG =$(MMDSPPROCESSOR)
TESTLISTS= testlist.lst
DATABASE = regression_std
include $(MM_MAKEFILES_DIR)/AudioTest.mk

