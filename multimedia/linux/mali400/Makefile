CROSS_COMPILE?=arm-none-linux-gnueabi-
# arm-linux-gcc makefile is taking CROSS_COMPILE as prefix so needs to be exported
export CROSS_COMPILE

TARGET_PLATFORM:=ux500 #use driver/build_system/platform/ux500.mak

ifneq (,$(findstring -android,$(PLATFORM)))
  TARGET_TOOLCHAIN:=arm-linux-gcc-vfp
else
  TARGET_TOOLCHAIN:=arm-linux-gcc
endif

CONFIG?=release

PLATFORM?=u8500_v2-linux
ifneq (,$(findstring href_v1,$(PLATFORM)))
  MALI_VERSION:=mali400-r0p1
else
  MALI_VERSION:=mali400-r1p0
endif

PWD:=$(realpath $(CURDIR))

# lbp
ifneq (,$(findstring -linux,$(PLATFORM)))
  SUB_VARIANT:=smp-gles11-gles20-vg-ump-hwmem-monolithic-linux
  INSTALL_ROOT?=$(PWD)/out
  INSTALL_HDR_PATH?=$(INSTALL_ROOT)/include
  ifneq (,$(findstring -nullws,$(PLATFORM)))
    SUB_VARIANT:=smp-gles11-gles20-vg-ump-hwmem-monolithic-linux-nullws
  endif
endif

# android
ifneq (,$(findstring -android,$(PLATFORM)))
  SUB_VARIANT:=smp-gles11-gles20-ump-hwmem-linux-android-rgb_is_xrgb-jellybean-no_monolithic
  INSTALL_ROOT:=$(ANDROID_PRODUCT_OUT)
  ANDROID_ABS_ROOT:=$(realpath $(ANDROID_BUILD_TOP))

  #check if current folder is in a (non-symlink) subtree of android root folder
  ifeq (,$(findstring $(ANDROID_ABS_ROOT),$(PWD)))
    #..if not then hardcode the relative path
    ANDROID_RELATIVE_PATH:=vendor/st-ericsson/multimedia/linux/mali400
  else
    #..else remove ANDROID_ABS_ROOT from the full path to get a relative path
    ANDROID_RELATIVE_PATH:=$(subst $(ANDROID_ABS_ROOT)/,,$(PWD))
  endif
endif

# MeeGo
ifneq (,$(findstring -meego,$(PLATFORM)))
  SUB_VARIANT:=smp-gles11-gles20-vg-ump-hwmem-monolithic-x11-linux
  INSTALL_ROOT?=$(PWD)/out
  INSTALL_HDR_PATH?=$(INSTALL_ROOT)/include
endif

ifeq (1,$(MALI_INSTRUMENTED))
PACKAGE_DIR:=$(PWD)/$(PLATFORM)-instrumented
VARIANT:=$(MALI_VERSION)-$(SUB_VARIANT)-instrumented
LIB_FLAGS:=CONFIG=$(CONFIG) TARGET_PLATFORM=$(TARGET_PLATFORM) TARGET_TOOLCHAIN=$(TARGET_TOOLCHAIN) CONFIG_FILE=instrumented_full MALI_EXTRA_ENVIRONMENT_FILE=/tmp/malienv.txt MALI_INSTRUMENTED=TRUE
else
PACKAGE_DIR:=$(PWD)/$(PLATFORM)
VARIANT:=$(MALI_VERSION)-$(SUB_VARIANT)
LIB_FLAGS:=CONFIG=$(CONFIG) TARGET_PLATFORM=$(TARGET_PLATFORM) TARGET_TOOLCHAIN=$(TARGET_TOOLCHAIN)
endif

## make targets #######################
.PHONY: all android check
.NOTPARALLEL: all

ifeq ($(BUILD_ARM_CODE),bin)
all: install
else
ifneq (,$(findstring linux,$(PLATFORM)))
all: libs tests package-lbp package-tests install
endif
ifneq (,$(findstring android,$(PLATFORM)))
all: libs android-libs package-android install
endif
ifneq (,$(findstring meego,$(PLATFORM)))
all: libs package-lbp
endif
endif #ifeq ($(BUILD_ARM_CODE),bin)

## Build ###############

#this is just for building outside of the full build system
android:
	$(MAKE) PLATFORM=u8500_v2-android

#invoke Android's build system for Mali
android-libs:
	@if [ ! -f $(ANDROID_BUILD_TOP)/$(ANDROID_RELATIVE_PATH)/driver/Android.mk ] ; then echo "Mali's Android.mk not found"; exit 1 ; fi
	@echo "#\n"\
	"# Mali400 is built from the multimedia build system and should not be\n"\
	"# built by Android's automatic recursion of folders looking for\n"\
	"# Android.mk files. This auto generated dummy Android.mk prevents further\n"\
	"# recursion down to driver/Android.mk\n"\
	"#\n" > Android.mk

	ONE_SHOT_MAKEFILE=$(ANDROID_RELATIVE_PATH)/driver/Android.mk $(MAKE) -C $(ANDROID_BUILD_TOP) files

#for Android this will just generate Android.mk and egl.cfg
#for linux this will build the libs
libs:
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT)

tests:
#build for all API combinations
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT) bin/platform_test_suite
#build if GLES 1.1 but not GLES 2.0
ifneq (,$(findstring -gles11-,-$(VARIANT)-))
ifeq (,$(findstring -gles20-,-$(VARIANT)-))
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT) bin/egl_api_main_suite_11
endif
endif
#build if GLES 2.0
ifneq (,$(findstring -gles20-,-$(VARIANT)-))
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT) bin/egl_api_main_suite_20 \
		bin/gles2_api_suite
endif
#build if VG
ifneq (,$(findstring -vg-,-$(VARIANT)-))
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT) bin/vg_api_tests
endif
#build if *only* VG
ifneq (,$(findstring -vg-,-$(VARIANT)-))
ifeq (,$(findstring -gles11-,-$(VARIANT)-))
ifeq (,$(findstring -gles20-,-$(VARIANT)-))
	$(MAKE) -C driver $(LIB_FLAGS) VARIANT=$(VARIANT) bin/egl_api_main_suite
endif
endif
endif

## Clean ###############

clean:
	$(MAKE) clean -C driver $(LIB_FLAGS) VARIANT=$(VARIANT)
	rm -f ./Android.mk

android-clean:
	rm -Rf $(ANDROID_PRODUCT_OUT)/obj/SHARED_LIBRARIES/libMali_intermediates
	rm -f $(ANDROID_PRODUCT_OUT)/obj/lib/libMali.so
	rm -f $(ANDROID_PRODUCT_OUT)/symbols/system/lib/libMali.so
	rm -f $(ANDROID_PRODUCT_OUT)/system/lib/libMali.so
	rm -Rf $(ANDROID_PRODUCT_OUT)/obj/SHARED_LIBRARIES/libUMP_intermediates
	rm -f $(ANDROID_PRODUCT_OUT)/obj/lib/libUMP.so
	rm -f $(ANDROID_PRODUCT_OUT)/symbols/system/lib/libUMP.so
	rm -f $(ANDROID_PRODUCT_OUT)/system/lib/libUMP.so
	rm -Rf $(ANDROID_PRODUCT_OUT)/obj/SHARED_LIBRARIES/lib*_mali_intermediates
	rm -f $(ANDROID_PRODUCT_OUT)/obj/lib/lib*_mali.so
	rm -f $(ANDROID_PRODUCT_OUT)/symbols/system/lib/lib*_mali.so
	rm -f $(ANDROID_PRODUCT_OUT)/system/lib/egl/lib*_mali.so
	rm -f $(ANDROID_PRODUCT_OUT)/system/lib/egl/egl.cfg

realclean: clean
	rm -Rf out
	#remove the pre-built binaries *if* the source code is available
	@if [ -d $(PWD)/driver ] ; then rm -Rf $(PACKAGE_DIR) ; fi

distclean: realclean

## Install #############

install: install-package

install-package:
	mkdir -p $(INSTALL_ROOT)
	cp -af $(PACKAGE_DIR)/* $(INSTALL_ROOT)

package-tests: tests
	mkdir -p $(PACKAGE_DIR)/usr/bin
	mkdir -p $(PACKAGE_DIR)/lib/
	cp -af driver/bin/platform_test_suite $(PACKAGE_DIR)/usr/bin
	cp -af driver/bin/egl_api_main_suite* $(PACKAGE_DIR)/usr/bin
ifneq (,$(findstring -gles20-,-$(VARIANT)-))
	mkdir -p $(PACKAGE_DIR)/mali/gles2_test/shaders
	cp -af driver/testbench/api_tests/opengles2/shaders/* $(PACKAGE_DIR)/mali/gles2_test/shaders/
	cp -af driver/bin/gles2_api_suite $(PACKAGE_DIR)/usr/bin
	mkdir -p $(PACKAGE_DIR)/mali/egl_test/shaders
	cp -af driver/testbench/api_tests/egl/shaders/* $(PACKAGE_DIR)/mali/egl_test/shaders/
endif
ifneq (,$(findstring -vg-,-$(VARIANT)-))
	mkdir -p $(PACKAGE_DIR)/mali/vg_test/failures
	cp -af driver/bin/vg_api_tests $(PACKAGE_DIR)/usr/bin
endif

package-lbp: libs
	mkdir -p $(PACKAGE_DIR)/lib $(INSTALL_HDR_PATH)
	cp -af driver/lib/* $(PACKAGE_DIR)/lib
        #official khronos headers
	cp -af driver/3rdparty/include/khronos/* $(INSTALL_HDR_PATH)
        #platform specific khronos headers
	cp -af driver/include/KHR $(INSTALL_HDR_PATH)
	cp -af driver/include/EGL/platform_fbdev/EGL/eglplatform.h $(INSTALL_HDR_PATH)/EGL/
	cp -af driver/include/EGL/fbdev_window.h $(INSTALL_HDR_PATH)/EGL/
	cp -af driver/include/VG/vgplatform.h $(INSTALL_HDR_PATH)/VG/

package-android:
	mkdir -p $(PACKAGE_DIR)/system/lib/egl/
	cp -a $(ANDROID_PRODUCT_OUT)/system/lib/libMali.so $(PACKAGE_DIR)/system/lib/
	cp -a $(ANDROID_PRODUCT_OUT)/system/lib/libUMP.so $(PACKAGE_DIR)/system/lib/
	cp -a $(ANDROID_PRODUCT_OUT)/system/lib/egl/*mali.so $(PACKAGE_DIR)/system/lib/egl/
	cp -a $(ANDROID_PRODUCT_OUT)/system/lib/egl/egl.cfg $(PACKAGE_DIR)/system/lib/egl/

sync:
	adb remount
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/egl/libGLESv1_CM_mali.so /system/lib/egl/
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/egl/libGLESv2_mali.so /system/lib/egl/
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/egl/libEGL_mali.so /system/lib/egl/
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/egl/egl.cfg /system/lib/egl/
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/libMali.so /system/lib/
	adb push $(ANDROID_PRODUCT_OUT)/system/lib/libUMP.so /system/lib/

printenv:
	@echo "\033[0;44m"
	@echo "ANDROID_PRODUCT_OUT: " $(ANDROID_PRODUCT_OUT)
	@echo "ANDROID_BUILD_TOP: " $(ANDROID_BUILD_TOP)
	@echo "KERNEL_BUILD_DIR: " $(KERNEL_BUILD_DIR)
	@echo "MM_INSTALL_DIR: " $(MM_INSTALL_DIR)
	@echo "PLATFORM: " $(PLATFORM)
	@echo "MMROOT: " $(MMROOT)
	@echo "INSTALL_MOD_PATH: " $(INSTALL_MOD_PATH)
	@echo "PACKAGE_DIR: " $(PACKAGE_DIR)
	@echo "VARIANT: " $(VARIANT)
	@echo "ANDROID_ABS_ROOT: " $(ANDROID_ABS_ROOT)
	@echo "PWD: " $(PWD)
	@echo "CURDIR: " $(CURDIR)
	@echo "ANDROID_RELATIVE_PATH: " $(ANDROID_RELATIVE_PATH)
	@echo "\033[0m"

#Debug printing of variable: Usage make print-MYVAR
print-%: ; @echo $* is $($*)
