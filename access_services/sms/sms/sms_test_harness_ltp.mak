#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_test_harness-0.1
#CFLAGS := -I$(TOPLEVEL)/linux-2.6-U300.git/include $(CFLAGS)
# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

SMS_SERVER_LIBRARY_SHARED_OBJECT := ./libsms_server.so

# The following stricter CFLAGS can be used to ensure that the SMS code is of higher quality.
ifeq ($(CFG_ENABLE_SMS_STRICT_CC_FLAGS), TRUE)
EXTRA_CFLAGS := -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror -Werror-implicit-function-declaration \
                -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations \
                -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs \
                -Wpointer-arith -Wshadow -Wswitch-default -std=c99 \
                -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -Wswitch-enum -fno-common \
                -Winline -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 \
                -pedantic
endif

CFLAGS := -c -Wall -O2 $(EXTRA_CFLAGS) $(STAGING_AREA)/usr/include $(CFLAGS) \
          -I./include \
          -I./internal/main/include \
          -I./internal/porting/include \
          -I./test_harness/common/include \
          -I$(CURDIR)/../../ltp/addons/include \
          -I$(CURDIR)/../../ltp/tempdir/ltp-full/include

# Signalling code ported from OSE builds with a large number of "dereferencing type-punned pointer will
# break strict-aliasing rules" warnings. Until / unless this is fixed disable the strict aliasing; this
# will prevent the compiler doing some optimisation.
CFLAGS := $(CFLAGS) -fno-strict-aliasing

SMS_TH_CFLAGS           := $(CFLAGS) -DSMS_PRINT_A_
LDFLAGS                 := -L$(CURDIR)/../../ltp/addons -L$(CURDIR)/../../ltp/tempdir/ltp-full/lib  -L$(STAGING_AREA)/usr/lib -L$(CURDIR) -lrt $(LDFLAGS)
#LDFLAGS                 := -L$(CURDIR)/../../ltp/addons/ -lltp_addons -L$(CURDIR)/../../ltp/tempdir/ltp-full/lib -lltp -L$(STAGING_AREA)/usr/lib -L$(CURDIR) -lrt $(LDFLAGS)
#LDFLAGS                 := -L$(STAGING_AREA)/usr/lib -L$(CURDIR) -lrt $(LDFLAGS)


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

OBJS := smstestltp.o test_cases.o

SMS_TEST_LTP_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./internal/porting/include/smsipc.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./test_harness/common/include/test_cases.con \
	./test_harness/common/include/test_cases.h \
	./sms_test_harness_ltp.mak \
	./sms_server.mak

install: build
	@$(PACKAGE_DIR) /opt 755 0 0
	@$(PACKAGE_DIR) /opt/ltp 755 0 0
	@$(PACKAGE_DIR) /opt/ltp/testcases 755 0 0
	@$(PACKAGE_DIR) /opt/ltp/testcases/bin 755 0 0
	@$(PACKAGE_DIR) /opt/ltp/runtest 755 0 0
	@$(PACKAGE_FILE) /opt/ltp/testcases/bin/smstest_ltp $(CURDIR)/smstest_ltp 755 0 0
	@$(PACKAGE_FILE) /opt/ltp/runtest/sms_command $(CURDIR)/test_harness/ltp/command/sms_command 755 0 0

# Build Test files.

smstest_ltp: $(OBJS)
#	$(CC) $(LDFLAGS) $(OBJS) $(SMS_SERVER_LIBRARY_SHARED_OBJECT) sms_test_ltp/lib/libltp.a -o smstest_ltp
	$(CC) $(LDFLAGS) $(OBJS) $(SMS_SERVER_LIBRARY_SHARED_OBJECT) $(CURDIR)/../../ltp/tempdir/ltp-full/lib/libltp.a -o smstest_ltp
	
smstestltp.o: $(SMS_TEST_LTP_DEPENDENCY_FILES) ./test_harness/ltp/src/smstestltp.c
	$(CC) $(SMS_TH_CFLAGS) ./test_harness/ltp/src/smstestltp.c
		
test_cases.o: $(SMS_TEST_LTP_DEPENDENCY_FILES) ./test_harness/common/src/test_cases.c
	$(CC) $(SMS_TH_CFLAGS) ./test_harness/common/src/test_cases.c



build: smstest_ltp

# If make file is called with "clean" then delete all ltp test object files and the built executable
clean:
	rm -f smstest_ltp smstestltp.o

# If make file is called with "all" then build sms_test_ltp
all: smstest_ltp

# If make file is called with "rebuild" then clean all files before building smstest_ltp.
rebuild: clean smstest_ltp





