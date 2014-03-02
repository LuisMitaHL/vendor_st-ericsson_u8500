#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson. All rights reserved.                *
# *  This code is ST-Ericsson proprietary and confidential.              *
# *  Any use of the code for whatever purpose is subject to              *
# *  specific written permission of ST-Ericsson SA.                      *
# *                                                                      *
# *  Author: Axel Lilliecrantz <axel.lilliecrantz@stericsson.com>        *
# *                                                                      *
# ************************************************************************/

CSPSATOOLSDIR = ../cspsa-tools

ifeq ($(O_TARGET),)
	BUILDDIR_TARGET :=
else
	BUILDDIR_TARGET := $(O_TARGET)/
endif

ifeq ($(O_HOST),)
	BUILDDIR_HOST :=
else
	BUILDDIR_HOST := $(O_HOST)/
endif

create-cspsa-image:
	make O=$(BUILDDIR_HOST) -C ../cspsa -f Makefile libs
	make O=$(BUILDDIR_HOST) -C ../cspsa-tools -f Makefile host
	$(call create-cspsa-files,$(STE_PLATFORM)_default_cspsa.bin,$(STE_PLATFORM)_default_cspsa.gdf)

# Function to create cspsa images.
# Input parameters
# $1 bin output file name
# $2 gdf output file name
# From $2 we get the nwm.gdf file, we string replacement below.
define create-cspsa-files
	mkdir -p nwm/modemfs/RFHAL
	mkdir -p pff/0x00000000-0x0000FFFF.modem
	rm -f pff/0x00000000-0x0000FFFF.modem/*
	$(BUILDDIR_HOST)/nwm2gdf -o $(BUILDDIR_TARGET)$(2:%.gdf=%_nwm.gdf) nwm
	$(BUILDDIR_HOST)/gdf2pff -p pff/0x00000000-0x0000FFFF.modem -n modem $(BUILDDIR_TARGET)$(2:%.gdf=%_nwm.gdf)
	$(BUILDDIR_HOST)/pff2cspsa -d -t 0xFFFFE000 -f $(BUILDDIR_TARGET)$1 -z 0x100000 pff
	$(BUILDDIR_HOST)/cspsalist -o $(BUILDDIR_TARGET)$2 $(BUILDDIR_TARGET)$1
	rm -f pff/0x00000000-0x0000FFFF.modem/*
	rmdir pff/0x00000000-0x0000FFFF.modem
endef

.PHONY: all
all: build

.PHONY: build
build: create-cspsa-image

.PHONY: clean
clean:
	rm -f $(BUILDDIR_TARGET)*.bin $(BUILDDIR_TARGET)*.gdf
	make O=$(BUILDDIR_HOST) -C ../cspsa clean
	make O=$(BUILDDIR_HOST) -C ../cspsa-tools clean

.PHONY: install
install:

.PHONY: distclean
distclean: clean

.PHONY: config
config:

