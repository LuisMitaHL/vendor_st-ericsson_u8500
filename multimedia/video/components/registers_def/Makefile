# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MM_MAKEFILES_DIR)/VideoStdConfig.mk

#INSTALL_HEADER = ${SVA_SUBSYSTEM}/inc/t1xhv_hamac_top_regs.h  ${SVA_SUBSYSTEM}/inc/t1xhv_hamac_xbus_regs.h

todo: lib
ifeq ($(findstring 500_a0,$(SVA_SUBSYSTEM)),500_a0)
	$(GEN_LN) -d $(COMPONENT_TOP_DIR)/$(SVA_SUBSYSTEM)/inc $(INSTALL_HEADER_DIR)/$(SVA_SUBSYSTEM)
endif

OBJ_DIR=
BIN_DIR=
LIB_DIR=
DEP_DIR=

all: todo

clean realclean:

include $(MM_MAKEFILES_DIR)/VideoStdRules.mk

