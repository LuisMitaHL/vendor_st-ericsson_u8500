######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : Top level android make file for sim system
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################
ifeq ($(SIM_SET_FEATURE_BACKEND),MAL)
include $(call all-subdir-makefiles)
endif
