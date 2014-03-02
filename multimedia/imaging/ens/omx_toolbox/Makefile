# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -I../osi_toolbox/src
# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVaromx_toolbox -DCONFIG_DEBUG_AUTOVAR

ifdef DEBUG
	CPPFLAGS   += -D_DEBUG
	CFLAGS      = -g -O0
	OBJ_EXT     =odebug
	DBG_SUF     =_debug
else
	CPPFLAGS   += -g -D_RELEASE
	DBG_SUF     =
endif
LIBNAME     = ste_omx_toolbox$(DBG_SUF)
LIBOBJ   = omx_toolbox_tools 

INSTALL_HEADER = omx_toolbox_tools.h omx_toolbox_shared.h omx_toolbox_lib.h
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#WARNING todo target is MANDATORY (exe and/or lib)
todo:  lib solib $(INSTALL_SOURCE)


ifdef FORCE_EXPORT_OF_SOURCES
INSTALL_SOURCE=install_source
MY_CPP_FILES := $(shell cd src ; find . -name "*.cpp" ; cd ..)

install_source:
	@echo " info FORCE_EXPORT_OF_SOURCES defined -> install source code !!!"
	mkdir -p $(INSTALL_ROOTFS_DIR)/$(PWD)/src
	$(foreach FILE, $(MY_CPP_FILES), ln -fs $(PWD)/src/$(FILE) $(INSTALL_ROOTFS_DIR)/$(PWD)/src ;)
else
INSTALL_SOURCE=
endif

SOLIBNAMES     =${LIBNAME}
INSTALL_SOLIB  =$(SOLIBNAMES)

LIBOBJ_$(LIBNAME) = $(LIBOBJ)
#LIBS_$(LIBNAME)   = ste_osi_toolbox$(DBG_SUF)
#EXTRALIBS_$(LIBNAME)   =ste_osi_toolbox$(DBG_SUF)
FLAGS_$(LIBNAME) += -lste_osi_toolbox$(DBG_SUF)

#how to include libs:
#      FLAGS_$(LIBNAME) += -lste_osi_toolbox$(DBG_SUF) work for .a and .so,  assert if not existing!
#      LIBS_$(LIBNAME)        = ste_osi_toolbox$(DBG_SUF)  only for .a  !!! warning !!!!  Be carrefull says nothing if not existing
# or   EXTRALIBS_$(LIBNAME)   = ste_osi_toolbox$(DBG_SUF)  only for .a !!! warning !!!!  Be carrefull says nothing if not existing
# !!! warning !!!! when .so and .a exist takes the most recent! , cannot force one or other
# !!! warning !!!!  _debug postfix is not inserted anymore

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e '$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)'

