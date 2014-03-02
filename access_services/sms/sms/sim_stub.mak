#/*
# * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sim_stub-0.1
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

#export CFG_ENABLE_SMS_STRICT_CC_FLAGS := TRUE

ARCHFLAGS := -m32

SMS_SERVER_LIBRARY_SHARED_OBJECT := ./libsms_server.so

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SIM_LIBRARY_INCLUDE_PATH := -I./test_harness/stubs/sim/include \
                            -I../../sim/sim/include \
                            -I../../sim/sim/libsimcom
else
SIM_LIBRARY_INCLUDE_PATH :=
endif

ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) \
                     $(SIM_LIBRARY_INCLUDE_PATH) \
                     -I./config/include

# The following stricter CFLAGS can be used to ensure that the SMS code is of higher quality.
ifeq ($(CFG_ENABLE_SMS_STRICT_CC_FLAGS), TRUE)
EXTRA_CFLAGS := -Wall -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror -Werror-implicit-function-declaration \
                -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations \
                -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs \
                -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wunsafe-loop-optimizations \
                -Wwrite-strings -std=c99 \
                -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -Wswitch-enum -fno-common \
                -Winline -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 \
                -Wundef -pedantic
endif

CFLAGS := -c -Wall $(ARCHFLAGS) -O2 $(EXTRA_CFLAGS) $(STAGING_AREA)/usr/include $(CFLAGS) $(ALL_INCLUDE_PATHS)

# Signalling code ported from OSE builds with a large number of "dereferencing type-punned pointer will
# break strict-aliasing rules" warnings. Until / unless this is fixed disable the strict aliasing; this
# will prevent the compiler doing some optimisation.
CFLAGS := $(CFLAGS) -fno-strict-aliasing

SIM_STUB_CFLAGS := $(CFLAGS) -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED -DSMS_PRINT_A_
LDFLAGS         := -L$(STAGING_AREA)/usr/lib32 -L$(CURDIR) -lrt $(LDFLAGS)

# Enable some extra SMS logging.
ifeq ($(CFG_ENABLE_EXTRA_SIM_LOGGING), TRUE)
SIM_STUB_CFLAGS := $(SIM_STUB_CFLAGS) -DSIM_PRINT_B_
endif

# Enable all SMS logging.
ifeq ($(CFG_ENABLE_VERBOSE_SIM_LOGGING), TRUE)
SIM_STUB_CFLAGS := $(SIM_STUB_CFLAGS) -DSIM_PRINT_B_ -DSIM_PRINT_C_
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SIM_STUB_CFLAGS  := $(SIM_STUB_CFLAGS) -DSMS_SIM_TEST_PLATFORM
endif

OBJS := sim_stub.o sim_util.o

SIM_STUB_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./sim_stub.mak \
	./sms_server.mak

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SIM_STUB_DEPENDENCY_FILES := $(SIM_STUB_DEPENDENCY_FILES) ./test_harness/stubs/sim/include/sim_stub.h \
                                                          ./test_harness/stubs/sim/include/sim_util.h
endif

install: build
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/sbin 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/sim_stub $(CURDIR)/sim_stub 755 0 0

sim_stub: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(SMS_SERVER_LIBRARY_SHARED_OBJECT) -o sim_stub $(GCOV_LIBS) $(ARCHFLAGS)

sim_util.o: $(SIM_STUB_DEPENDENCY_FILES) test_harness/stubs/sim/libsim/sim_util.c
	$(CC) $(SIM_STUB_CFLAGS) test_harness/stubs/sim/libsim/sim_util.c

sim_stub.o: $(SIM_STUB_DEPENDENCY_FILES) test_harness/stubs/sim/simd/sim_stub.c
	$(CC) $(SIM_STUB_CFLAGS) test_harness/stubs/sim/simd/sim_stub.c

build: sim_stub

# If make file is called with "clean" then delete all test harness object files and the built executable
clean:
	rm -f sim_stub $(OBJS)

# If make file is called with "all" then build sim_stub
all: sim_stub

# If make file is called with "rebuild" then clean all files before building sim_stub.
rebuild: clean sim_stub

