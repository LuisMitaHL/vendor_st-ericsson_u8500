/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


(If you are trying to build the SMS Server test harness for the Android platform, please
see information at end of this file.)

The SMS Server test harness is split in two parts depending on the build you are doing.
They are:
 - a stand-alone test harness
 - test cases which run within the LTP framework
Whichever test harness is compiled they both use the same test cases which are in common files.

The top-level sms_server.mak file is used to configure which test harness should be run.
(Don't try and enable both at the same time!)

The contents of the ./test_harness/ folder are are as follows:

./test_harness/common/
    Contains files which are used by both test harnesses.  All new test cases need
    to be:
     1) written in ./common/src/test_cases.c.
     2) added to ./common/include/test_cases.con using macros SMS_TEST_CASE_EXECUTE
        and SMS_TEST_CASE_DECLARATION. (Follow instructions in test_cases.con.)

./test_harness/config/
    Contains a test harness version of c_sms_config.c. This file is used when either
    test harness is being used so that certain configurable parameters are correctly
    set for the test harnesses to complete successfully.

./test_harness/ltp/
    Contains the files necessary to run the test harness using the LTP framework.
    ./ltp/smstestltp.c calls the test cases which are in ./common/src/test_cases.c.

./test_harness/stand_alone/
    Contains the files necessary to run the test harness as a stand-alone process.
    ./stand_alone/smstest.c calls the test cases which are in ./common/src/test_cases.c.

./test_harness/stubs/
    Contains modem stub files for the MAL and MFL modem implementations. These stub
    files are used to allow the SMS Server to compile and run in a controlled environment
    so the test cases can be executed without requiring live network support.
    The top-level makefile, sms_server.mak, determines which stubs should be compiled based
    on the value of the SMS_SET_FEATURE_BACKEND parameter (MAL or MFL).






***************** Building and Running Test Harness for Android *****************
When you need to enable the test harness, you need to determine whether you also need to enable
the stub test harness module. 
When the test harness is enabled in the SMS Server Android.mk file, it will either build the
test harness for MAL (without the stub) or with the stub module.
The executables will be stored in /system/bin of the flashed Android phone. However, without
some further tweaks, the tests cannot be run for the following reasons:
1) If configured, the test code relies on the sms_stub process simulating certain responses
from the modem, however the sms_stub process must be started before the sms_server process or
sockets will not be correctly initialised.
2) The ril daemon will be started automatically and will try and use the SMS Server; this could
   interfere with the tests.
Therefore, it is necessary to update the Android code which controls the starting of processes
and daemons; i.e. update ./system/core/rootdir/init.rc
init.rc should be modified so that the ril-daemon (rild) is modified to a "oneshot"
service and the SMS Server (sms_server) is commented out.
Then, when the software has been flashed and the phone booted, the test harness can be started by
typing at the prompt:
  ps
  [Check the list of running processes and confirm that sms_server is not running and kill rild.]
  sms_stub& (only if stub eneabled)
  sms_server&
  sms_test_harness&
