######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : Makefile
## Description     : For routing controller test system.
##
## Author          : Christian Nilsson
##
##############################################################################

CC=gcc
CFLAGS= -Wall -DSTERC_MODULE_TEST_ENABLED -g #-Wall for all warnings, -g for ddd debugging

#Valgrind
VALGRINDTESTD = valgrind --time-stamp=yes --log-file=valgrind_testd.log --leak-check=full --show-reachable=yes
VALGRINDSTERCD = valgrind --time-stamp=yes --log-file=valgrind_stercd.log --leak-check=full --show-reachable=yes

#GCOV
GCOV_FLAGS=-fprofile-arcs -ftest-coverage #-pg
GCOV_LIBS=-lgcov
GCOV_OPT="--zerocounters"

#Tee log
TEELOGTESTD = | tee -a test.log
TEELOGSTERCD = 2>&1 2> stercd.log#| tee -a stercd.log#

COMMON = ../psdata/libmpl/mpl_config.c ../psdata/libmpl/mpl_list.c ../psdata/libmpl/mpl_msg.c ../psdata/libmpl/mpl_param.c \
	libsterc/sterc_msg.c ../../common/libstecom/stecom.c ../../common/libstecom/subscribe.c ../psdata/libnlcom/nlcom.c
COMMON_INCLUDES = -Ilibsterc/ -I../psdata/libmpl -I../../common/libstecom/ -Isterc/ -I../psdata/libnlcom/

#Point out stercd files
STERCD = $(COMMON) sterc/sterc_handler.c sterc/sterc_runscript.c sterc/stercd.c
INCLUDES = $(COMMON_INCLUDES)

#Point out test and lib files
TESTC = $(COMMON) test/test.c
TESTINCLUDES = $(COMMON_INCLUDES)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

check:clean
	mkdir -p /tmp/
	touch /tmp/sterc_ctrl_server_socket
	touch /tmp/sterc_event_server_socket
	mkdir -p ./coverage
	lcov --directory . $(GCOV_OPT) | > gcov.log #start lcov monitoring
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INCLUDES) -o stercd $(STERCD) $(GCOV_LIBS)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TESTINCLUDES) -o testd $(TESTC) $(GCOV_LIBS)
	$(VALGRINDSTERCD) ./stercd -f -d 1 -t unix -r test/test_script -s test/test_script $(TEELOGSTERCD) & # fork to allow test to run
	sleep 1s # to let the stercd startup before test
	$(VALGRINDTESTD) ./testd $(TEELOGTESTD)
	killall -v -r ^memcheck #kill forked stercd (valgrind process)
	sleep 1s # needed for lcov monitoring to stop
	lcov --directory . --quiet --capture --output-file ./coverage/stercd_gcov.log 2>> gcov.log # extract lcov info
	lcov --extract ./coverage/stercd_gcov.log "*" --output-file ./coverage/stercd_gcov_sum.log >> gcov.log # summarize
	genhtml --title "STE sterc module test" -o ./coverage ./coverage/stercd_gcov_sum.log >> gcov.log # generate nice lcov html result

build:clean
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INCLUDES) -o stercd $(STERCD) $(GCOV_LIBS)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TESTINCLUDES) -o testd $(TESTC) $(GCOV_LIBS)

clean:
	rm -rf *.log *.out coverage *.gcda *.gcno ..\#* \#* stercd testd