#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
   
#MMBOARD=x
MMSYSTEM=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

INSTALL_HEADER=cli.h

LIBNAME = cli
LIBOBJ  = cli_engine cli_io cli_cmd
INSTALL_LIB = libcli.a 

ifneq ($(MMSYSTEM),linux)
LIBNAME_2 = cli_usb
LIBOBJ_2  = cli_engine_usb cli_io_usb cli_cmd_usb
INSTALL_LIB += libcli_usb.a
DEP_DEP=$(addsuffix _usb.c,$(addprefix src/,$(LIBOBJ)))
endif

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
CFLAGS=-g -O0
else
CFLAGS=-g -O3
CPPFLAGS += -DRELEASE -D__RELEASE
endif

CPPFLAGS += -Iinc


SOLIBNAMES = cli
LIBOBJ_$(SOLIBNAMES) = cli_engine cli_io cli_cmd
FLAGS_$(SOLIBNAMES) += -llos
INSTALL_SOLIB = $(SOLIBNAMES)

ifdef FORCE_EXPORT_OF_SOURCES
MY_CPP_FILES := $(shell cd src ; find . -name "*.cpp" ; cd ..)
MY_C_FILES := $(shell cd src ; find . -name "*.c" ; cd ..) 

my_install:
	@echo " info FORCE_EXPORT_OF_SOURCES defined -> install source code !!!"
#	$(foreach FILE, $(MY_CPP_FILES), echo $(FILE) ;)
	mkdir -p $(INSTALL_ROOTFS_DIR)/$(PWD)/src
	$(foreach FILE, $(MY_CPP_FILES), ln -fs $(PWD)/src/$(FILE) $(INSTALL_ROOTFS_DIR)/$(PWD)/src ;)
# don't work : bug with MMINSTALL when path names are too long
#	$(foreach FILE, $(MY_CPP_FILES), $(MMINSTALL) src/$(FILE) $(INSTALL_ROOTFS_DIR)/$(PWD)/src ;)
	$(foreach FILE, $(MY_C_FILES), ln -fs $(PWD)/src/$(FILE) $(INSTALL_ROOTFS_DIR)/$(PWD)/src ;)

ifeq ($(STE_PLATFORM),ux600_svp)
CPPFLAGS += -DPLATFORME_NAME_UX600
todo: solib my_install
else
todo: lib my_install
endif

else

ifeq ($(STE_PLATFORM),ux600_svp)
CPPFLAGS += -DPLATFORME_NAME_UX600
todo: solib
else
todo: lib
endif

endif



#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

clean realclean: clean_usb

clean_usb:
	rm -f src/*_usb.c

src/%_usb.c: %.c
	@$(LN) $*.c $@

$(OBJ_DIR)/%_usb.$(OBJ_EXT):CPPFLAGS+=-DATE_USB -I$(MMROOT)/shared/usb/include

test cleantest:
	@echo No Test...

.SECONDARY:
