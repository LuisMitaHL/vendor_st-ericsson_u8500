DIRS= libmpl ../psdata_stepscc/libpscc ../../../../../validation/module/connectivity/psdata/psdata_module_test/testapps ../psdata_stepscc/stepscc

all:
	@echo "Available targets: install, uninstall, check, clean"

install:
	(cd ../../common/libstecom && make install)
	(cd ../ste_cli && make install)
	@for DIR in $(DIRS); do \
		(cd $$DIR; make install) \
	done

uninstall:
	(cd ../../common/libstecom && make uninstall)
	(cd ../ste_cli && make uninstall)
	@for DIR in $(DIRS); do \
		(cd $$DIR; make uninstall) \
	done

check: clean install
	@for DIR in $(DIRS); do \
		(cd $$DIR; make check) \
	done

clean:
	(cd ../ste_cli && make clean)
	(cd ../../common/libstecom && make clean)
	@for DIR in $(DIRS); do \
		(cd $$DIR; make clean) \
	done
