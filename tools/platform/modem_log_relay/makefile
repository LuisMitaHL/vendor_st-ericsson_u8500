PREFIX?=/
CC?=gcc

MALPATH = ../../../access_services/modem_adaptations/mal/modem_lib/mal

MALINCLUDE = -I$(MALPATH)/libphonet/include -I$(MALPATH)/libmalmon/include -I$(MALPATH)/mal_util/include

MALINCLUDE += -I$(MALPATH)/../../../../../access_services/call_network/call_network/libcn/include

CFLAGS+=-O -Wall -g -DTSTU -DSPLIT_TRACE_FILES
CFLAGS+= $(MALINCLUDE)


LDFLAGS+=

BINARIES=modem_log_relay 

#-----------------------------------------------------------------------

all: $(BINARIES)

install:
#	mkdir -p $(PREFIX)/sbin
#	install -m 755 $(BINARIES) $(PREFIX)/sbin
#	mkdir -p $(PREFIX)/sbin
#	install -m 755 $(BINARIES) $(PREFIX)/sbin

clean:
	rm -f $(BINARIES) *.o

modem_log_relay: modemlogrelay.o utils.o dgramsk.o libmon.o libmlr.o coredump.o kerneldump.o xfiledecoder.o memfile.o
	$(CC) $^ $(LDFLAGS) -o $@ $(LIB) -lpthread

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(@)
