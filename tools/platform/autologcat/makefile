PREFIX?=/
CC?=gcc

CFLAGS+=-O -Wall -g

LDFLAGS+=

BINARIES=autologcat 

#-----------------------------------------------------------------------

all: $(BINARIES)

install:
#	mkdir -p $(PREFIX)/sbin
#	install -m 755 $(BINARIES) $(PREFIX)/sbin
#	mkdir -p $(PREFIX)/sbin
#	install -m 755 $(BINARIES) $(PREFIX)/sbin

clean:
	rm -f $(BINARIES) *.o

autologcat: autologcat.o
	$(CC) $^ $(LDFLAGS) -o $@ $(LIB)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(@)
