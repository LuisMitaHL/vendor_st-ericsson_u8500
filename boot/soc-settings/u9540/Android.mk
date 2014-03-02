ifeq ($(SOC_SETTINGS_SET_PLATFORM), u9540)

include $(CLEAR_VARS)


.PHONY: build-socsettings install-socsettings clean-socsettings distclean-socsettings

#Source location
PRIVATE_SOCSETTINGS_DIR := $(abspath $(BOOT_PATH)/soc-settings/u9540)

#Output
PRIVATE_SOCSETTINGS_OUT := $(FLASHKIT_INSTALL_BASE)/

#Other flags
PRIVATE_SOCSETTINGS_PREFIX := $(PRIVATE_SOCSETTINGS_OUT)/

PRIVATE_SOCSETTINGS_FLAGS := PREFIX=$(PRIVATE_SOCSETTINGS_PREFIX)

build-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) postbuild

install-socsettings: build-socsettings
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) install

clean-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) clean

distclean-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) distclean

clean clobber: clean-socsettings

st-ericsson-flashkit: build-socsettings install-socsettings

endif
