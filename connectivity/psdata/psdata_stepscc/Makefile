######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : Makefile
## Description     : For packet switch connection controller test system.
##
## Author          : Christian Nilsson
##
###############################################################################

CC=gcc
COMMON_FLAGS= -Wall -DPSCC_MODULE_TEST_ENABLED -g #-Wall for all warnings, -g for ddd debugging
CFLAGS= $(COMMON_FLAGS) -D_LINUX_IN6_H -DSIOCKILLADDR=1 # special flags to make ubuntu android compatible
#DEPS = include/psccd.h libpsccd/psccd_internal.h

#Valgrind
VALGRINDTESTD_MAL = valgrind --time-stamp=yes --log-file=valgrind_testd_mal.log --leak-check=full --show-reachable=yes
VALGRINDPSCCD_MAL = valgrind --time-stamp=yes --log-file=valgrind_psccd_mal.log --leak-check=full --show-reachable=yes
VALGRINDTESTD_MFL = valgrind --time-stamp=yes --log-file=valgrind_testd_mfl.log --leak-check=full --show-reachable=yes
VALGRINDPSCCD_MFL = valgrind --time-stamp=yes --log-file=valgrind_psccd_mfl.log --leak-check=full --show-reachable=yes

#GCOV
GCOV_FLAGS=-fprofile-arcs -ftest-coverage #-pg
GCOV_LIBS=-lgcov
GCOV_OPT="--zerocounters"

#Tee log
TEELOGTESTD_MAL = | tee -a mal_test.log
TEELOGPSCCD_MAL = 2>&1 2> mal_psccd.log
TEELOGTESTD_MFL = | tee -a mfl.log
TEELOGPSCCD_MFL = >> mfl.log

#Utils needed by both test and stub
COMMON = ../psdata/libmpl/mpl_config.c ../psdata/libmpl/mpl_list.c ../psdata/libmpl/mpl_msg.c ../psdata/libmpl/mpl_param.c \
	libpscc/pscc_msg.c ../../common/libstecom/stecom.c ../../common/libstecom/subscribe.c test/framework/common.c test/stubs/sim.c test/stubs/util.c
COMMON_INCLUDES = -Itest/ -Itest/stubs -Ilibpscc/ -I../psdata/libmpl -I../../common/libstecom/ -Itest/framework -I../../../access_services/sim/sim/include \
        -I../../../access_services/common/common_functionality/include

#Point out psccd and stub files
MAINC = $(COMMON) stepscc/pscc_handler.c stepscc/pscc_nwifcfg.c stepscc/pscc_object.c stepscc/pscc_runscript.c stepscc/pscc_utils.c stepscc/psccd.c stepscc/pscc_sim.c stepscc/pscc_log_client.c
MAINC_MAL = stepscc/pscc_bearer_mal.c stepscc/pscc_qpc_mal.c
MAINC_MFL = stepscc/pscc_bearer_mfl.c stepscc/pscc_pco.c stepscc/pscc_netdev_caif.c ../psdata/libnlcom/nlcom.c
STUBC = #test/stubs/sim.c
STUBC_MAL = test/stubs/mal/mal_gpds.c test/tc_mal/util.c
STUBC_MFL = test/stubs/mfl/mfl.c test/tc_mfl/util.c
INCLUDES = $(COMMON_INCLUDES) -Istepscc/ -I../psdata/libnlcom/
MAININCLUDES_MAL = -Itest/tc_mal/ -Itest/stubs/mal
MAININCLUDES_MFL = -Itest/tc_mfl/ -Itest/stubs/mfl
MAINLIBS = -lcrypto

#Point out test and lib files
TESTC = $(COMMON) test/framework/test.c
TESTCASES_MAL = test/tc_mal/util_conn_param.c test/tc_mal/util_requests.c test/tc_mal/util_scenarios.c test/tc_mal/util.c test/tc_mal/tc_mal_basic.c test/tc_mal/tc_mal_modify.c test/tc_mal/tc_mal_cc.c test/tc_mal/tc_mal_attach.c
TESTCASES_MFL = test/tc_mfl/util.c #test/tc_mfl/tc_mfl_basic.c
TESTINCLUDES = $(COMMON_INCLUDES)
TESTINCLUDES_MAL = -Itest/tc_mal/ -Itest/stubs/mal
TESTINCLUDES_MFL = -Itest/tc_mfl/ -Itest/stubs/mfl
TESTLIBS =

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

check:check_mal check_mfl

check_mal:clean
	mkdir -p /tmp/
	touch /tmp/pscc_ctrl_server_socket
	touch /tmp/pscc_event_server_socket
	touch /tmp/pscc_test_gpds_socket_ts
	touch /tmp/pscc_test_gpds_socket_st
	touch /tmp/pscc_test_netlnk_socket_ts
	touch /tmp/pscc_test_netlnk_socket_st
	touch /tmp/pscc_test_sim_socket_ts
	touch /tmp/pscc_test_sim_socket_st
	mkdir -p ./coverage
	lcov --directory . $(GCOV_OPT) | > mal_gcov.log #start lcov monitoring
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TESTINCLUDES) $(TESTINCLUDES_MAL) -o testd $(TESTC) $(TESTCASES_MAL) $(GCOV_LIBS) $(TESTLIBS)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INCLUDES) $(MAININCLUDES_MAL) -o psccd $(MAINC) $(MAINC_MAL) $(STUBC) $(STUBC_MAL) $(GCOV_LIBS) $(MAINLIBS)
	$(VALGRINDPSCCD_MAL) ./psccd -f -d 9 $(TEELOGPSCCD_MAL) & # fork to allow test to run
	sleep 1s # to let the psccd startup before test
	$(VALGRINDTESTD_MAL) ./testd $(TEELOGTESTD_MAL)
	killall -v -r ^memcheck #kill forked psccd (valgrind process)
	sleep 1s # needed for lcov monitoring to stop
	@rm -f *mpl_config* *mpl_list* *mpl_msg* *mpl_param* *stecom* *subscribe* #
	lcov --directory . --quiet --capture --output-file ./coverage/psccd_mal_gcov.log 2>> mal_gcov.log # extract lcov info
	lcov --extract ./coverage/psccd_mal_gcov.log "*" --output-file ./coverage/psccd_mal_gcov_sum.log >> mal_gcov.log # summarize
	genhtml --title "STE PSCC module test" -o ./coverage ./coverage/psccd_mal_gcov_sum.log >> mal_gcov.log # generate nice lcov html result

check_mfl:clean
	mkdir -p /tmp/
	touch /tmp/pscc_ctrl_server_socket
	mkdir -p ./coverage
	lcov --directory . $(GCOV_OPT) | > mal_gcov.log #start lcov monitoring
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TESTINCLUDES) $(TESTINCLUDES_MFL) -o testd $(TESTC) $(TESTCASES_MFL) $(GCOV_LIBS) $(TESTLIBS)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INCLUDES) $(MAININCLUDES_MFL) -o psccd $(MAINC) $(MAINC_MFL) $(STUBC) $(STUBC_MFL) $(GCOV_LIBS) $(MAINLIBS)
	$(VALGRINDPSCCD_MFL) ./psccd -f -d 9 $(TEELOGPSCCD_MFL) & # fork to allow test to run
	sleep 1s # to let the psccd startup before test
	$(VALGRINDTESTD_MFL) ./testd $(TEELOGTESTD_MFL)
	killall -v -r ^memcheck #kill forked psccd (valgrind process)
	sleep 1s # needed for lcov monitoring to stop
	lcov --directory . --capture --output-file ./coverage/psccd_mfl_gcov.log >> mfl_gcov.log # extract lcov info
	lcov --extract ./coverage/psccd_mfl_gcov.log "*" --output-file ./coverage/psccd_mfl_gcov_sum.log >> mfl_gcov.log # summarize
	genhtml --title "STE PSCC module test" -o ./coverage ./coverage/psccd_mfl_gcov_sum.log >> mfl_gcov.log # generate nice lcov html result

build:build_mal build_mfl

build_mal:clean
	$(CC) $(CFLAGS) $(TESTINCLUDES) $(TESTINCLUDES_MAL) -o testd $(TESTC) $(TESTCASES_MAL) $(TESTLIBS)
	$(CC) $(CFLAGS) $(INCLUDES) $(MAININCLUDES_MAL) -o psccd $(MAINC) $(MAINC_MAL) $(STUBC) $(STUBC_MAL) $(MAINLIBS)

build_mfl:clean
	$(CC) $(CFLAGS) $(TESTINCLUDES) $(TESTINCLUDES_MFL) -o testd $(TESTC) $(TESTCASES_MFL) $(TESTLIBS)
	$(CC) $(CFLAGS) $(INCLUDES) $(MAININCLUDES_MFL) -o psccd $(MAINC) $(MAINC_MFL) $(STUBC) $(STUBC_MFL) $(MAINLIBS)

clean:
	rm -rf *.log *.out coverage *.gcda *.gcno ..\#* \#* psccd testd
