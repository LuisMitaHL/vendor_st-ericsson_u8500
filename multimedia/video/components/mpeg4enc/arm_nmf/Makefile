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
	make -f Make.nmf $(@:nmf-%=%)
endif	

install:
	make -f make-lib install
	make -f Make.nmf install

#include $(MMROOT)/video/components/Make.doc

