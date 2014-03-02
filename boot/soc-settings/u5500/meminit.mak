
#Source location
PRIVATE_SOCSETTINGS_DIR := $(shell pwd)

PRIVATE_SOCSETTINGS_FLAGS := PREFIX=$(PREFIX)

build-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) postbuild
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) U5500_TYPE=C postbuild

install-socsettings: build-socsettings
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) install
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) U5500_TYPE=C install

clean-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) clean
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) U5500_TYPE=C clean

distclean-socsettings:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) clean
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_FLAGS) U5500_TYPE=C clean

clean clobber: clean-socsettings

st-ericsson-flashkit: build-socsettings install-socsettings

.PHONY: config

config:

