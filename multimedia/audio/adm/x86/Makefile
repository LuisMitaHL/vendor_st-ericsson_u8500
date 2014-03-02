	# Makefile for stand-alone building during development.
	# This makefile is not used while building for the product

	#Get the path to this makefile.
	MAKEDIR:=$(dir $(lastword $(MAKEFILE_LIST)))

	#Should point to a build root (the folder in which e.g. mm/audio/ is present).
	SYSROOT ?= ../../../../../../

	OMX_INCLUDE_FLAGS := \
		-I$(SYSROOT)/out/target/product/u8500/obj/include\
		-I$(SYSROOT)/external/alsa-lib/include \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/shared/omxil \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/shared \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/linux \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/audio \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/audio/audiolibs/common/include \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/audio/audiolibs/fake_dsptools \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/imaging \
		-I$(SYSROOT)/out/target/product/u8500/obj/mmbuildout/include/u8500_v2-android/video -DSRC_MODE_INTERNAL


#TODO: Separate for flags etc

	ADM_GCC_WARNINGS := -W -Wall -Wno-unused -Winit-self -Wpointer-arith -fno-exceptions -Wno-multichar \
 					    -Wextra -Wshadow -Wconversion -Wcast-qual  -DADM_NO_EFFECT_CONFIG_SUPPORT



	SRC_FILES_ADM := \
		adm_main.c \
		sqlite3.c \
		ste_adm_api_cscall.c \
		ste_adm_cscall_omx.c \
		ste_adm_api_modem.c \
		ste_adm_api_d2d.c \
		ste_adm_api_devset.c \
		ste_adm_api_util.c \
		ste_adm_db.c \
		ste_adm_db_cfg.c \
		ste_adm_db_test.c \
		ste_adm_db_remap_check.c \
		ste_adm_dev.c \
		ste_adm_dev_omx.c \
		ste_adm_dev_omx_graph.c \
		ste_adm_dev_omx_util.c \
		ste_adm_hw_handler.c \
		ste_adm_omx_io.c \
		ste_adm_omx_log.c \
		ste_adm_omx_tool.c \
		ste_adm_omx_core.c \
		ste_adm_platform_adaptions.c \
		ste_adm_client.c \
		ste_adm_dbg.c \
		ste_adm_main.c \
		ste_adm_srv.c \
		ste_adm_util.c \
		ste_adm_hw_handler_x86.c \
		src_stubs.c \
		mixer_stubs.c

	SRC_DIRS := . api/cscall api db dev hw_handler il_tool test
	VPATH := $(addprefix $(MAKEDIR)/../src/, $(SRC_DIRS))

	INCLUDE_DIRS := $(patsubst %,-I%,$(subst :, ,$(VPATH)))
	INCLUDE_DIRS += -I$(MAKEDIR)/../include



	ifneq ($(ADM_IL_PROCESS), 1)
		SRC_FILES_ADM += il_stubs.c
	endif


	BIONIC_CC  := $(SYSROOT)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-gcc
	BIONIC_CXX := $(SYSROOT)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-g++

	BIONIC_INCLUDE := 	-I$(SYSROOT)/bionic/libc/arch-arm/include \
				-I$(SYSROOT)/bionic/libc/include \
				-I$(SYSROOT)/bionic/libstdc++/include \
				-I$(SYSROOT)/bionic/libc/kernel/common \
				-I$(SYSROOT)/bionic/libc/kernel/arch-arm \
				-I$(SYSROOT)/bionic/libm/include \
				-I$(SYSROOT)/bionic/libthread_db/include

	ANDROID_INCLUDE := 	-I$(SYSROOT)/system/core/include \
				-I$(SYSROOT)/frameworks/base/include/media \
				-I$(SYSROOT)/frameworks/base/include


	BIONIC_CC_CMD := $(BIONIC_CC) $(BIONIC_INCLUDE) $(ANDROID_INCLUDE) $(INCLUDE_DIRS) -DANDROID -fmessage-length=0 \
			 -msoft-float -fpic \
			 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon \
		         -include $(SYSROOT)/system/core/include/arch/linux-arm/AndroidConfig.h \
			 -I $(SYSROOT)/android/system/core/include/arch/linux-arm/ \
                         -mthumb-interwork -DSK_RELEASE -DNDEBUG -g -Wstrict-aliasing=2 -finline-functions -fno-inline-functions-called-once \
			 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
		         -D__STN_8500=30 -D__CUT_30 -D__STN_4500=30 -DLINUX -D__ARM_LINUX -D__ARM_ANDROID -DANDROID \
			 -I$(SYSROOT)/android/system/core/include  -g \
			 -I$(SYSROOT)/android/   -D_POSIX_SOURCE

	BIONIC_LD_CMD := $(BIONIC_CC) -ldl  -nostdlib -Bdynamic -Wl,-T,$(SYSROOT)/build/core/armelf.x \
			 -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,--no-undefined \
			 -L $(SYSROOT)out/target/product/u8500/obj/lib \
			 -Wl,-rpath-link=$(SYSROOT)/out/target/product/u8500/obj/lib -lc -lm -lstdc++ \
			 $(SYSROOT)/out/target/product/u8500/obj/lib/crtbegin_dynamic.o \
			 -Wl,--start-group -lmedia -llos  -ldl -L$(SYSROOT)/vendor/st-ericsson/multimedia/shared/lib/cortexA9-android-android-u8500_v2 -Wl,--end-group

	BIONIC_LD_POSTCMD := $(SYSROOT)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/../lib/gcc/arm-eabi/4.4.0/thumb/interwork/libgcc.a



.PHONY: all
all : admd ste-adm-test
# admd-arm ste-adm-test-arm

# ~/prj/lcov/lcov-1.9/bin/lcov --directory . --capture --base-directory /home/qhenseg/adm-wt/adm/x86/ --output-file app.info

# C99 does not currently work on target for some bionic reason, but later add this: -std=c99 -D_POSIX_SOURCE
# -I$(SYSROOT)/vendor/st-ericsson/multimedia/audio/ld_anm/

# NOTE: Alsa 32-bit library on 64-bit Ubuntu: sudo apt-get install lib32asound2-dev

# Build for ARM/Bionic
arm-obj/%.o : %.c $(MAKEFILE_LIST) | arm-obj-dir
	@echo CC ARM $<
	@$(BIONIC_CC_CMD) -DADM_NO_EFFECT_CONFIG_SUPPORT \
	    -DADM_SOCKET_UID=0 -DADM_SOCKET_GID=0 -DADM_SOCKET_MODE=0700 \
		-DADM_LOG_ERRORS -DADM_LOG_WARNINGS -DADM_LOG_STATUS \
		-DADM_DISABLE_TONEGEN \
	    -I../include/ -I. -I../src -I../new -I../src/db -I../src/cscall $(OMX_INCLUDE_FLAGS) -I$(SYSROOT) -c $< -o $@

x86-obj/%.o : %.c $(MAKEFILE_LIST) | x86-obj-dir
	@echo CC X86 $<
	@gcc -m32 -fprofile-arcs -ftest-coverage -O0 -DADM_DBG_X86 $(ADM_GCC_WARNINGS) $(ADM_DEBUG)  -ggdb3 $(OMX_INCLUDE_FLAGS) -I$(SYSROOT) \
		-c $< -o $@ -DSTE_ADM_SOCK_PATH=\"/tmp/admd-socket-$(shell id -nu)\" $(INCLUDE_DIRS) \
		-DADM_LOG_OMXIL -DADM_LOG_ERRORS -DADM_LOG_WARNINGS -DADM_LOG_STATUS -DADM_LOG_OMXIL_FLOW \
		-DADM_SOCKET_UID=0 -DADM_SOCKET_GID=0 -DADM_SOCKET_MODE=0700 -DADM_DISABLE_TONEGEN


admd-arm : $(addprefix arm-obj/,$(patsubst %.c,%.o,$(SRC_FILES_ADM) il_stubs.c))
	@echo LD ARM $@
	@$(BIONIC_LD_CMD) -lasound  $^ $(BIONIC_LD_POSTCMD) -o $@

admd : $(addprefix x86-obj/,$(patsubst %.c,%.o,$(SRC_FILES_ADM) il_stubs.c))
	@echo LD X86 $@
	@g++ -m32 -lpthread -lasound -fprofile-arcs -lgcov -o $@ $^


arm-obj-dir:
	mkdir -p arm-obj

x86-obj-dir:
	mkdir -p x86-obj

#
# Build sqlite wihtout any extra warnings, else it will spam the build log.
# It does not make sense to modify sqlite to get rid of the warnings.
#
arm-obj/sqlite3.o : sqlite3.c $(MAKEFILE_LIST)
	@echo CC ARM $<
	@$(BIONIC_CC_CMD) -O2 -c $< -o $@

x86-obj/sqlite3.o : sqlite3.c $(MAKEFILE_LIST)
	@echo CC ARM sqlite3.c
	@gcc -O0 -m32 -ggdb3 -DSQLITE_THREADSAFE=0 -DADM_DBG_X86 -I../include/ -I. \
		-I../src -I../src/db -I../src/cscall \
	  	-c $< -o $@ -DSTE_ADM_SOCK_PATH=\"/tmp/admd-socket-$(shell id -nu)\"

##
## Test client
##


SRC_FILES_TSTCLIENT := \
  test_client.c \
  cmdline_parser.c \
  ste_adm_client.c

ste-adm-test-arm : $(addprefix arm-obj/,$(patsubst %.c,%.o,$(SRC_FILES_TSTCLIENT) il_stubs.c))
	@echo LD ARM $@
	@$(BIONIC_LD_CMD) -lasound  $^ $(BIONIC_LD_SUFFIX) -o $@

ste-adm-test : $(addprefix x86-obj/,$(patsubst %.c,%.o,$(SRC_FILES_TSTCLIENT) il_stubs.c))
	@echo LD X86 $@
	@g++ -m32 -lpthread -lasound -fprofile-arcs -lgcov -o $@ $^


##
## Clean
##

.PHONY : clean
clean:
	find $(MAKEDIR)/.. -name "*.o" | xargs rm -f
	rm -f $(MAKEDIR)/ste-adm-test $(MAKEDIR)/admd
	rm -f $(MAKEDIR)/ste-adm-test-arm $(MAKEDIR)/admd-arm

##
## Whitespace
##

.PHONY: whitespace
whitespace :
	# Remove trailing spaces and tabs
	find $(MAKEDIR)/.. -name "*.c" -or -name "*.cpp" -or  -name "*.h" -or -name "*.txt" -or -name "*.sh" | xargs -n 1 sed -i 's/[ \t]*$$//'

    # Replace tab with 4 spaces
	find $(MAKEDIR)/.. -name "*.c" -or -name "*.cpp" -or -name "*.h" -or -name "*.txt" | xargs -n 1 sed -i 's/\t/    /g'


##
## LCOV
##

#
# Note: lcov shipping with Ubuntu 10.04 (1.7) does not work properly!
# 1.9 works.
#
LCOV_BIN_DIR ?= ~qhenseg/lcov/bin

.PHONY : lcov-reset
lcov-reset:
	cd .. && $(LCOV_BIN_DIR)/lcov --directory . --zerocounters

.PHONY : lcov
lcov:
	cd .. && $(LCOV_BIN_DIR)/lcov --directory . --capture --base-directory $(PWD) --output-file app.info
	cd .. && $(LCOV_BIN_DIR)/genhtml app.info

