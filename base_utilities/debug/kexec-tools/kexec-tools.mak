PACKAGE_NAME=kexec-tools
PACKAGE_BUILD_ROOT=$(CURDIR)
OLD_TEMPDIR := $(shell grep '^prefix=' config.log | sed -e "s/^prefix='\(.*\)'/\1/")

# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

# Ok, this needs some explaining. kexec-tools should build for both
# Android and LBP. For LBP the compiler and the prefix directory is
# decided at configuration time, i.e. when the makefile is generated.
# Therefore, configure needs to be run as a part of the build; we
# cannot have different branches of the kexec-tools git for different
# toolchains...
#
# Also, we reconfigure when the old staging area cannot be found;
# configure looks for headers in there and generates a makefile
# referencing them (so they can be used by the compiler). Actually
# installing the tools there, the way it's done now, is probably
# overkill. They could be pulled directly from the build directory
# with PACKAGE_FILE.

build:
ifneq ($(OLD_TEMPDIR),$(STAGING_AREA))
	./configure --prefix=$(STAGING_AREA) --host=$(CROSS_PREFIX)
	$(MAKE) clean
	@echo "Configured $(PACKAGE_NAME)"
endif
	$(MAKE)
	@echo "Built $(PACKAGE_NAME)"

install: build
	$(MAKE) install
	# @$(STRIP_SYMBOLS) $(STAGING_AREA)/sbin/kexec
	@$(PACKAGE_DIR) /sbin 755 0 0
	@$(PACKAGE_FILE) /sbin/kexec $(STAGING_AREA)/sbin/kexec 755 0 0
	@echo "Install $(PACKAGE_NAME) done"

# Don't fail if cleaning before configure has been run
clean:
	-$(MAKE) clean
	-rm config.log

.PHONY: build install clean
