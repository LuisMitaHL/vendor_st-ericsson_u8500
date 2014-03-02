#-------------------------------
# Default Config
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.config
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# ite_nmf ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USB_SPECIFIC = ite_main
DEP_DEP =$(addsuffix _usb.c,$(addprefix src/console/,$(USB_SPECIFIC)))
CFLAGS=-Werror -Wall -g -O0

ifndef USB
#EXENAME = ite_$(PLATFORM)
EXENAME = ite_nmf
else
EXENAME = ite_$(PLATFORM)_usb
endif
INSTALL_BIN = $(EXENAME)

EXEOBJ = configCM

DEP_DEP += src/configCM.c
TOBECLEAN += src/configCM.c
#--------------------------------------
# Test file path
#--------------------------------------
# choose one of the values here for IMAGING_PATH
IMAGING_PATH=/sdcard
#--------------------------------------
# Generate ite_nmf library
#--------------------------------------

LIBNAME = ite_nmf
LIBOBJ += $(subst .c,,$(notdir $(shell find ./src -type f -name "*.c")))
LIBOBJ += $(subst .cpp,,$(notdir $(shell find ./src -type f -name "*.cpp")))
VPATH += $(subst ./,,$(shell find ./inc -type d))
VPATH += $(subst ./,,$(shell find ./src -type d))
CPPFLAGS += -Iinc/console -O0 -I$(MMROOT)/shared/mmte/inc -Iinc/verif_test -Iinc/miscelious -Iinc/pipe -Iinc/grab -Isrc/miscelious/linux
#-----------------------
# Display API'S
#-----------------------
# We should be able to not use the following (blt_api.h should be enough) (for b2r2_blt.h use by the display function...)
CPPFLAGS += -I$(MMROOT)/linux/b2r2lib/src -I$(MMROOT)/linux/b2r2lib/include -I$(MMROOT)/linux/kernel/u8500_v2-linux/include/video

# for nmf_tuning (C++ tuning encapsulation)
CC += -D__C_NMF_TUNING
CPPFLAGS += -D__DEBUG
CPPFLAGS += -D$(ARM_DEBUGGER)
ifeq ($(MMIO_ENABLE_NEW_IF),true)
  CPPFLAGS += -DMMIO_ENABLE_NEW_IF
endif
# ER:408207: Flash driver integration in ITE NMF.
# Comment below line to remove flash driver support from ITE-NMF
FLASH_DRIVER_SUPPORT = 1
ifeq ($(FLASH_DRIVER_SUPPORT),1)
CPPFLAGS+=-DFLASH_DRIVER_INCLUDE=1
endif 
INSTALL_LIB += libite_nmf.a

#from ite/Make.common
ARM_DEBUGGER = ARM_DEBUGGER_TRUE
VALIDATION_TMP = $(MMROOT)/imaging
ifdef WIN_ROOT
    VALIDATION = $(subst $(MMROOT),$(WIN_ROOT),$(VALIDATION_TMP))
else
    VALIDATION = $(VALIDATION_TMP)
endif
CPPFLAGS += -I$(IMG_REPOSITORY)/host -DITE_ROOT_DIR=\"$(VALIDATION)\"1

ifdef FLAT
CPPFLAGS += -D__THINK_FLAT_MAPPING
endif

ifeq ($(findstring linux,$(PLATFORM)),linux)
CPPFLAGS += -DIMAGING_PATH=\"$(IMAGING_PATH)\" -I$(MMROOT)/linux/include
LIBS+= liblos.a libnmf.a libnmfee.a libst_mmio.a  libst_mmhwbuffer.a libst_ens.a libst_omxilosalservices.a libst_isp8500_firmware_primary.a libst_isp8500_firmware_secondary.a libst_ifm.a libite_darkbox.a
endif

ifeq ($(MMSYSTEM),android)
CPPFLAGS += -DIMAGING_PATH=\"$(IMAGING_PATH)\" -I$(MMROOT)/linux/include
LIBS+= liblos.a libnmf.a libnmfee.a libst_mmio.a  libst_mmhwbuffer.a libst_ens.a libst_omxilosalservices.a libst_ifm.a
endif

#Use CUSTOMER env variable for compiling specific code
#ifeq ($(CUSTOMER),CUSTOMER1)
ifeq ($(IMG_CONFIG),100)
CPPFLAGS += -DCUSTOMER1
endif
#ifeq ($(CUSTOMER),CUSTOMER2)
ifeq ($(IMG_CONFIG),201)
CPPFLAGS += -DCUSTOMER2
endif
ifeq ($(IMG_CONFIG),102)
CPPFLAGS += -D__TUNING_FILES_AVAILABLE__
LIBS+= libst_tuningloader_sync.a libst_variationif.a
endif

LDFLAGS= -lstelpcutils
# href Android Platform
ifeq ($(MMSYSTEM),android)
# ste_ensloader: component using MMHWbuffer lib - currently within libste_ensloader.so
LDFLAGS += $(addprefix -L,$(wildcard $(ANDROID_BSP_ROOT)/$(PRODUCT_OUT)/obj/mmbuildout/lib/* $(ANDROID_BSP_ROOT)/$(PRODUCT_OUT)/obj/mmbuildout/rootfs/*/system/lib))
LDFLAGS += $(addprefix -Wl$(comma)-rpath-link -Wl$(comma),$(wildcard $(ANDROID_BSP_ROOT)/$(PRODUCT_OUT)/obj/mmbuildout/lib/* $(ANDROID_BSP_ROOT)/$(PRODUCT_OUT)/obj/mmbuildout/rootfs/*/system/lib))
LDFLAGS+= -lcutils -lste_ensloader -ltrace
LDFLAGS+= -lst_sensor -lste_isp8500_firmware_primary -lste_isp8500_firmware_secondary
comma:= ,

else
LDFLAGS+= -ltrace
endif

ifeq ($(FLASH_DRIVER_SUPPORT),1)
LDFLAGS+= -lcamera_flash
endif

LIBS+= libst_sia_itfs.a libite_nmf.a libite_common.a

ifneq ($(findstring linux,$(PLATFORM)),linux)
ifneq ($(MMSYSTEM),android)
LIBS+= libdisplay_8500_v1_mmuoffset.a
endif
endif

ifndef USB
LIBS+= libcli.a
else
LIBS+= libcli_usb.a
endif

ifeq ($(MMSYSTEM),android)
todo: readhwreg lib exe install
else
todo: lib exe ramdisk
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules

#-------------------------------
# local Rules
#-------------------------------
#-----------------
# USB specificity
#-----------------
src/console/$(USB_SPECIFIC)_usb.c: $(USB_SPECIFIC).c
	@echo "Symbolic link: $(LN) $(USB_SPECIFIC).c $@ "
	@$(LN) $(USB_SPECIFIC).c $@

$(OBJ_DIR)/%_usb.$(OBJ_EXT):CPPFLAGS+=-DMMTE_USB_TRUE -I$(MMROOT)/shared/usb/include

#---------------------------------------------
# Linux ramdisk generation
#---------------------------------------------
ifeq ($(findstring linux,$(PLATFORM)),linux)
RAMDISK_DIR = $(MMROOT)/imaging/rootfs/$(MMPLATFORM)
MMVALIDROOT = usr/share/mm-valid

# Prepare all the ".ite" scripts to be copied in the Linux ramdisk
ramdisk:
	rm -rf $(RAMDISK_DIR)/$(MMVALIDROOT)/imaging/ite_nmf/;
	mkdir -p $(RAMDISK_DIR)/$(MMVALIDROOT)/imaging/ite_nmf/;
	#Copy all the ".ite" files with their path
	cp -r $(MMROOT)/imaging/ite_nmf/script $(RAMDISK_DIR)/$(MMVALIDROOT)/imaging/ite_nmf/;
	#Copy the test result directory
	cp -r $(MMROOT)/imaging/ite_nmf/test_results_nreg $(RAMDISK_DIR)/$(MMVALIDROOT)/imaging/ite_nmf/;

endif

#-----------------------------------------------------------
# Prepare all scripts to be copied to board's file system
#-----------------------------------------------------------
ifeq ($(MMSYSTEM),android)

MMVALIDROOT = system/usr/share/mm-valid
readhwreg:
	(cd $(MMROOT)/imaging/ite_nmf/readHwReg;make -f Makefile;)
install:
	$(MMINSTALL) -d script $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/ite_nmf/script
endif

#--------------------
# CMinit generation
#--------------------
.PHONY:

REPOSITORY_STRING_TMP=$(WIN_ROOT)"/shared/nmf/mpcee/repository/mmdsp_8500";$(SHARED_REPOSITORY);$(AUDIO_REPOSITORY);$(VIDEO_REPOSITORY);$(IMG_REPOSITORY)
ifdef WIN_ROOT
REPOSITORY_STRING = $(subst $(MMROOT),$(WIN_ROOT),$(REPOSITORY_STRING_TMP))
else
REPOSITORY_STRING = $(REPOSITORY_STRING_TMP)
endif

src/configCM.c:Makefile
	@echo "/*****************************************************************************" > $@
	@echo "* 									     " >> $@
	@echo "* Copyright (C) ST-Ericsson SA 2010. All rights reserved.		     " >> $@
	@echo "* This code is ST-Ericsson proprietary and confidential.			     " >> $@
	@echo "* Any use of the code for whatever purpose is subject to			     " >> $@
	@echo "* specific written permission of ST-Ericsson SA.				     " >> $@
	@echo "*									     " >> $@
	@echo "*****************************************************************************/" >> $@
	@echo "// FILE GENERATED, DO NOT EDIT" >> $@
	@echo "#ifndef $(subst -,_,$(subst .,_,$(notdir $@)))" >> $@
	@echo "#define $(subst -,_,$(subst .,_,$(notdir $@)))" >> $@
	@echo "#define HDD_REPOSITORY \"$(REPOSITORY_STRING)\"" >> $@
	@echo "#include \"mmte_nmf_utils.c\"" >> $@
	@echo "#endif" >> $@
	@echo "$@ generated"

.SECONDARY:
