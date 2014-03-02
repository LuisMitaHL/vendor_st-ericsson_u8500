# Makefile for libstecom
#
# Copyright (C) ST-Ericsson AB 2010.
#

CC ?= gcc
MAKE ?= make
LDO ?= $(CC)

RM_F ?= rm -f
MKDIR ?= mkdir
INSTALL ?= install

LCOV ?= lcov
GENHTML ?= genhtml

ifndef CFLAGS
CFLAGS := -O2 -Wall -Werror -g -fPIC
else
CFLAGS += -fPIC
endif

ifndef INSTALLDIR_PREFIX
INSTALLDIR_PREFIX=/usr/local
endif



SUBDIRS = test

ALL=libstecom.so libstecom.a

HEADERS=libstecom.h

all: verify_config $(ALL)
	for dir in ${SUBDIRS}; do ( cd $$dir; $(MAKE) all); done

install: all
	$(INSTALL) $(HEADERS) $(INSTALLDIR_PREFIX)/include
	$(INSTALL) $(ALL) $(INSTALLDIR_PREFIX)/lib
	ldconfig

uninstall:
	@for FILE in $(HEADERS); do \
		$(RM_F) $(INSTALLDIR_PREFIX)/include/$$FILE; \
	done
	@for FILE in $(ALL); do \
		$(RM_F) $(INSTALLDIR_PREFIX)/lib/$$FILE; \
	done
	ldconfig

verify_config:
	@if [ ! -r .config ]; then \
		echo 'Missing .config file, creating a default empty one..'; \
		touch .config ; \
	fi

enable_nl80211testmode:
	echo CONFIG_NL80211_TESTMODE=y >> .config

enable_moduletest:
	echo CONFIG_MODULETEST=y >> .config

OBJS = stecom.o subscribe.o

LDFLAGS += -shared

-include .config

ifdef CONFIG_NL80211_TESTMODE
CFLAGS += -DCONFIG_NL80211_TESTMODE
OBJS += nl80211testmode.o
LIBS += -lnl

ifdef CONFIG_LIBNL20
CFLAGS += -DCONFIG_LIBNL20
endif

ifdef NETLINK_DIR
INCLUDES += -I$(NETLINK_DIR)/include
LIBS += -L$(NETLINK_DIR)/lib
endif

KLIB := /lib/modules/$(shell uname -r)
KLIB_BUILD ?= $(KLIB)/build
INCLUDES += -I$(KLIB_BUILD)/include
endif

ifdef CONFIG_MODULETEST
CFLAGS += -DCONFIG_MODULETEST
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
GCOV_LIBS = -lgcov
endif

-include $(OBJS:%.o=%.d)


ifdef CONFIG_MODULETEST
ifdef  RUN_TEST
CFLAGS += $(GCOV_FLAGS)
LIBS += $(GCOV_LIBS)
endif
memcheck: dist-clean
	$(MAKE) check VALGRIND="valgrind -q --time-stamp=yes --log-file=./memcheck/log.%p"

check: dist-clean
	$(MAKE) libstecom.a RUN_TEST=true
	$(MAKE) -C test RUN_TEST=true
	($(MKDIR) -p ./coverage && $(LCOV) --directory . --zerocounters)
	($(MKDIR) -p module_test/results)
	($(MKDIR) -p memcheck)
	LD_LIBRARY_PATH=../libstecom runtest --tool libstecom --srcdir module_test --outdir module_test/results VALGRIND="$(VALGRIND)"
	$(LCOV) --directory . --capture --output-file ./coverage/libstecom.info
	$(LCOV) --extract ./coverage/libstecom.info "*libstecom/*" --output-file ./coverage/libstecom.info.stripped
	$(GENHTML) --title "libstecom module test" -o ./coverage ./coverage/libstecom.info.stripped
else
check:
	echo "Module test is not enabled!"

memcheck:
	echo "Module test is not enabled!"
endif

libstecom.so: $(OBJS)
	$(LDO) $(LDFLAGS) -Wl,-soname,libstecom.so -o libstecom.so $(OBJS) $(LIBS)

libstecom.a: $(OBJS)
	$(AR) rcs libstecom.a $(OBJS)

%.o: %.c
	$(CC) $(INCLUDES) -MD -c -o $@ $(CFLAGS) $<

clean:
	$(RM_F) core *~ *.o *.d $(ALL)
	for dir in ${SUBDIRS}; do ( cd $$dir; $(MAKE) clean ); done

dist-clean: clean
	$(RM_F) -r coverage module_test/results memcheck

