
ifndef INSTALL_MOD_PATH
INSTALL_MOD_PATH=/usr/local/lib
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=
endif

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
LDO             = $(CC)
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

ifndef LIBPSCC_LDFLAGS
LIBPSCC_LDFLAGS = -shared
endif

ifndef LIBPSCC_CFLAGS
LIBPSCC_CFLAGS=-Wall -Wextra -Wno-main -Werror -g -fPIC -I../libmpl
endif

OBJS = pscc_msg.o

ALL=libpscc.so libpscc.a

install:
	@cp libpscc.so $(INSTALL_MOD_PATH)/lib/libpscc.so

uninstall:
	@rm $(INSTALL_MOD_PATH)/lib/libpscc.so

build: libpscc.so libpscc.a

libpscc.a: $(OBJS)
	$(AR) rcs libpscc.a $(OBJS)

libpscc.so: $(OBJS)
	$(LDO) $(LIBPSCC_LDFLAGS) -o libpscc.so $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(LIBPSCC_CFLAGS) $<

clean:
	rm $(OBJS) libpscc.so libpscc.a
