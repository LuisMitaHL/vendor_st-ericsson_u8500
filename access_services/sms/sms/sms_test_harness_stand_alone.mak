#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_test_harness-0.1
CC=gcc
#CFLAGS := -I$(TOPLEVEL)/linux-2.6-U300.git/include $(CFLAGS)
# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

ifeq ($(CFG_ENABLE_GCOV), TRUE)
GCOV_LIBS  := -lgcov
else
GCOV_LIBS  :=
endif

ARCHFLAGS := -m32

SMS_SERVER_LIBRARY_SHARED_OBJECT := ./libsms_server.so

# The following stricter CFLAGS can be used to ensure that the SMS code is of higher quality.
ifeq ($(CFG_ENABLE_SMS_STRICT_CC_FLAGS), TRUE)
EXTRA_CFLAGS := -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror -Werror-implicit-function-declaration \
                -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations \
                -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs \
                -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default \
                -Wwrite-strings -std=c99 \
                -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -Wswitch-enum -fno-common \
                -Winline -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 \
                -Wundef -pedantic
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SIM_LIBRARY_INCLUDE_PATH := -I./test_harness/stubs/sim/include \
                            -I../../sim/sim/include \
                            -I../../sim/sim/libsimcom
else
SIM_LIBRARY_INCLUDE_PATH :=
endif

ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) \
                     -I./include \
                     -I./config/include \
                     -I./internal/main/include \
                     -I./usr/include \
                     -I../../common/common_functionality/include \
                     $(SIM_LIBRARY_INCLUDE_PATH)

CFLAGS := -c -Wall $(ARCHFLAGS) -O2 $(EXTRA_CFLAGS) $(STAGING_AREA)/usr/include $(CFLAGS) $(ALL_INCLUDE_PATHS) \
          -I./include \
          -I./internal/main/include \
          -I./internal/porting/include \
          -I./test_harness/common/include

# Signalling code ported from OSE builds with a large number of "dereferencing type-punned pointer will
# break strict-aliasing rules" warnings. Until / unless this is fixed disable the strict aliasing; this
# will prevent the compiler doing some optimisation.
CFLAGS := $(CFLAGS) -fno-strict-aliasing

SMS_TH_CFLAGS           := $(CFLAGS) -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED -DSMS_PRINT_A_
LDFLAGS                 := -L$(STAGING_AREA)/usr/lib32 -L$(CURDIR) -lrt $(LDFLAGS)


ifdef CFG_REMOVE_SMS_STORAGE_LAYER
SMS_TH_CFLAGS           := $(SMS_TH_CFLAGS) -DREMOVE_SMSTRG_LAYER
endif

# Enable some extra SMS logging.
ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING), TRUE)
SMS_TH_CFLAGS := $(SMS_TH_CFLAGS) -DSMS_PRINT_B_
endif

# Enable all SMS logging.
ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING), TRUE)
SMS_TH_CFLAGS := $(SMS_TH_CFLAGS) -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SMS_TH_CFLAGS := $(SMS_TH_CFLAGS) -DSMS_SIM_TEST_PLATFORM
endif

ifeq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
SMS_TH_CFLAGS := $(SMS_TH_CFLAGS) -DSMS_MODEM_TEST_PLATFORM
endif

OBJS := smstest.o test_cases.o c_sms_config_test.o

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
OBJS := $(OBJS) sim_util.o sim.o
endif

SMS_TEST_HARNESS_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./config/include/c_sms_config.h \
	./internal/main/include/g_sms.h \
	./internal/porting/include/smsipc.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./test_harness/common/include/test_cases.con \
	./test_harness/common/include/test_cases.h \
	./sms_test_harness_stand_alone.mak \
	./sms_server.mak

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SMS_TEST_HARNESS_DEPENDENCY_FILES := $(SMS_TEST_HARNESS_DEPENDENCY_FILES) ./test_harness/stubs/sim/include/sim_stub.h \
	                                                                  ./test_harness/stubs/sim/include/sim_util.h
endif

install: build
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/sbin 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/sms_test_harness $(CURDIR)/sms_test_harness 755 0 0

# Build Test harness files.
sms_test_harness: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(SMS_SERVER_LIBRARY_SHARED_OBJECT) -o sms_test_harness $(GCOV_LIBS) $(ARCHFLAGS)

smstest.o: $(SMS_TEST_HARNESS_DEPENDENCY_FILES) test_harness/stand_alone/src/smstest.c
	$(CC) $(SMS_TH_CFLAGS) test_harness/stand_alone/src/smstest.c

test_cases.o: $(SMS_TEST_HARNESS_DEPENDENCY_FILES) test_harness/common/src/test_cases.c
	$(CC) $(SMS_TH_CFLAGS) test_harness/common/src/test_cases.c

c_sms_config_test.o: $(SMS_TEST_HARNESS_DEPENDENCY_FILES) test_harness/config/c_sms_config_test.c
	$(CC) $(SMS_TH_CFLAGS) test_harness/config/c_sms_config_test.c

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
sim_util.o: $(SMS_TEST_HARNESS_DEPENDENCY_FILES) test_harness/stubs/sim/libsim/sim_util.c
	$(CC) $(SMS_TH_CFLAGS) test_harness/stubs/sim/libsim/sim_util.c

sim.o: $(SMS_TEST_HARNESS_DEPENDENCY_FILES) test_harness/stubs/sim/libsim/sim.c
	$(CC) $(SMS_TH_CFLAGS) test_harness/stubs/sim/libsim/sim.c
endif

build: sms_test_harness

# If make file is called with "clean" then delete all test harness object files and the built executable
clean:
	rm -f sms_test_harness smstest.o test_cases.o c_sms_config_test.o

# If make file is called with "all" then build sms_test_harness
all: sms_test_harness

# If make file is called with "rebuild" then clean all files before building sms_test_harness.
rebuild: clean sms_test_harness
