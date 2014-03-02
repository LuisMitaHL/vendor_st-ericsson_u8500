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

ifdef USE_GCOV
GCOVFLAGS=-fprofile-arcs -ftest-coverage
GCOV_LIBS=-lgcov
endif

ifndef CC
CC=gcc
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

ALL=libmpl.so libmpl.a

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

HEADERS= mpl_list.h mpl_param.h mpl_msg.h mpl_config.h
OBJS = mpl_list.o mpl_param.o mpl_msg.o mpl_config.o

libmpl.so: $(OBJS)
	$(LDO) $(LDFLAGS) -o libmpl.so $(OBJS) $(LIBS)

libmpl.a: $(OBJS)
	$(AR) rcs libmpl.a $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

enable_gcov:
	echo USE_GCOV=y >> .config

test: libmpl.a
	(cd $@; make)

check: clean all
ifdef VALGRIND
	(mkdir -p ./memcheck)
endif 
ifdef USE_GCOV
	(mkdir -p ./coverage && lcov --directory . --zerocounters)
endif
	(mkdir -p ./module_test)
	(cd ../../../../../../validation/module/connectivity/psdata/psdata_module_test/libmpl && make check)
	(cd ../../../../../../validation/module/connectivity/psdata/psdata_module_test/libmpl && LD_LIBRARY_PATH=../ runtest --tool testapp --srcdir testsuite --outdir ../../../../../../connectivity/psdata/psdata/libmpl/module_test VALGRIND="$(VALGRIND)")
ifdef USE_GCOV
	lcov --directory . --capture --output-file ./coverage/libmpl.info
	lcov --extract ./coverage/libmpl.info "*libmpl/*" --output-file ./coverage/libmpl.info.stripped
	genhtml --title "LIB MPL module test" -o ./coverage ./coverage/libmpl.info.stripped
endif

memcheck: clean
	make check VALGRIND="valgrind --time-stamp=yes --log-file=../memcheck/valgrind_log.%p --leak-check=full --show-reachable=yes" 

clean:
	rm -rf core *~ ../../../../../../validation/module/connectivity/psdata/psdata_module_test/libmpl/*~ *.o *.gcno *.gcda coverage memcheck module_test $(ALL)
	(cd ../../../../../../validation/module/connectivity/psdata/psdata_module_test/libmpl; make clean)
