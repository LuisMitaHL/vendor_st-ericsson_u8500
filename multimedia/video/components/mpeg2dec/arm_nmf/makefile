# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
MODULE:=..

all : nmf-all
clean : nmf-clean 
realclean : nmf-realclean nmf-conf-clean

#DUMMY:=$(shell mkdir ../../host)

nmf-all nmf-idl-doc nmf-clean nmf-realclean nmf-conf-clean: 
ifndef NO_NMF_BUILD
	make -f Make.nmf $(@:nmf-%=%)
	#make -C ../../.. -f mpeg2dec/mpc/ddep/Make.nmf $(@:nmf-%=%)
	#make -C ../../host -f ../mpc/ddep/Make.nmf.stubskel $(@:nmf-%=%)
endif

install:
	make -f Make.nmf install
	#make -C ../../.. -f jpegdec/mpc/ddep/Make.nmf install
	#make -C ../../host -f ../mpc/ddep/Make.nmf.stubskel install

#include $(MMROOT)/video/components/Make.doc

