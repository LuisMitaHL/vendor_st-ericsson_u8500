#
# Makefile for PS Connection Control
#
# How to: 
#   - enable Code Coverage in build 
#   make enable_gcov 
#
#   - run module test (Dejagnu required)
#   make check
#
#   - run memory check (Valgrind required)
#   make memcheck 
#
#CROSS_COMPILE=arm-none-linux-gnueabi-
#
ifndef CROSS_COMPILE
CROSS_COMPILE=
endif
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

-include .config

ACCESS_COMMONFUNC_DIR= ../../../../access_services/common/common_functionality

ifdef USE_GCOV
GCOV_FLAGS=-fprofile-arcs -ftest-coverage
GCOV_LIBS=-lgcov
endif

ifndef GCOV_OPT
GCOV_OPT="--zerocounters"
endif

ifdef USE_MAL
MODEM_OBJS=pscc_bearer_mal.o pscc_qpc_mal.o
MODEM_LIBS=-lmalgpds
MODEM_CFLAGS=-DPSCC_USE_MAL
else
MODEM_OBJS= pscc_pco.o pscc_bearer_mfl.o pscc_netdev_caif.o
MODEM_LIBS=-lmfl_cas -lmfl -lnlcom
MODEM_CFLAGS=-DPSCC_USE_MFL
endif

ifndef LDO
LDO=$(CC)
endif

ifndef LDFLAGS
LDFLAGS=$(GCOV_FLAGS)
endif
# When building with ubuntu kernel in6_addr is defined both in linux/in6.h and netinet/in.h so we add last flag to avoid redefinitions
CFLAGS=-g -Wall -Wextra -Werror=div-by-zero $(GCOV_FLAGS) -DPSCC_MODULE_TEST_ENABLED -D_LINUX_IN6_H -I$(ACCESS_COMMONFUNC_DIR)/include
LIBS=-lstecom -lmpl -lpscc -lcrypto $(MODEM_LIBS) $(GCOV_LIBS)

PROG=psccd
OBJS = pscc_handler.o psccd.o pscc_object.o pscc_runscript.o pscc_utils.o pscc_nwifcfg.o pscc_sim.o $(MODEM_OBJS) $(ACCESS_COMMONFUNC_DIR)/security/src/util_file.o $(ACCESS_COMMONFUNC_DIR)/security/src/util_user.o $(ACCESS_COMMONFUNC_DIR)/log/src/util_log.o

ifndef INSTALLDIR_PREFIX
INSTALLDIR_PREFIX=/usr/local
endif

ifndef INSTALL
INSTALL=install
endif

ifndef RM
RM=rm
endif

all: $(PROG)

install: all
	$(INSTALL) -m 4755 $(PROG) $(INSTALLDIR_PREFIX)/bin

uninstall:
	$(RM) -f $(INSTALLDIR_PREFIX)/bin/$(PROG)

psccd: $(OBJS)
	$(CC) $(LDFLAGS) -o psccd $(OBJS) $(LIBS)

enable_gcov:
	echo USE_GCOV=y >> .config

check: clean
	make check_mfl GCOV_OPT="--zerocounters"
	make check_mal GCOV_OPT="--capture"

memcheck: clean
	make check_mfl VALGRIND="valgrind --time-stamp=yes --log-file=memcheck/valgrind_log.%p --leak-check=full --show-reachable=yes"
	make check_mal VALGRIND="valgrind --time-stamp=yes --log-file=memcheck/valgrind_log.%p --leak-check=full --show-reachable=yes"

check_mfl: all
	(mkdir -p /data/local/tmp)
	(cd ../../ste_cli; make)
ifdef VALGRIND
	(mkdir -p ./memcheck)
endif
ifdef USE_GCOV
	(mkdir -p ./coverage && lcov --directory . $(GCOV_OPT))
endif
	(mkdir -p ./module_test)
	runtest --tool pscc --srcdir ../../../../../../validation/module/connectivity/psdata/psdata_module_test/stepscc/testsuite/mfl --outdir ./module_test VALGRIND="$(VALGRIND)"
ifdef USE_GCOV
	lcov --directory . --capture --output-file ./coverage/pscc.info
	lcov --extract ./coverage/pscc.info "*pscc/*" --output-file ./coverage/pscc.info.stripped
	genhtml --title "STE PS CC module test" -o ./coverage ./coverage/pscc.info.stripped
endif

check_mal:
	(mkdir -p /data/local/tmp)
	make all USE_MAL=y
	(cd ../../ste_cli; make)
ifdef VALGRIND
	(mkdir -p ./memcheck)
endif
ifdef USE_GCOV
	(mkdir -p ./coverage && lcov --directory . $(GCOV_OPT))
endif
	(mkdir -p ./module_test)
	runtest --tool pscc_mal --srcdir ../../../../../../validation/module/connectivity/psdata/psdata_module_test/stepscc/testsuite/mal --outdir ./module_test VALGRIND="$(VALGRIND)"
ifdef USE_GCOV
	lcov --directory . --capture --output-file ./coverage/pscc.info
	lcov --extract ./coverage/pscc.info "*pscc/*" --output-file ./coverage/pscc.info.stripped
	genhtml --title "STE PS CC module test" -o ./coverage ./coverage/pscc.info.stripped
endif


clean:
	rm -rf $(PROG) *.o *.a *.so *.gcno *.gcov *.gcda coverage memcheck module_test .config
