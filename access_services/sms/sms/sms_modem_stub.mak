#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_stub-0.1
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

ALL_INCLUDE_PATHS := -I./include \
                     -I./internal/main/include \
                     -I./internal/porting/include \
                     -I../../modem_adaptations/mal/modem_lib/mal/libmalsms/include \
                     -I../../common/common_functionality/include

# Add Modem stubs include paths
ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) -I./test_harness/stubs/mal/include \
                                          -I../modem_lib/mal/libmalsms/include
endif
ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) ./test_harness/stubs/mfl/include
endif


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

SMS_STUB_CFLAGS := $(CFLAGS) -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED -DSMS_PRINT_A_
LDFLAGS         := -L$(STAGING_AREA)/usr/lib32 -L$(CURDIR) -lrt $(LDFLAGS)

ifeq ($(CFG_REMOVE_SMS_STORAGE_LAYER), TRUE)
SMS_STUB_CFLAGS := $(SMS_STUB_CFLAGS) -DREMOVE_SMSTRG_LAYER
endif

# Enable some extra SMS logging.
ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING), TRUE)
SMS_STUB_CFLAGS := $(SMS_STUB_CFLAGS) -DSMS_PRINT_B_
endif

# Enable all SMS logging.
ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING), TRUE)
SMS_STUB_CFLAGS := $(SMS_STUB_CFLAGS) -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SMS_STUB_CFLAGS  := $(SMS_STUB_CFLAGS) -DSMS_SIM_TEST_PLATFORM
endif

ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
OBJS := stub_side_mal_sms.o
endif
ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
OBJS := stub_side_mfl_framework_stub.o
endif

SMS_STUB_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./sms_modem_stub.mak \
	./sms_server.mak

# Add Modem include stub paths
ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
SMS_STUB_DEPENDENCY_FILES := $(SMS_STUB_DEPENDENCY_FILES) \
	../../modem_adaptations/mal/modem_lib/mal/libmalsms/include/mal_sms.h \
	./test_harness/stubs/mal/include/mal_stub.h
endif
ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
# There are many other header files in ./test_harness/stubs/mfl/include/ however they are unlikely to be
# editted so there have been left out of the dependency list!
SMS_STUB_DEPENDENCY_FILES := $(SMS_STUB_DEPENDENCY_FILES) \
	./test_harness/stubs/mfl/include/mfl_stub.h
endif

install: build
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/sbin 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/sms_stub $(CURDIR)/sms_stub 755 0 0

sms_stub: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(SMS_SERVER_LIBRARY_SHARED_OBJECT) -o sms_stub $(GCOV_LIBS) $(ARCHFLAGS)
	
ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
stub_side_mal_sms.o: $(SMS_STUB_DEPENDENCY_FILES) ./test_harness/stubs/mal/src/stub_side_mal_sms.c
	$(CC) $(SMS_STUB_CFLAGS) test_harness/stubs/mal/src/stub_side_mal_sms.c
endif
ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
stub_side_mfl_framework_stub.o: $(SMS_STUB_DEPENDENCY_FILES) test_harness/stubs/mfl/src/stub_side_mfl_framework_stub.c
	$(CC) $(SMS_STUB_CFLAGS) test_harness/stubs/mfl/src/stub_side_mfl_framework_stub.c
endif

build: sms_stub

# If make file is called with "clean" then delete all test harness object files and the built executable
clean:
	rm -f sms_stub $(OBJS)

# If make file is called with "all" then build sms_stub
all: sms_stub

# If make file is called with "rebuild" then clean all files before building sms_stub.
rebuild: clean sms_stub





