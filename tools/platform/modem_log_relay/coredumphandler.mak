PACKAGE_NAME=coredumphandler

CFLAGS+=-O -Wall -g

BINARIES=coredump-handler

build: $(BINARIES)

install: build
	@$(PACKAGE_FILE) /usr/bin/coredump-handler $(CURDIR)/coredump-handler 755 0 0
	# Package coredump-handler startup script
	@$(PACKAGE_FILE) /etc/init.d/coredump-handler $(CURDIR)/rc.coredumphandler 755 0 0
	@$(PACKAGE_LINK) /etc/rc.d/S99_coredumphandler ../init.d/coredump-handler 755 0 0

clean:
	rm -f $(BINARIES) core *.o

coredump-handler: utils.o kerneldump.o coredumphandler.o
	$(CC) $^ $(LDFLAGS) -o $@ $(LIB)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(@)
