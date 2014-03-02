include .config.mk

SYSNAME:=${shell uname}
SYSNAME!=uname
HFILES=../include/spfs.h ../include/spclient.h spcimpl.h
CFLAGS=-Wall -g -I ../include

LIBFILES=\
	dirread.o\
	fid.o\
	fsys.o\
	mount.o\
	netmount.o\
	open.o\
	pool.o\
	poll.o\
	read.o\
	readnb.o\
	remove.o\
	stat.o\
	stat2wstat.o\
	walk.o\
	write.o\
	writenb.o\

libspclient.a: $(LIBFILES)
	ar rc libspclient.a $(LIBFILES)
	ranlib libspclient.a

install:
	cp ../include/spclient.h /usr/local/include
	cp libspclient.a /usr/local/lib

clean:
	rm -f *.o *.a *~
	make -C examples clean

$(LIBFILES): Makefile $(HFILES)

%.o: %.c 
	$(CC) $(CFLAGS) -c $*.c
