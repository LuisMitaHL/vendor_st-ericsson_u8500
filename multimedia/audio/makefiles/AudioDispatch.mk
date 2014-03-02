# -*- Mode: Makefile -*-
# vim:syntax=make:

#-----------------------
# List of Directory
#-----------------------
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

DIRECTORIES= $(wildcard lib*) codec effect src standalone

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk


