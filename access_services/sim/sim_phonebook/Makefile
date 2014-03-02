######################### -*- Mode: Makefile-Gmake -*- ########################
##
## Copyright (C) ST-Ericsson SA 2010. All rights reserved.
## This code is ST-Ericsson proprietary and confidential.
## Any use of the code for whatever purpose is subject to
## specific written permission of ST-Ericsson SA.
##
## Description     : For sim phonebook test system.
## 
## Author          : Christian Nilsson
## 
###############################################################################

CC=gcc
CFLAGS= -g -D __i386 -O0 -Wall -m32#-g #-Wall for all warnings, -g for ddd debugging
#DEPS = include/simpb.h libsimpb/simpb_internal.h

#Valgrind
VALGRINDTESTD = valgrind --time-stamp=yes --log-file=valgrind_testd.log --leak-check=full --show-reachable=yes
VALGRINDSIMPBD = valgrind --time-stamp=yes --log-file=valgrind_simpbdd.log --leak-check=full --show-reachable=yes

#GCOV
GCOV_FLAGS=-pg -fprofile-arcs -ftest-coverage -m32
GCOV_LIBS=-lgcov
GCOV_OPT=--zerocounters

#Tee log
TEELOGTESTD = | tee testd.log
TEELOGSIMPBD = | tee simpbd.log

#Point out simpbd files
MAINC = simpbd/main/fdmon.c simpbd/main/log.c simpbd/main/main.c simpbd/main/simpbd_record.c simpbd/main/simpbd.c simpbd/main/sockserv.c
CACHEC = simpbd/cache/simpbd_cache.c
HANDLERSC = simpbd/handlers/simpbd_handlers.c simpbd/handlers/simpbd_handler_error.c
SIMC = test/sim_stub/sim.c
INCLUDES = -Isimpbd/include -Iinclude -Itest/sim_stub -Itest -I../sim/include -I../sim/libsimcom/ 

#Point out test and lib files
TESTC = \
	test/test.c\
	libsimpb/libsimpb.c\
	simpbd/main/log.c

TESTCASES = \
	test/tc_supported_pbs.c \
	test/tc_update_record.c \
	test/tc_read_record.c \
	test/tc_get_file_information.c \
	test/tc_urc_pc_refresh.c \
	test/tc_urc_state_changed.c
	
TESTINCLUDES = -Itest -Iinclude -Ilibsimpb -Isimpbd/include -Itest/sim_stub -I../sim/include -I../sim/libsimcom/

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

check: clean
	mkdir -p /tmp/socket/
	touch /tmp/socket/simpbd # fd between libsimpb and simpbd
	touch /tmp/socket/simpbd_st # fd between stub and test
	touch /tmp/socket/simpbd_ts # fd between test and stub
	mkdir -p ./coverage 
	lcov --directory . $(GCOV_OPT) | > gcov.log #start lcov monitoring
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INCLUDES) -o simpbdd $(MAINC) $(CACHEC) $(HANDLERSC) $(SIMC) $(GCOV_LIBS)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TESTINCLUDES) -o testd $(TESTC) $(TESTCASES) $(GCOV_LIBS)
	$(VALGRINDSIMPBD) ./simpbdd  $(TEELOGSIMPBD) & # fork to allow test to run
	sleep 1s # to let the simpbd startup before test
	$(VALGRINDTESTD) ./testd $(TEELOGTESTD) ;
	killall  memcheck-x86-li #kill forked simpbd (valgrind process)
	sleep 1s # needed for lcov monitoring to stop
	lcov --directory . --capture --output-file ./coverage/simpb_gcov.log >> gcov.log # extract lcov info
	lcov --extract ./coverage/simpb_gcov.log "*" --output-file ./coverage/simpb_gcov_sum.log >> gcov.log # summarize
	genhtml --title "STE SIM Phonebook module test" -o ./coverage ./coverage/simpb_gcov_sum.log >> gcov.log # generate nice lcov html result
	rm -f /tmp/socket/*
	
build:clean
	$(CC) $(CFLAGS) $(INCLUDES) -o simpbdd $(MAINC) $(CACHEC) $(HANDLERSC) $(SIMC)
	$(CC) $(CFLAGS) $(TESTINCLUDES) -o testd $(TESTC) $(TESTCASES)

check_simpbdd: clean
	$(CC) $(CFLAGS) $(INCLUDES) -o simpbdd $(MAINC) $(CACHEC) $(HANDLERSC) $(SIMC)
	$(VALGRINDSIMPBD) ./simpbdd

check_testd: clean
	$(CC) $(CFLAGS) $(TESTINCLUDES) -o testd $(TESTC) $(TESTCASES)
	$(VALGRINDTESTD) ./testd

clean:
	rm -rf *.log *.out coverage *.gcda *.gcno simpbdd testd
	rm -f /tmp/socket/*
	
	
