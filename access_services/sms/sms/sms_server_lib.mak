#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_server_lib-0.1
CC=gcc
#CFLAGS := -I$(TOPLEVEL)/linux-2.6-U300.git/include $(CFLAGS)
# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

ifeq ($(CFG_ENABLE_GCOV), TRUE)
GCOV_FLAGS := -pg -fprofile-arcs -ftest-coverage
GCOV_LIBS  := -lgcov
else
GCOV_FLAGS :=
GCOV_LIBS  :=
endif

ARCHFLAGS := -m32

# The following STRICT CFLAGS can be used to ensure that the SMS code is of high quality.
ifeq ($(CFG_ENABLE_SMS_STRICT_CC_FLAGS), TRUE)
EXTRA_CFLAGS := -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror -Werror-implicit-function-declaration \
                -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations \
                -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
                -Wstrict-prototypes -Wswitch-default -Wwrite-strings \
                -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -fno-common \
                -Winline -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 \
                -Wundef
endif
ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) \
          -I../../common/common_functionality/include

CFLAGS := -c -g -Wall $(ARCHFLAGS) $(GCOV_FLAGS) -O2 $(EXTRA_CFLAGS) $(STAGING_AREA)/usr/include $(CFLAGS)  $(ALL_INCLUDE_PATHS) \
          -I./include \
          -I./internal/porting/include \
          -I./internal/main/include \
          -I../../common/common_functionality/include

# Signalling code ported from OSE builds with a large number of "dereferencing type-punned pointer will
# break strict-aliasing rules" warnings. Until / unless this is fixed disable the strict aliasing; this
# will prevent the compiler doing some optimisation.
CFLAGS := $(CFLAGS) -fno-strict-aliasing

SMS_INTERFACE_CFLAGS := $(CFLAGS) -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED -DSMS_PRINT_A_
LDFLAGS := -shared
LIBS := -L$(STAGING_AREA)/usr/lib32 -lrt

# Enable some extra SMS logging.
ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING), TRUE)
SMS_INTERFACE_CFLAGS := $(SMS_INTERFACE_CFLAGS) -DSMS_PRINT_B_
endif

# Enable all SMS logging.
ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING), TRUE)
SMS_INTERFACE_CFLAGS := $(SMS_INTERFACE_CFLAGS) -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

ifeq ($(CFG_REMOVE_SMS_STORAGE_LAYER), TRUE)
SMS_INTERFACE_CFLAGS := $(SMS_INTERFACE_CFLAGS) -DREMOVE_SMSTRG_LAYER
endif

OBJS := r_sms.o r_cbs.o smslinuxporting.o smsipc.o smstimerserver.o util_stub.o

SMS_SERVER_LIB_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./include/t_cbs.h \
	./include/r_cbs.h \
	./include/t_sms_cb_session.h \
	./include/r_sms_cb_session.h \
	./internal/main/include/d_sms.h \
	./internal/main/include/g_sms.h \
	./internal/main/include/g_cbs.h \
	./internal/porting/include/smsipc.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./sms_server_lib.mak \
	./sms_server.mak

libsms_server.so: $(OBJS)
	$(CC) $(LDFLAGS) -o libsms_server.so $(OBJS) $(LIBS) $(GCOV_LIBS) $(ARCHFLAGS)

libsms_server.a: $(OBJS)
	$(AR) rcs libsms_server.a $(OBJS)

r_sms.o: $(SMS_SERVER_LIB_DEPENDENCY_FILES) internal/main/src/r_sms.c
	$(CC) $(SMS_INTERFACE_CFLAGS) internal/main/src/r_sms.c
	
r_cbs.o: $(SMS_SERVER_LIB_DEPENDENCY_FILES) internal/main/src/r_cbs.c
	$(CC) $(SMS_INTERFACE_CFLAGS) internal/main/src/r_cbs.c
	
smsipc.o: $(SMS_SERVER_LIB_DEPENDENCY_FILES) internal/porting/src/smsipc.c
	$(CC) $(SMS_INTERFACE_CFLAGS) internal/porting/src/smsipc.c
	
smslinuxporting.o: $(SMS_SERVER_LIB_DEPENDENCY_FILES) internal/porting/src/smslinuxporting.c
	$(CC) $(SMS_INTERFACE_CFLAGS) internal/porting/src/smslinuxporting.c
	
smstimerserver.o: $(SMS_SERVER_LIB_DEPENDENCY_FILES) internal/porting/src/smstimerserver.c
	$(CC) $(SMS_INTERFACE_CFLAGS) internal/porting/src/smstimerserver.c

util_stub.o $(SMS_SERVER_LIB_DEPENDENCY_FILES) test_harness/stubs/common_functionality/util_stub.c:
	$(CC) $(SMS_INTERFACE_CFLAGS) test_harness/stubs/common_functionality/util_stub.c

clean:
	rm -f libsms_server.so libsms_server.a $(OBJS)

build: libsms_server.so libsms_server.a

all: build 

rebuild: clean build

install: build
	@$(PACKAGE_DIR) usr 755 0 0
	@$(PACKAGE_DIR) usr/lib 755 0 0
	@$(PACKAGE_FILE) usr/lib/libsms_server.so ./usr/lib/libsms_server.so 755 0 0

