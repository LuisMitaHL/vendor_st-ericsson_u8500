# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
MODULE:=..

all : nmf-all
clean : nmf-clean 
realclean : nmf-realclean nmf-conf-clean

#DUMMY:=$(shell mkdir ../../host)

nmf-all nmf-idl-doc nmf-clean nmf-realclean nmf-conf-clean: 
	make -f make-lib $(@:nmf-%=%)
ifndef NO_NMF_BUILD
ifneq ($(PLATFORM),x86-linux)
	make -f Make_mpc.nmf $(@:nmf-%=%)
	make -f Make_dual.nmf $(@:nmf-%=%)
endif
	make -f Make_arm.nmf $(@:nmf-%=%)
endif

install:
	make -f make-lib install
ifneq ($(PLATFORM),x86-linux)	
	make -f Make_mpc.nmf install
	make -f Make_dual.nmf install
endif
	make -f Make_arm.nmf install

#include $(MMROOT)/video/components/Make.doc

