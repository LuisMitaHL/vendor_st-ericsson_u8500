#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_server-0.1

ifeq ($(SMS_SET_FEATURE_BACKEND),MAL)
$(info SMS: Building for MAL backend)
else
ifeq ($(SMS_SET_FEATURE_BACKEND),MFL)
$(info SMS: Building for MFL backend)
else
$(error SMS: No backend selected!)
endif
endif

# Enable flag CFG_ENABLE_EXTRA_SMS_LOGGING if medium debugging is wanted in SMS server.
# (Adds SMS_B_ prints)
export CFG_ENABLE_EXTRA_SMS_LOGGING:=TRUE

# Enable flag CFG_ENABLE_VERBOSE_SMS_LOGGING if maximum debugging is wanted in SMS server.
# (Adds SMS_B_ and SMS_C_ prints)
export CFG_ENABLE_VERBOSE_SMS_LOGGING:=TRUE

# Enable the use of stricter compiler flags. This can help uncover problems in the SMS server
# code. However it is not possible to keep them enabled all the time as there are some problems
# which are introduced in header files which the SMS Server must include.
#export CFG_ENABLE_SMS_STRICT_CC_FLAGS := TRUE



# Enable Stand Alone Test harness.
#export CFG_ENABLE_SMS_TEST_HARNESS := TRUE

# Enable LTP Test Harness.
#export CFG_ENABLE_SMS_LTP_TEST_HARNESS := TRUE

# Enable use of modem stubs for running with test harness. (Only applicable if
# test harness is enabled.)
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), TRUE)
# export flag is used so that CFG_ENABLE_SMS_MODEM_STUB can be used in
# sub-makefiles.
export CFG_ENABLE_SMS_MODEM_STUB := TRUE
export CFG_ENABLE_SMS_SIM_STUB   := TRUE
export CFG_ENABLE_SMS_COPS_STUB  := TRUE
endif
ifeq ($(CFG_ENABLE_SMS_LTP_TEST_HARNESS), TRUE)
# export flag is used so that CFG_ENABLE_SMS_MODEM_STUB can be used in
# sub-makefiles.
export CFG_ENABLE_SMS_MODEM_STUB := TRUE
export CFG_ENABLE_SMS_SIM_STUB   := TRUE
export CFG_ENABLE_SMS_COPS_STUB  := TRUE
endif

# Define which, if any, modem stubs to be used. 
# See notes below about unsetting these flags
ifeq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
export CFG_SMS_USE_MAL_MODEM_STUBS := TRUE
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
export CFG_SMS_USE_MFL_MODEM_STUBS := TRUE
endif
endif


ifeq ($(CFG_ENABLE_SMS_SIM_STUB), TRUE)
export CFG_SMS_USE_SIM_STUBS := TRUE
endif

ifeq ($(CFG_ENABLE_SMS_COPS_STUB), TRUE)
export CFG_SMS_USE_COPS_STUBS := TRUE
endif

ifeq ($(CFG_ENABLE_CODE_COVERAGE), TRUE)
export CFG_ENABLE_GCOV := TRUE
endif

# Remove support for SMS Storage Layer. (N.B. Storage support is provided for
# TA test cases using AT commands. While there are no known bugs, all SIM-based
# Short Message support is currently disabled because there are no suitable SIM
# APIs. Therefore it is possible that storage support, which would normally use
# the SIM, may not be working completely as wanted!)
#export CFG_REMOVE_SMS_STORAGE_LAYER := TRUE


# If you want the make process to continue even if there is an error, use something like
# $(MAKE) -i -f sms_server_main.mak buildmessaging

clean:
	rm -rf *.o *.so *.a *.gcda *.gcno *.log *.c.gcov coverage \
	messaging sms_server sms_test_harness sms_stub sim_stub \
	smstest_ltp *.out

build:
	$(MAKE) -f sms_server_lib.mak               build
	$(MAKE) -f sms_server_main.mak              build
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_stand_alone.mak build
endif
ifeq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
	$(MAKE) -f sms_modem_stub.mak               build
endif
ifeq ($(CFG_ENABLE_SMS_SIM_STUB), TRUE)
	$(MAKE) -f sim_stub.mak                     build
endif
ifeq ($(CFG_ENABLE_SMS_LTP_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_ltp.mak         build
endif

install:
	$(MAKE) -f sms_server_lib.mak               install
	$(MAKE) -f sms_server_main.mak              install
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_stand_alone.mak install
endif
ifeq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
	$(MAKE) -f sms_modem_stub.mak               install
endif
ifeq ($(CFG_ENABLE_SMS_SIM_STUB), TRUE)
	$(MAKE) -f sim_stub.mak                     install
endif
ifeq ($(CFG_ENABLE_SMS_LTP_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_ltp.mak         install
endif

all:
	$(MAKE) -f sms_server_lib.mak               all
	$(MAKE) -f sms_server_main.mak              all
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_stand_alone.mak all
endif
ifeq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
	$(MAKE) -f sms_modem_stub.mak               all
endif
ifeq ($(CFG_ENABLE_SMS_SIM_STUB), TRUE)
	$(MAKE) -f sim_stub.mak                     all
endif
ifeq ($(CFG_ENABLE_SMS_LTP_TEST_HARNESS), TRUE)
	$(MAKE) -f sms_test_harness_ltp.mak         all
endif

rebuild: clean build
