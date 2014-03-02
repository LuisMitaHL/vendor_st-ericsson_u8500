BINDIR=/sbin
WARNFLAGS=-Wall
CFLAGS?=-Os -g ${WARNFLAGS}

watchdog-kicker: kickdogd.o
	$(CC) kickdogd.o -o $@ -lpthread

build: watchdog-kicker

install: build
	cp watchdog-kicker ${STAGING_AREA}${BINDIR}
	@$(PACKAGE_FILE) /sbin/watchdog-kicker ${STAGING_AREA}${BINDIR}/watchdog-kicker 755 0 0

clean:
	rm -f ./kickdogd.o ./watchdog-kicker

.PHONY: clean build install
