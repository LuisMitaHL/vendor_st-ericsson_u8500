######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : Top level android make file for sim system
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

ifeq ($(SIM_ENABLE_FEATURE_SIM),true)
#SIM_PATH := $(call my-dir)
#include $(CLEAR_VARS)

# Libs
#include $(SIM_PATH)/libsimcom/Android.mk
#include $(SIM_PATH)/libsim/Android.mk
#include $(SIM_PATH)/cat/Android.mk
#include $(SIM_PATH)/libapdu/Android.mk
#include $(SIM_PATH)/uiccd/Android.mk

# Executables
#include $(SIM_PATH)/simd/Android.mk
#include $(SIM_PATH)/tcat/Android.mk
#include $(SIM_PATH)/tapdu/Android.mk
#include $(SIM_PATH)/tuicc/Android.mk
include $(call all-subdir-makefiles)
endif
