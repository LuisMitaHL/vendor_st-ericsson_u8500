ifndef TARGET
#TARGET=native
endif

SHELL= /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

prefix ?= /usr/local
sbindir ?= ${prefix}/sbin
bindir ?= ${prefix}/bin
libdir ?= ${prefix}/lib
sysconfdir ?= ${prefix}/etc
includedir ?= $(prefix)/include

use_echo=@
V ?= 0
ifneq ($(V), 0)
$(info Building with verbose information.)
use_echo=
endif

INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m 644

ifeq ($(TARGET),native)
FB_LX=$(HOME)/fairbanks_lx
CC=$(FB_LX)/android/prebuilt/linux-x86/toolchain/arm-eabi-a9-4.3.2/bin/arm-eabi-gcc
CPPFLAGS+= --sysroot=$(FB_LX)/android
CPPFLAGS+= -isystem =/bionic/libc/kernel/common
CPPFLAGS+= -isystem =/bionic/libc/include
CPPFLAGS+= -isystem =/bionic/libc/arch-arm/include
CPPFLAGS+= -isystem =/bionic/libc/kernel/arch-arm
CPPFLAGS+= -I$(FB_LX)/android/external/openssl/include

ifndef DEBUG
CFLAGS+= -mthumb
endif

else
# TODO find out how to compile natively against bionic (Android libc)
CC=gcc
CPPFLAGS+= -D_GNU_SOURCE -fPIC
endif


ifdef WARNS
ifndef NOWERROR
CFLAGS_WARNS+= -Werror
endif
CFLAGS_WARNS_1+= -Wall
CFLAGS_WARNS_2+= $(CFLAGS_WARNS_1)
#CFLAGS_WARNS_2+= -Wstrict-prototypes -Wold-style-definition
#CFLAGS_WARNS_2+= -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith
#CFLAGS_WARNS_2+= -Wcast-qual -Wwrite-strings -Wcast-align -Wshadow
CFLAGS_WARNS_2+= $(CFLAGS_HIGH) $(CFLAGS_MEDIUM) $(CFLAGS_LOW)
CFLAGS_WARNS_3+= $(CFLAGS_WARNS_2)

# From swadvice #675 (removed -Werror as it's handled differently here)
CFLAGS_HIGH = -Wall -Wbad-function-cast -Wcast-align \
              -Werror-implicit-function-declaration -Wextra -Wfloat-equal \
              -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
              -Wmissing-declarations -Wmissing-format-attribute \
              -Wmissing-include-dirs -Wmissing-noreturn \
              -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
              -Wshadow -Wstrict-prototypes -Wswitch-default \
              -Wunsafe-loop-optimizations -Wwrite-strings

#-Wunreachable-code is removed as it generates an error in cops_mac
# when compiling with -O2

# From swadvice #675 (removed -Wswitch-enum it's harmful /EJENWIK )
CFLAGS_MEDIUM = -Waggregate-return -Wlarger-than-65500 -Wredundant-decls \
                -fno-common

# From swadvice #675 (removed -Wpacked)
CFLAGS_LOW = -Winline  -Wold-style-definition -Wstrict-aliasing=2 -Wundef \
             -pedantic -std=c99 -Wno-format-zero-length \
             -fdata-sections -ffunction-sections

CFLAGS_WARNS+= $(CFLAGS_WARNS_$(WARNS))

endif #ifdef WARNS

ifdef DEBUG
CFLAGS+= -g
CPPFLAGS+= -DCOPS_DEBUG=$(DEBUG)
else
CFLAGS+= -O2
endif

CFLAGS+= -pipe

ifdef EXTENDED_SIPC_MSGS
CFLAGS+= -DEXTENDED_SIPC_MSGS
endif

ifdef COPS_ROOT
COPS_SHARED= $(COPS_ROOT)/shared/ta
COPS_API= $(COPS_ROOT)/cops-api
COPS_SERVICES= $(COPS_ROOT)/services
INSTALL_DIR?=$(COPS_ROOT)/bin
endif #COPS_ROOT

ifdef USE_COPS_SHARED
include $(COPS_SHARED)/srcs_incs.mk
endif #USE_COPS_SHARED

ifdef USE_COPS_API
include $(COPS_API)/incs_lib.mk
endif #USE_COPS_API

ifdef USE_LTP
COPS_LTP= ${COPS_ROOT}/ltp/ltp-full
LDADD+= -lltp -L${COPS_LTP}/lib
CPPFLAGS+= -I${COPS_LTP}/include
endif

DEPS += $(SRCS:.c=.d)

# the @echo > /dev/null will supress "nothing to be done for `all`
.PHONY: all
all:
	@echo > /dev/null

ifdef LIB
LIBNAME=lib$(LIB).so

all: $(LIBNAME)

CLEANFILES += core $(LIBNAME) $(OBJS) $(DEPS)

OBJS += $(SRCS:.c=.o)

$(LIBNAME): $(OBJS)
ifeq ($(V), 0)
	@echo "  LD      $@"
endif
	@rm -f $@
	$(use_echo)$(CC) $(LDFLAGS) -shared -Wl,-x  \
                -o $@ -Wl,-soname=$(LIBNAME) \
                $(OBJS) $(LDADD)

%.o: %.c
ifeq ($(V), 0)
	@echo "  CC      $<"
endif
	$(use_echo)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(CFLAGS_WARNS) -o $@ $<

ifndef IGNORE_DEFAULT_INSTALL
.PHONY: install
install: $(LIBNAME)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL_PROGRAM) $(LIBNAME) $(DESTDIR)$(libdir)/$(LIBNAME)
endif
endif #LIB

ifdef LIB_A
LIBNAME=lib$(LIB_A).a

all: $(LIBNAME)

CLEANFILES += core $(LIBNAME) $(OBJS) $(DEPS)

OBJS += $(SRCS:.c=.o)

$(LIBNAME): $(OBJS)
	@echo building static library $(LIBNAME)
	@rm -f $@
	ar rcs $(LIBNAME) $(OBJS) $(LINKED_LIBS)

%.o: %.c
ifeq ($(V), 0)
	@echo "  CC      $<"
endif
	$(use_echo)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(CFLAGS_WARNS) -o $@ $<

.PHONY: install
install: $(LIBNAME)
	@mkdir -p $(libdir)
	$(INSTALL_PROGRAM) $(LIBNAME) $(libdir)/$(LIBNAME)

endif #LIB_A

ifdef PROG
all: $(PROG)

CLEANFILES += core $(PROG) $(OBJS) $(DEPS)

OBJS += $(SRCS:.c=.o)

$(PROG): $(OBJS)
ifeq ($(V), 0)
	@echo "  LD      $@"
endif
	$(use_echo)$(CC) $(LDFLAGS) -o $(PROG) $(OBJS) $(LDADD) -Wl,--gc-sections

%.o: %.c
ifeq ($(V), 0)
	@echo "  CC      $<"
endif
	$(use_echo)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(CFLAGS_WARNS) -o $@ $<

ifndef IGNORE_DEFAULT_INSTALL
.PHONY: install
install: $(PROG)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(PROG) $(DESTDIR)$(bindir)/$(PROG)
endif

endif #PROG

%.d: %.c
	@(set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@);\
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif


.PHONY: clean
clean:
ifdef CLEANFILES
	@rm -f $(CLEANFILES)
endif #CLEANFILES

ifdef DIRS

all: recursive_all

.PHONY: recursive_all
recursive_all:
	@set -e; \
        for d in $(DIRS); do \
		$(MAKE) -C $$d all; \
	done

clean: recursive_clean clean-htmldocs

.PHONY: recursive_clean
recursive_clean:
	@set -e; \
	for d in $(DIRS); do \
		$(MAKE) -C $$d clean; \
	done

.PHONY: clean-htmldocs
clean-htmldocs:
	@rm -fr html/

.PHONY: clean-cscope
clean-cscope:
	@rm -rf cscope.*

.PHONY: clean-git
clean-git:
	@git clean -f -X

.PHONY: distclean
distclean: clean clean-cscope clean-git

.PHONY: install
install: recursive_install

.PHONY: recursive_install
recursive_install:
	@set -e; \
	for d in $(DIRS); do \
		$(MAKE) -C $$d install; \
	done

endif #DIRS

#Astyle

astyle=$(COPS_ROOT)/tools/astyle

.PHONY: apply-format
apply-format:
	 @$(astyle) \
		 --style=k/r \
		 --indent=spaces \
		 --break-blocks \
		 --convert-tabs \
		 --add-brackets \
		 --unpad-paren \
		 --pad-header \
		 --pad-oper \
		 --indent-col1-comments \
		 --align-pointer=name \
		 --exclude=cops-api/include/queue.h \
		 --suffix=none \
		 -R "*.h" \
		 -R "*.c" \
		 -R "*.cpp"

.PHONY: cscope
cscope:
	@echo Create/update cscope database ...
	@find `pwd`/ -name "*.[ch]" > cscope.files && cscope -b -q -k
