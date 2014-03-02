#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

#-------------------------------
# Warning DIRECTORIES variable has to be defined
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

ifndef DIRECTORIES
    $(error "Variable DIRECTORIES not set!") 
endif
ifeq ($(MAKECMDGOALS),)
MAKECMDGOALS=all
endif

ifdef NO_MMDSP
NO_X86_MMDSP=1
endif

MKFLAGS = $(foreach tmp, $(MAKEFLAGS), $(addsuffix \",$(addprefix \",$(shell echo $(tmp) | grep =))))
$(MAKECMDGOALS):
ifeq ($(MMPROCESSOR),x86_cortexA9)
  ifndef NO_X86_CORTEXA9
	@for dir in $(DIRECTORIES); do \
	if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	then :; else exit 1 ; fi ; fi;\
	done
  endif
else
  ifeq ($(MMPROCESSOR),x86_mmdsp)
    ifndef NO_X86_MMDSP
	@for dir in $(DIRECTORIES); do \
	if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	then :; else exit 1 ; fi ; fi;\
	done
    endif     
  else
    ifeq ($(MMPROCESSOR),x86)
      ifndef NO_X86
	@for dir in $(DIRECTORIES); do \
	if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	then :; else exit 1 ; fi ; fi;\
	done
      endif
    else
      ifeq ($(CORE),mmdsp)
        ifndef NO_MMDSP
	  @for dir in $(DIRECTORIES); do \
	  if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	  then :; else exit 1 ; fi ; fi;\
	  done
        endif
      else
        ifeq ($(findstring arm,$(MMPROCESSOR)),arm)
          ifndef NO_ARM11
	    @for dir in $(DIRECTORIES); do \
	    if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	    then :; else exit 1 ; fi ; fi;\
	    done
          endif
        else
          ifeq ($(MMPROCESSOR),cortexA9)
            ifndef NO_CA9
	      @for dir in $(DIRECTORIES); do \
	      if test -e $$dir/Makefile ; then if $(MAKE) -C $$dir $(MKFLAGS) $@ ;\
	      then :; else exit 1 ; fi ; fi;\
	      done
            endif
          endif
        endif
      endif
    endif
  endif
endif	
# x86_cortexA9-linux END
.PHONY: $(MAKECMDGOALS)
