#
#***************************************************************
#
# Synopsis: Hats Makefile- Linux Application Build Rules.
#
#***************************************************************
#



include .config.mk


export HATS_MODEM


HATS_MODEM= 1

# Main Hats variables
PACKAGE_NAME = hats

PARSER = hatsconf.pl
MAKEDEPS = $(HATS_OBJ)/hatsdeps.mk

KERNEL=$(HATS_INC)/kernel
LINUXTREE=$(HATS_VARLOCAL)
VERSION=$(HATS_SYSTEMFS)/etc/version

CONFIG_LIST = $(notdir $(wildcard configs/*_defconfig))

# Specific kconfig variables
SOURCE:=kconfigBin
HATS_PATH?=$(PWD)
export srctree=$(HATS_PATH)

HOST_SHELL = /bin/sh
HOST_CC = gcc
HOST_CFLAGS := -I$(HATS_PATH)/$(SOURCE) -Wmissing-prototypes -Wstrict-prototypes -O2 \
	-fomit-frame-pointer -DCURSES_LOC=\"<ncurses.h>\" -DLOCALE -c \
	-DCONFIG_='\"HATSCONF_\"' -DPACKAGE='\"hats\"'
HOST_BASH_SHELL = /bin/bash

.PHONY: all clean distclean $(VERSION) $(HATS_CONFIG)

# General and module specific rules
all: $(KERNEL) $(LINUXTREE) $(VERSION)
all clean: $(MAKEDEPS)
ifeq ($(module),)
	@$(ECHO) -e "\t-----> Hats : $@ in each modules"
	@$(MAKE) --no-print-directory -f $(MAKEDEPS) $@
else
	@$(ECHO) -e "\t-----> Hats : $@ in $(module) module(s)"
	@$(MAKE) --no-print-directory -f $(MAKEDEPS) $@ DIRS="$(module)"
endif

distclean: clean
	@$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	@if test -e $(HATS_OBJ); then $(RM) -rf $(HATS_OBJ); fi
	@if test -e $(HATS_SYM); then $(RM) -rf $(HATS_SYM); fi
	@if test -e $(HATS_INC); then $(RM) -rf $(HATS_INC); fi
	@if test -e $(HATS_SYSTEMFS); then $(RM) -rf $(HATS_SYSTEMFS); fi
	@if test -e $(HATS_ROOTFS); then $(RM) -rf $(HATS_ROOTFS); fi

# Parser rules
$(MAKEDEPS): $(HATS_CONFIG) $(PARSER)
	@$(ECHO) -e "\t-----> $(PARSER) : $@"
	@$(PERL) $(PARSER) $(HATS_CONFIG) $(MAKEDEPS)

# Check Config file rule
$(HATS_CONFIG):
	@if [ -f $(HATS_CONFIG) ]; then \
		$(MAKE) -s oldconfig; \
	else \
		$(MAKE) -s $(PRODUCT)_defconfig; \
	fi

# Kconfig rules
config menuconfig: $(HATS_CONFIG)
config %config:
	@$(MKDIR) -p $(HATS_OBJ)/$(SOURCE)/lxdialog
	@cd $(HATS_OBJ) && $(MAKE) -sf $(HATS_PATH)/$(SOURCE)/Makefile \
		CONFIG_SHELL=$(HOST_SHELL) HOSTCC=$(HOST_CC) CFLAGS="$(HOST_CFLAGS)" \
		srctree=$(HATS_PATH) src=$(SOURCE) objtree=$(HATS_OBJ) obj=$(SOURCE) \
		VPATH=$(HATS_PATH) KBUILD_KCONFIG=Kconfig KBUILD_DEFCONFIG=$(PRODUCT)_defconfig $@

# Help rule
help:
	@$(ECHO) -e " HATS Makefile help "
	@$(ECHO) -e "  all\t\t  - build all HATS, according to current configuration"
	@$(ECHO) -e "  \t\t\tif no config exists, will use defconfig"
	@$(ECHO) -e "  clean\t\t  - clean all HATS, according to current configuration"
	@$(ECHO) -e "  \t\t\tcurrent configuration is kept"
	@$(ECHO) -e "  distclean\t  - clean all HATS, according to current configuration"
	@$(ECHO) -e "  \t\t\tall configuration is erased"
	@$(ECHO) -e "  module=<module> - compile a specific module"
	@$(ECHO) -e "  module=\"<module1> <module2> ...\" - compile several specific modules"
	@$(ECHO) -e "  -jX\t\t  - compile HATS with X jobs in parallel."
	@$(ECHO) -e "  test_deps\t  - test all hats dependencies. Do not put any arguments."
	@$(ECHO) -e " HATS Kconfig help "
	@$(MAKE) -sf $(HATS_PATH)/$(SOURCE)/Makefile $@
	@$(ECHO) -e " HATS Available Configs"
	@$(ECHO) -e "  "$(CONFIG_LIST)

# Filesystem generation rules
$(LINUXTREE): $(HATS_CONFIG)
	@$(ECHO) -e "\t-----> Generate linuxTree : $@"
#	###############################################
#	Preparation of the Filesystem
#	###############################################
	@$(MKDIR) -p  $(HATS_OBJ)
	@$(MKDIR) -p  $(HATS_SYM)
	@$(MKDIR) -p  $(HATS_INC)
	@$(MKDIR) -p  $(HATS_SYSTEMFS)
	@$(MKDIR) -p  $(HATS_ROOTFS)
	@$(MKDIR) -p  $(HATS_MODEMFS)
	@$(MKDIR) -p  $(HATS_BIN)
	@$(MKDIR) -p  $(HATS_LOCALBIN)
	@$(MKDIR) -p  $(HATS_ETC)
	@$(MKDIR) -p  $(HATS_LIB)
	@$(MKDIR) -p  $(HATS_LOCALLIB)
	@$(MKDIR) -p  $(HATS_SHARE)
	@$(MKDIR) -p  $(HATS_VARLOCAL)
	@$(HOST_BASH_SHELL) PopulateTemplateFs $(HOST_BASH_SHELL) $(HATS_SYSTEMFS) $(TOOLCHAIN_TARGET_DIR) $(HATS_LOG_PATH) $(HATS_CONFIG) $(PRODUCT)


$(KERNEL):
	@$(ECHO) -e "\t-----> Generate kernel headers"
	@$(MAKE) V=0 -s -C $(HATS_KERNEL) INSTALL_HDR_PATH=$(HATS_INC)/kernel $(VARS) headers_install

$(VERSION): $(LINUXTREE)
	@$(ECHO) -e "\t-----> Generate version file"
	@$(ECHO) "compiled on $(shell hostname) by $(USER)" > $(HATS_SYSTEMFS)/etc/version
	@$(ECHO) "compilation date $(shell date)" >> $(HATS_SYSTEMFS)/etc/version

cscope:
	@$(ECHO) "Generating cscope database..."
	@$(RM) -f cscope.out
	@find -L . \( -name "*.[chxsS]" -o -name "*.cc" \) -print | sed -e 's/\(.*\)/\"\1\"/' > cscope.files
	@cscope -b -q -k -v

# Usage is for dependency test only !
# Directory will be distcleaned many times, and all modules will be tested one by one
# Do not call it in multijob, test rule will overwrite it.
TEST_PATERNS:= connectivityservices DthFramework generalservices \
	miniape modemservices multimediaservices powerservices tat tools
TEST_DIRS:= $(foreach d,$(TEST_PATERNS), \
	$(subst /, ,$(dir $(wildcard $(d)/*/Makefile))))
TEST_DIRS:= $(filter-out kconfigBin $(TEST_PATERNS), $(TEST_DIRS))

test_deps:
	@$(MKDIR) -p  logs
	@for i in $(TEST_DIRS); do \
		$(ECHO) -e "\t-----> testing dependencies to module = $$i"; \
		$(MAKE) -j16 distclean > logs/test_$$i.txt 2>&1; \
		$(MAKE) -j16 module=$$i >> logs/test_$$i.txt 2>&1; \
		if [ $$? -ne 0 ]; \
		then \
			$(ECHO) "Error found during $$i compilation."; \
			$(ECHO) "Please check logs/test_$$i.txt"; \
			exit; \
		fi; \
		$(ECHO) -e "\t<----- module $$i is compiled."; \
	done
