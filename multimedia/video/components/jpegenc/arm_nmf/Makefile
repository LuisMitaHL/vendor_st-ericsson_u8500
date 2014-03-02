# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
MODULE:=..

all : nmf-all
clean : nmf-clean 
realclean : nmf-realclean 

#DUMMY:=$(shell mkdir ../../host)

nmf-all nmf-idl-doc nmf-clean nmf-realclean: 
ifndef NO_NMF_BUILD
ifeq ($(findstring 5500,$(PLATFORM)),5500)
	make -f Make_hva.nmf $(@:nmf-%=%)
endif	
	make -f Make.nmf $(@:nmf-%=%)
endif

install:
	make -f Make.nmf install
ifeq ($(findstring 5500,$(PLATFORM)),5500)	
	make -f Make_hva.nmf install	
endif	

#include $(MMROOT)/video/components/Make.doc

