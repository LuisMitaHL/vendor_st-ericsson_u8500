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

ifndef LIBMPL_LDFLAGS
LIBMPL_LDFLAGS = -shared
endif

ifndef LIBMPL_CFLAGS
LIBMPL_CFLAGS=-Wall -Wextra -Wno-main -Werror -g -fPIC
endif

OBJS = mpl_list.o mpl_param.o mpl_msg.o mpl_config.o

ALL=libmpl.so libmpl.a

install: 
	@cp libmpl.so $(INSTALL_MOD_PATH)/lib/libmpl.so

uninstall: 
	@rm $(INSTALL_MOD_PATH)/lib/libmpl.so

build: libmpl.so libmpl.a

libmpl.a: $(OBJS)
	$(AR) rcs libmpl.a $(OBJS)

libmpl.so: $(OBJS)
	$(LDO) $(LIBMPL_LDFLAGS) -o libmpl.so $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(LIBMPL_CFLAGS) $<

clean:
	rm $(OBJS) libmpl.so libmpl.a
