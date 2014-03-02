# vim:syntax=make:
# -*- Mode: Makefile -*-
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

all clean realclean install uninstall:
	@if test -d common ; then \
	   if $(MAKE) -C common CORE=$(MMPROCESSOR) $@ ; then :; else exit 1 ; fi ;\
	fi ;

ifeq ($(CORE_NUMBER),2)
	@if test -d mpc ; then \
	   if $(MAKE) -C mpc CORE=mmdsp $@ ; then :; else exit 1 ; fi ;\
	fi ;
endif
	@if test -d hst ; then \
	   if $(MAKE) -C hst CORE=$(MMPROCESSOR) $@ ; then :; else exit 1 ; fi ;\
	fi ;


test cleantest:
ifeq ($(CORE_NUMBER),2)	
	@if test -d mpc ; then \
	   if $(MAKE) -C mpc CORE=mmdsp $@ ; then :; else exit 1 ; fi ;\
	fi ;

else #Host only

ifeq ($(MMPLATFORM),x86)
	@echo "Test to be implemented ... CORE_NUMBER = $(CORE_NUMBER) MMPLATFORM = $(MMPLATFORM) "
	@if test -d hst ; then \
	   if $(MAKE) -C hst $@ ; then :; else exit 1 ; fi ;\
	fi ;
endif

endif

