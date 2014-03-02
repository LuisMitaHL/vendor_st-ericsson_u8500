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

ALL=libnlcom.so libnlcom.a

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

HEADERS = nlcom.h
OBJS = nlcom.o

libnlcom.so: $(OBJS)
	$(LDO) $(LDFLAGS) -o libnlcom.so $(OBJS) $(LIBS)

libnlcom.a: $(OBJS)
	$(AR) rcs libnlcom.a $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

enable_gcov:
	echo USE_GCOV=y >> .config

test: libnlcom.a
	(cd $@; make)

check:
#check: clean all
#ifdef VALGRIND
#	(mkdir -p ./memcheck)
#endif 
#ifdef USE_GCOV
#	(mkdir -p ./coverage && lcov --directory . --zerocounters)
#endif
#	(mkdir -p ./module_test)
#	(cd test && make check)
#	(cd test && LD_LIBRARY_PATH=../ runtest --tool testapp --srcdir testsuite --outdir ../module_test VALGRIND="$(VALGRIND)")
#ifdef USE_GCOV
#	lcov --directory . --capture --output-file ./coverage/libnlcom.info
#	lcov --extract ./coverage/libnlcom.info "*libnlcom/*" --output-file ./coverage/libnlcom.info.stripped
#	genhtml --title "LIB NETLINK COM module test" -o ./coverage ./coverage/libnlcom.info.stripped
#endif
#
#memcheck: clean
#	make check VALGRIND="valgrind --time-stamp=yes --log-file=../memcheck/valgrind_log.%p --leak-check=full --show-reachable=yes" 
#
clean:
	rm -rf core *~ test/*~ *.o *.gcno *.gcda coverage memcheck module_test $(ALL)
