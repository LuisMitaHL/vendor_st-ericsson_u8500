#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

MMINSTALL_ARGS += -n shared_emptythisbuffer.elf \
                  -n shared_fillthisbuffer.elf \
                  -n eventhandler.elf \
                  -n fsminit.elf \
                  -n sendcommand.elf

MMINSTALL_ARGS += -n specific_frameinfo.idt.h \
                  -n fsmInit.idt.h \
                  -n trace.idt.h \
                  -n omxerror.idt.h \
                  -n omxcommand.idt.h \
                  -n omxstate.idt.h \
                  -n omxevent.idt.h \
                  -n omxeventext.idt.h

MMINSTALL_ARGS += -n shared_emptythisbuffer.hpp \
                  -n shared_fillthisbuffer.hpp \
                  -n sendcommand.hpp \
                  -n fsminit.hpp \
                  -n eventhandler.hpp

DEBUG_FLAG = `echo $(DEBUG_TRACES) | sed -e "s/1/-DDEBUG_TRACES/g"`
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
  COMPONENTS_ROOT = $(MMROOT)/video/components/
  CONF_ROOT = $(subst /,\/,$(COMPONENTS_ROOT)/$(dir $@)/)
  HAMAC_XBUS_REGS_FILE = "$(COMPONENTS_ROOT)/registers_def/$(SVA_SUBSYSTEM)/inc/t1xhv_hamac_xbus_regs.h"
else
  CONF_ROOT = $(subst /,\/,$(shell pwd)/)
  HAMAC_XBUS_REGS_FILE = "$(MM_HEADER_DIR)/$(SVA_SUBSYSTEM)/t1xhv_hamac_xbus_regs.h"
endif

%.conf: %.gconf
	@echo "  Pre-Proc. $(subst $(PRODUCT_TOP_DIR),,$(abspath $<))"
	@cpp -E $< 	\
		$(CONFPPFLAGS) 					\
		-DMODULE=$(MODULE) 				\
		-DSVA_SUBSYSTEM=$(SVA_SUBSYSTEM) 		\
		-D__$(SVA_SUBSYSTEM)__		 		\
		$(DEBUG_FLAG)					\
		-DHW_VERSION=hw_$(shell grep '#define HV_HW_REL' $(HAMAC_XBUS_REGS_FILE) | sed -e "s/.*0x//" | sed -e "s/U.*//")	\
		-DCOMPONENT_VERSION=version_none \
		| sed -e "s/CONF_ROOT/$(CONF_ROOT)/g" 			\
		| sed -e "s/SVA_SUBSYSTEM/$(SVA_SUBSYSTEM)/g" 		\
		| sed -e "s/-DDEBUG_TRACES/$(DEBUG_FLAG)/g"		\
		> ../$(MODULE)/$(@:%.conf=%.pp)
	@awk '! /^#/ { print $$0 }' ../$(MODULE)/$(@:%.conf=%.pp) > ../$(MODULE)/$@
	@rm -f ../$(MODULE)/$(@:%.conf=%.pp)

# Kept for legacy reasons - nothing to be done
conf-check:

conf-clean:
	rm -f $(addprefix $(MODULE)/../, $(CONFFILES))

realclean: conf-clean

.PHONY: conf-clean conf-check

