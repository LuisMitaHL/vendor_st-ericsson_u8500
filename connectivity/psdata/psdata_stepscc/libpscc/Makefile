# Makefile for LIB PSCC
#
# How to:
#   - enable Code Coverage in build
#   make enable_gcov
#
#   - run module test (dejagnu required)
#   make check
#
#   - run memory check (Valgrind required)
#   make memcheck
#

-include .config

CC=gcc

ifdef USE_GCOV
GCOVFLAGS=-fprofile-arcs -ftest-coverage
GCOV_LIBS=-lgcov
endif


ifndef AR
AR=ar
endif

ifndef CFLAGS
CFLAGS=-Wall -Wextra -Wno-main -Werror -g -fPIC $(GCOVFLAGS)
endif

ifndef LDFLAGS
LDFLAGS = -shared $(GCOVFLAGS)
endif

ifndef LIBS
LIBS=
endif

ifndef LDO
LDO=$(CC)
endif

ifndef INSTALLDIR_PREFIX
INSTALLDIR_PREFIX=/usr/local
endif

ifndef INSTALL
INSTALL=install
endif

ALL=libpscc.so libpscc.a

all: verify_config $(ALL)

install: all
	$(INSTALL) $(HEADERS) $(INSTALLDIR_PREFIX)/include
	$(INSTALL) $(ALL) $(INSTALLDIR_PREFIX)/lib
	ldconfig

ifndef RM
RM=rm
endif

uninstall:
	@for FILE in $(HEADERS); do \
		$(RM) -f $(INSTALLDIR_PREFIX)/include/$$FILE; \
	done
	@for FILE in $(ALL); do \
		$(RM) -f $(INSTALLDIR_PREFIX)/lib/$$FILE; \
	done
	ldconfig

verify_config:
	@if [ ! -r .config ]; then \
		echo 'Missing .config file, creating a default empty one..'; \
		touch .config ; \
	fi

HEADERS = pscc_msg.h
OBJS = pscc_msg.o

libpscc.so: $(OBJS)
	$(LDO) $(LDFLAGS) -o libpscc.so $(OBJS) $(LIBS)

libpscc.a: $(OBJS)
	$(AR) rcs libpscc.a $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

enable_gcov:
	echo USE_GCOV=y >> .config

test: libpscc.a
	(cd $@; make)

check: clean all
ifdef VALGRIND
	(mkdir -p ./memcheck)
endif
ifdef USE_GCOV
	(mkdir -p ./coverage && lcov --directory . --zerocounters)
endif
	(mkdir -p ./module_test)
	(cd test && make check)
	(cd test && LD_LIBRARY_PATH=../ runtest --tool testapp --srcdir testsuite --outdir ../module_test VALGRIND="$(VALGRIND)")
ifdef USE_GCOV
	lcov --directory . --capture --output-file ./coverage/libpscc.info
	lcov --extract ./coverage/libpscc.info "*libpscc/*" --output-file ./coverage/libpscc.info.stripped
	genhtml --title "LIB STE PSCC module test" -o ./coverage ./coverage/libpscc.info.stripped
endif

memcheck: clean
	make check VALGRIND="valgrind --time-stamp=yes --log-file=../memcheck/valgrind_log.%p --leak-check=full --show-reachable=yes"

clean:
	rm -rf core *~ test/*~ *.o *.gcno *.gcda coverage memcheck module_test $(ALL)
