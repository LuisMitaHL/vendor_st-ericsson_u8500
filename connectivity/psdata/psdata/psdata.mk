
ifndef INSTALL_MOD_PATH
INSTALL_MOD_PATH=/usr/local/lib
endif

DIRS= libmpl libpscc ../psdata_stepscc ../psdata_module_test/testapps/psdata-test-apps

all:
	@echo "Available targets: build, install, uninstall, clean"

build:
	@for DIR in $(DIRS); do \
		(cd $$DIR; make $(PSDATA_FLAGS) -f $$DIR.mk build) \
	done

install:
	@for DIR in $(DIRS); do \
		(cd $$DIR; make INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) -f $$DIR.mk install) \
	done

uninstall:


clean:
	@for DIR in $(DIRS); do \
		(cd $$DIR; make -f $$DIR.mk clean) \
	done
