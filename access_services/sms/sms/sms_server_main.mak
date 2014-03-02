#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
PACKAGE_NAME=sms_server-0.1
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

ACCESS_SERVICES_PATH := ../..

################################################################
# Configure modem paths and shared object information depending
# on which modem is being used and whether stubs are being used.
#
# MODEM_LIBRARY_CC_INCLUDE_PATH is the -I flag passed to CC when
# compiling files to ensure that all header files are found.
#
# MODEM_LIBRARY_CC_SO_PATH is the -L flag passed to CC when
# linking object files together.
#
# MODEM_LIBRARY_SO is the list of shared objects which are are
# passed to CC when linking objects together.
################################################################
ifneq ($(CFG_ENABLE_SMS_MODEM_STUB), TRUE)
# Using real modem interface, not stubs.

ifeq ($(SMS_SET_FEATURE_BACKEND),MFL)
#Using MFL modem.
MODEM_LIBRARY_CC_INCLUDE_PATH := -I$(ACCESS_SERVICES_PATH)/mfl_framework/include \
                                 -I$(ACCESS_SERVICES_PATH)/mfl_proxy_cas/include
# TODO fix paths / Shared Object Info when known. (Don't forget -L flag!)
MODEM_LIBRARY_CC_SO_PATH      := 
MODEM_LIBRARY_SO              := 
else
# Using MAL modem.
MODEM_LIBRARY_CC_INCLUDE_PATH := -I$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include \
									-I$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
# TODO fix paths / Shared Object Info when known. (Don't forget -L flag!)
MODEM_LIBRARY_CC_SO_PATH      := 
MODEM_LIBRARY_SO              := 
endif

else
# Using SMS server stubbed version of modem interface.
# We have a local version of the modem API's which are
# needed for testing.
# It has its own main() function and is not a shared
# object library.

ifeq ($(SMS_SET_FEATURE_BACKEND),MFL)
#Using MFL modem stubs.
MODEM_LIBRARY_CC_INCLUDE_PATH := -I./test_harness/stubs/mfl/include
else
# Using MAL modem stubs.
MODEM_LIBRARY_CC_INCLUDE_PATH := -I./test_harness/stubs/mal/include \
				 -I$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include \
				 -I$(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
endif
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SIM_LIBRARY_INCLUDE_PATH := -I./test_harness/stubs/sim/include \
                            -I../../sim/sim/include \
                            -I../../sim/sim/libsimcom
else
SIM_LIBRARY_INCLUDE_PATH :=
endif

# If we are using one of the test harnesses, configure a flag to
# select the correct version of c_sms_config.c to be built.
CFG_SMS_TEST_HARNESS_USE_C_SMS_CONFIG_TEST := FALSE
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), TRUE)
CFG_SMS_TEST_HARNESS_USE_C_SMS_CONFIG_TEST := TRUE
endif
ifeq ($(CFG_ENABLE_SMS_LTP_TEST_HARNESS), TRUE)
CFG_SMS_TEST_HARNESS_USE_C_SMS_CONFIG_TEST := TRUE
endif

################################################################
# Information about where sms_server shared object is located.
################################################################
SMS_LIBRARY_SO_PATH      :=
SMS_LIBRARY_SO           := ./libsms_server.so

LIBRARY_SHARED_OBJECTS   := $(SMS_LIBRARY_SO) $(MODEM_LIBRARY_SO)

ALL_INCLUDE_PATHS := $(ALL_INCLUDE_PATHS) \
                     -I../../modem_adaptations/mal/modem_lib/mal/libmalsms/include \
                     -I../../modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include \
                     -I../../call_network/call_network/libcn/include \
                     -I../../common/common_functionality/include \
                     -I../../../processing/security_framework/cops/cops-api/include \
                     -I./include \
                     -I./config/include \
                     -I./internal/main/include \
                     -I./internal/porting/include \
                     -I./internal/sm/include \
                     -I./internal/cb/include \
                     -I./usr/include \
                     $(MODEM_LIBRARY_CC_INCLUDE_PATH) \
                     $(SIM_LIBRARY_INCLUDE_PATH)

# The following stricter CFLAGS can be used to ensure that the SMS code is of higher quality.
ifeq ($(CFG_ENABLE_SMS_STRICT_CC_FLAGS), TRUE)
EXTRA_CFLAGS := -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror -Werror-implicit-function-declaration \
                -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations \
                -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow \
                -Wstrict-prototypes -Wswitch-default -Wwrite-strings -std=c99 \
                -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -fno-common \
                -Winline -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 \
                -Wundef -pedantic
endif

# TODO

CFLAGS := -g -c -Wall $(ARCHFLAGS) -O2 $(GCOV_FLAGS) $(EXTRA_CFLAGS) $(STAGING_AREA) $(CFLAGS) $(ALL_INCLUDE_PATHS)

# Signalling code ported from OSE builds with a large number of "dereferencing type-punned pointer will
# break strict-aliasing rules" warnings. Until / unless this is fixed disable the strict aliasing; this
# will prevent the compiler doing some optimisation.
CFLAGS := $(CFLAGS) -fno-strict-aliasing

SMS_CFLAGS := $(CFLAGS) -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED -DSMS_PRINT_A_
LDFLAGS    := $(LDFLAGS) -L$(STAGING_AREA)/usr/lib32 -L$(SMS_LIBRARY_SO_PATH) -lrt

ifdef MODEM_LIBRARY_CC_SO_PATH
# If MODEM_LIBRARY_CC_SO_PATH is set to anything, add it to the paths used
# by the linker.
LDFLAGS    := $(LDFLAGS) $(MODEM_LIBRARY_CC_SO_PATH)
endif

# Set compile flag for modem being used. The default, in the source code, is the MAL modem
# which does not have its own compiler switch.
ifeq ($(SMS_SET_FEATURE_BACKEND),MFL)
SMS_CFLAGS := $(SMS_CFLAGS) -DSMS_USE_MFL_MODEM
endif

# Enable some extra SMS logging.
ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING), TRUE)
SMS_CFLAGS := $(SMS_CFLAGS) -DSMS_PRINT_B_
endif

# Enable all SMS logging.
ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING), TRUE)
SMS_CFLAGS := $(SMS_CFLAGS) -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

ifeq ($(CFG_REMOVE_SMS_STORAGE_LAYER), TRUE)
SMS_CFLAGS := $(SMS_CFLAGS) -DREMOVE_SMSTRG_LAYER
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SMS_CFLAGS := $(SMS_CFLAGS) -DSMS_SIM_TEST_PLATFORM
endif

OBJS := sms_log_client.o smsmain.o smsevnthndr.o smslist.o sm.o smbearer.o smmngr.o smotadp.o smrouter.o smsimtkt.o smtpdu.o smutil.o cbmngr.o cbmsgs.o cbsubscribe.o cn_client_stub.o

ifeq ($(CFG_SMS_TEST_HARNESS_USE_C_SMS_CONFIG_TEST), TRUE)
# Using test harness so link in a special version of c_sms_config.c which includes support for storage.
OBJS := $(OBJS) c_sms_config_test.o
else
OBJS := $(OBJS) c_sms_config.o
endif

ifneq ($(CFG_REMOVE_SMS_STORAGE_LAYER), TRUE)
OBJS := $(OBJS) smstrg.o
endif

ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
OBJS := $(OBJS) client_side_mal_sms.o
endif

ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
OBJS := $(OBJS) client_side_mfl_framework_stub.o mfl_cas_stub.o
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
OBJS := $(OBJS) sim_util.o sim.o
endif

ifeq ($(CFG_SMS_USE_COPS_STUBS), TRUE)
OBJS := $(OBJS) cops_api_client_osfree.o
endif

SMS_SERVER_DEPENDENCY_FILES := \
	./include/t_sms.h \
	./include/r_sms.h \
	./include/t_cbs.h \
	./include/r_cbs.h \
	./include/t_sms_cb_session.h \
	./include/r_sms_cb_session.h \
	./config/include/c_sms_config.h \
	./internal/main/include/d_sms.h \
	./internal/main/include/g_sms.h \
	./internal/main/include/g_cbs.h \
	./internal/main/include/smsevnthndr.h \
	./internal/main/include/smslist.h \
	./internal/main/include/smsmain.h \
	./internal/porting/include/smsipc.h \
	./internal/porting/include/t_smslinuxporting.h \
	./internal/porting/include/r_smslinuxporting.h \
	./internal/sm/include/sm.h \
	./internal/sm/include/smbearer.h \
	./internal/sm/include/smmngr.h \
	./internal/sm/include/smotadp.h \
	./internal/sm/include/smrouter.h \
	./internal/sm/include/smsimtkt.h \
	./internal/sm/include/smstrg.h \
	./internal/sm/include/smtpdu.h \
	./internal/sm/include/smutil.h \
	./internal/cb/include/cbmngr.h \
	./internal/cb/include/cbmsgs.h \
	./internal/cb/include/cbsubscribe.h \
	./sms_server_main.mak \
	./sms_server.mak

ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
SMS_SERVER_DEPENDENCY_FILES := $(SMS_SERVER_DEPENDENCY_FILES) ../../modem_adaptations/mal/modem_lib/mal/libmalsms/include/mal_sms.h ../../modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
SMS_SERVER_DEPENDENCY_FILES := $(SMS_SERVER_DEPENDENCY_FILES) ./test_harness/stubs/sim/include/sim_stub.h \
	                                                      ./test_harness/stubs/sim/include/sim_util.h
endif

# If make file is called with "sms_server" then build the SMS Server code.
# IMPORTANT NOTE. This make file includes all header files in its dependency tree.
# This will slow the build down slightly if one of these files is updated as it could
# cause more SMS_SERVER files to be re-built than absolutely necessary; however it is
# safer and the SMS Server should be fairly quick to build.
# At a later date, it may be possible to determine the dependencies at build time.

sms_server: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBRARY_SHARED_OBJECTS) -o sms_server $(GCOV_LIBS) $(ARCHFLAGS)

sms_log_client.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/main/src/sms_log_client.c
	$(CC) $(SMS_CFLAGS) internal/main/src/sms_log_client.c

smsmain.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/main/src/smsmain.c
	$(CC) $(SMS_CFLAGS) internal/main/src/smsmain.c

smsevnthndr.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/main/src/smsevnthndr.c
	$(CC) $(SMS_CFLAGS) internal/main/src/smsevnthndr.c

smslist.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/main/src/smslist.c
	$(CC) $(SMS_CFLAGS) internal/main/src/smslist.c

ifeq ($(CFG_SMS_TEST_HARNESS_USE_C_SMS_CONFIG_TEST), TRUE)
# Using test harness so link in a special version of c_sms_config.c which includes support for storage.
c_sms_config_test.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/config/c_sms_config_test.c
	$(CC) $(SMS_CFLAGS) test_harness/config/c_sms_config_test.c
else
c_sms_config.o: $(SMS_SERVER_DEPENDENCY_FILES) config/src/c_sms_config.c
	$(CC) $(SMS_CFLAGS) config/src/c_sms_config.c
endif

sm.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/sm.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/sm.c

smbearer.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smbearer.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smbearer.c

smmngr.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smmngr.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smmngr.c

smotadp.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smotadp.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smotadp.c

smrouter.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smrouter.c
	$(CC) $(SMS_CFLAGS) -DSMS_PRINT_B_ internal/sm/src/smrouter.c

smsimtkt.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smsimtkt.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smsimtkt.c

ifneq ($(CFG_REMOVE_SMS_STORAGE_LAYER), TRUE)
smstrg.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smstrg.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smstrg.c
endif

smtpdu.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smtpdu.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smtpdu.c

smutil.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/sm/src/smutil.c
	$(CC) $(SMS_CFLAGS) internal/sm/src/smutil.c

cbmngr.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/cb/src/cbmngr.c
	$(CC) $(SMS_CFLAGS) internal/cb/src/cbmngr.c

cbmsgs.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/cb/src/cbmsgs.c
	$(CC) $(SMS_CFLAGS) internal/cb/src/cbmsgs.c

cbsubscribe.o: $(SMS_SERVER_DEPENDENCY_FILES) internal/cb/src/cbsubscribe.c
	$(CC) $(SMS_CFLAGS) internal/cb/src/cbsubscribe.c

cn_client_stub.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/call_network/libcn/src/cn_client_stub.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/call_network/libcn/src/cn_client_stub.c

ifeq ($(CFG_SMS_USE_MAL_MODEM_STUBS), TRUE)
client_side_mal_sms.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/mal/src/client_side_mal_sms.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/mal/src/client_side_mal_sms.c
endif

ifeq ($(CFG_SMS_USE_MFL_MODEM_STUBS), TRUE)
client_side_mfl_framework_stub.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/mfl/src/client_side_mfl_framework_stub.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/mfl/src/client_side_mfl_framework_stub.c

mfl_cas_stub.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/mfl/src/mfl_cas_stub.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/mfl/src/mfl_cas_stub.c
endif

ifeq ($(CFG_SMS_USE_SIM_STUBS), TRUE)
sim_util.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/sim/libsim/sim_util.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/sim/libsim/sim_util.c

sim.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/sim/libsim/sim.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/sim/libsim/sim.c
endif

ifeq ($(CFG_SMS_USE_COPS_STUBS), TRUE)
cops_api_client_osfree.o: $(SMS_SERVER_DEPENDENCY_FILES) test_harness/stubs/cops/cops-api/cops_api_client_osfree.c
	$(CC) $(SMS_CFLAGS) test_harness/stubs/cops/cops-api/cops_api_client_osfree.c
endif

install: build
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/sbin 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/sms_server $(CURDIR)/sms_server 755 0 0

build: sms_server

# If make file is called with "clean" then delete all object files and the built executable
clean:
	rm -f sms_server $(OBJS)

# If make file is called with "all" then build sms_server
all: sms_server

# If make file is called with "rebuild" then clean all files before building sms_server.
rebuild: clean sms_server
