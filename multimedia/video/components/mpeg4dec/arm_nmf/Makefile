# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
MODULE:=..

all : nmf-all
clean : nmf-clean 
realclean : nmf-realclean nmf-conf-clean

nmf-all nmf-idl-doc nmf-clean nmf-realclean nmf-conf-clean: 
	make -C algo -f Make-lib $(@:nmf-%=%)
#ifndef NO_NMF_BUILD
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -f Make.nmf $(@:nmf-%=%)
endif 
endif
	make -f Make_using_arm.nmf $(@:nmf-%=%)
#endif

install:
	make -C algo -f Make-lib install
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -f Make.nmf install
endif
endif
	make -f Make_using_arm.nmf install

#include $(MMROOT)/video/components/Make.doc

