######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : Makefile
## Description     : For sms system.
##
## Author          : Kjell Andersson <kjell.k.andersson@stericsson.com>
##
###############################################################################
LOG_SIMSTUB              := sim_stub.log
LOG_SMSSTUB              := sms_stub.log
LOG_SMSSERVER            := sms_server.log
LOG_TESTHARNESS          := sms_test_harness.log
LOG_SIMSTUB_VALGRIND     := valgrind_$(LOG_SIMSTUB)
LOG_SMSSTUB_VALGRIND     := valgrind_$(LOG_SMSSTUB)
LOG_SMSSERVER_VALGRIND   := valgrind_$(LOG_SMSSERVER)
LOG_TESTHARNESS_VALGRIND := valgrind_$(LOG_TESTHARNESS)
LOG_GCOV                 := gcov.log
LOG_SUMMARY              := result_summary.log

VALGRINDSIMSTUB        := valgrind --time-stamp=yes --track-fds=yes --log-file=$(LOG_SIMSTUB_VALGRIND) --leak-check=full --show-reachable=yes
VALGRINDSMSSTUB        := valgrind --time-stamp=yes --track-fds=yes --log-file=$(LOG_SMSSTUB_VALGRIND) --leak-check=full --show-reachable=yes
VALGRINDSMSSERVER      := valgrind --time-stamp=yes --track-fds=yes --log-file=$(LOG_SMSSERVER_VALGRIND) --leak-check=full --show-reachable=yes --track-origins=yes
VALGRINDSMSTESTHARNESS := valgrind --time-stamp=yes --track-fds=yes --log-file=$(LOG_TESTHARNESS_VALGRIND) --leak-check=full --show-reachable=yes

GCOV_OPT :="--zerocounters"

all:
	$(MAKE) -f sms_server.mak CFG_ENABLE_SMS_TEST_HARNESS=TRUE SMS_SET_FEATURE_BACKEND=MAL all
clean:
	$(MAKE) -f sms_server.mak CFG_ENABLE_SMS_TEST_HARNESS=TRUE SMS_SET_FEATURE_BACKEND=MAL clean

coverity:
ifneq ($(TOP),)
		@sh coverity.sh sms_coverity
		firefox https://seldx080.lud.stericsson.com:1759
else
		@echo "$TOP does not appear to be an Android top directory..."
endif

check:  clean
	mkdir -p ./coverage
	lcov --directory . $(GCOV_OPT) | tee $(LOG_GCOV)    #start lcov monitoring
	$(MAKE) -f sms_server.mak CFG_ENABLE_SMS_TEST_HARNESS=TRUE CFG_ENABLE_CODE_COVERAGE=TRUE SMS_SET_FEATURE_BACKEND=MAL all
	$(VALGRINDSIMSTUB) ./sim_stub | tee $(LOG_SIMSTUB) &    # fork to allow sim stub to run
	sleep 2s
	$(VALGRINDSMSSTUB) ./sms_stub | tee $(LOG_SMSSTUB) &    # fork to allow sms stub to run
	sleep 2s
	$(VALGRINDSMSSERVER) ./sms_server | tee $(LOG_SMSSERVER) &    # fork to allow sms server
	sleep 2s
	$(VALGRINDSMSTESTHARNESS) ./sms_test_harness | tee $(LOG_TESTHARNESS)
	killall -v -w memcheck-x86-li    #kill forked (valgrind process)
	lcov --capture --directory . --output-file ./coverage/sms_test_harness_gcov.log | tee -a $(LOG_GCOV)   # extract lcov info
	lcov --remove ./coverage/sms_test_harness_gcov.log "test_harness*" --output-file ./coverage/sms_test_harness_gcov.log | tee -a $(LOG_GCOV) # filter coverage result
	lcov --remove ./coverage/sms_test_harness_gcov.log "/usr*" --output-file ./coverage/sms_test_harness_gcov.log | tee -a $(LOG_GCOV) # filter coverage result
	lcov --extract ./coverage/sms_test_harness_gcov.log "*" --output-file ./coverage/sms_test_harness_sum.log | tee -a $(LOG_GCOV)  # summarize
	genhtml --title "STE SMS Module - Code Coverage Report" --show-details --num-spaces "4" -o ./coverage ./coverage/sms_test_harness_sum.log | tee -a $(LOG_GCOV)    # generate nice lcov html
	firefox ./coverage/index.html & #open coverage report

	# Create summary report
	echo "************Test result:" > $(LOG_SUMMARY)
	grep 'Test Case' $(LOG_TESTHARNESS) >> $(LOG_SUMMARY) || echo "No match"
	echo "" >> $(LOG_SUMMARY)
	grep ': FAIL' $(LOG_TESTHARNESS) >> $(LOG_SUMMARY) || echo "No match"
	echo "" >> $(LOG_SUMMARY)
	echo "************Coverage result:" >> $(LOG_SUMMARY)
	grep -A 2 Overall $(LOG_GCOV) >> $(LOG_SUMMARY) || echo "No match"
	echo "" >> $(LOG_SUMMARY)
	echo "************Valgrind result: ($(LOG_SMSSERVER_VALGRIND))" >> $(LOG_SUMMARY)
	grep -s 'FILE DESCRIPTORS:' $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) && grep -s 'Open file descriptor' $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) || echo "No match"
	grep -s 'HEAP SUMMARY:' -A 2 $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) || echo "No match"
	grep -s -A 5 'LEAK SUMMARY:' $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) || grep -s 'no leaks' $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) || echo "No match"
	grep -s 'ERROR SUMMARY:' $(LOG_SMSSERVER_VALGRIND) >> $(LOG_SUMMARY) || echo "No match"
	clear && cat $(LOG_SUMMARY)
