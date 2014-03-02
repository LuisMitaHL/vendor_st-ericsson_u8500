#CROSS_COMPILE=arm-none-linux-gnueabi-
CROSS_COMPILE=
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
CFLAGS=-g -Wall -I../.
LDFLAGS=
LIBS=../libmpl.a

PROG=example

example_main: example_main.o example_msg.o
	$(CC) $(LDFLAGS) -o example example_main.o example_msg.o $(LIBS)

all: $(PROG)

clean:
	rm -rf $(PROG) *.o *.sum *.log
