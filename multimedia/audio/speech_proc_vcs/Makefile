# vim:syntax=make:
# -*- Mode: Makefile -*-
CORE=arm
MMSYSTEM=x
MMBOARD=x

ifeq ($(DEBUG),1)
CPPFLAGS=-g
endif

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
all clean realclean install unistall:		
	$(MAKE) -C vcs  $@ 
	$(MAKE) -C wrapper  $@ 
