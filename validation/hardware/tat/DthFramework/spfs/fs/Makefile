include .config.mk

SYSNAME:=${shell uname}
SYSNAME!=uname
CFLAGS=-Wall -Werror -g -I. -I ../include -I ../../include -DSYSNAME=$(SYSNAME) -DUSE_EXTERN_FUNCTION=\"libdthsrvhelper.so\"
LFLAGS=-L../libspfs -lspfs -ldl

FS=9pserver

all: 9pserver

ufs: ufs.o
	$(CC) -o ufs $(CFLAGS) ufs.o $(LFLAGS) $(NPFS_LFLAGS)

9pserver: 9pserver.o
	$(CC) -o 9pserver $(CFLAGS) 9pserver.o $(LFLAGS) $(NPFS_LFLAGS)

clean:
	rm -f *.o *~ $(FS) core.*

%.c: ../include/spfs.h Makefile

$(FS): ../libspfs/libspfs.a

%.o: %.c 
	$(CC) $(CFLAGS) -c $*.c

