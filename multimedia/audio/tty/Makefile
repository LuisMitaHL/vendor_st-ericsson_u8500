# Default Config
CORE=arm
MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
VPATH+= include lib
LIBNAME = tty
INSTALL_LIB = libtty.a
INSTALL_HEADERDIR  = include
#WARNING todo target is MANDATORY (exe and/or lib)
todo: export lib
# Generic Rules
include $(MM_MAKEFILES_DIR)/SharedRules.mk
# local Rules
test cleantest:
	@echo no test
export:
	@if test ! -d  $(LIB_DIR) ; then mkdir -p $(LIB_DIR); echo ***Creating directory $(LIB_DIR) ; fi 
ifeq ($(MMPROCESSOR),cortexA9)
	@cp delivery/arm/*.a $(LIB_DIR)/
else ifeq ($(MMPROCESSOR),x86)
	@cp delivery/x86/*.a $(LIB_DIR)/
else
	@echo "ERROR => unsupported platform $(MMPLATFORM)"; exit 1;
endif
.PHONY: test
