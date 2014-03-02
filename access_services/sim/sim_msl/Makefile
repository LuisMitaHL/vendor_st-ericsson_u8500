######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : Make file for sim modem simulator
## 
## Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
## 
###############################################################################


SIM_DIR := ../sim

SUBDIRS := libmsl


.PHONY: $(SUBDIRS) all clean splint instrument_funcs

all: $(SUBDIRS)
all: TARGET=all

clean: $(SUBDIRS)
		@rm -fr *.info	#remove code coverage files
		@rm -fr */*.gcda	#remove code coverage files
		@rm -fr */*.gcno	#remove code coverage files
clean: TARGET=clean

splint: $(SUBDIRS)
splint: TARGET=splint

instrument_funcs: $(SUBDIRS)
instrument_funcs: TARGET=instrument_funcs

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) $(TARGET)
