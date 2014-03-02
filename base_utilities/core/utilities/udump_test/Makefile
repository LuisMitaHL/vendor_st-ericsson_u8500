BINDIR=/opt/udump_test
WARNFLAGS=-Wall
CFLAGS?=-O2 -g ${WARNFLAGS}

dump_verifier: dump_verifier.o
	$(CC) dump_verifier.o  -o $@

build: dump_verifier

install: build
	mkdir -p ${STAGING_AREA}${BINDIR}
	cp dump_verifier ${STAGING_AREA}${BINDIR}
	cp dumpme ${STAGING_AREA}${BINDIR}
	@$(PACKAGE_DIR) /opt/udump_test 755 0 0
	@$(PACKAGE_FILE) /opt/udump_test/dump_verifier ${STAGING_AREA}${BINDIR}/dump_verifier 755 0 0
	@$(PACKAGE_FILE) /opt/udump_test/dumpme ${STAGING_AREA}${BINDIR}/dumpme 755 0 0

clean:
	rm -f ./dump_verifier.o ./dump_verifier

.PHONY: clean build install
