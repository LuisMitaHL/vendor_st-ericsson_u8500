# vim:syntax=make:
# -*- Mode: Makefile -*-


include $(MM_MAKEFILES_DIR)/SharedConfig.mk



# C preprocessor flags
CPPFLAGS += \
-DNDEBUG -DOSCL_UNUSED_ARG= -DARM -DBUILD_FIXED -DNEON \
-I$(MM_HEADER_BASEDIR)/audio \
-I$(ANDROID_BSP_ROOT)/frameworks/base/media/libstagefright/include \
-I$(ANDROID_BSP_ROOT)/frameworks/base/include/media \
-I$(COMPONENT_TOP_DIR)/codec/include \

CFLAGS += \
-fvisibility=hidden \
-DPIC \
-g \
-Wall \

# Extra linker flags, libdl is needed by some guys.
LDFLAGS += -ldl\

#Extra search paths

ifeq ($(MMCOMPILER),android)
    COMPONENT_PATH:=/system/lib
else
    COMPONENT_PATH:=/usr/lib
endif

INSTALL_SOLIB_DIR := $(MMBUILDOUT)/rootfs/$(MMPLATFORM)$(COMPONENT_PATH)
SOLIBS_PATH := $(MMBUILDOUT)/rootfs/$(MMPLATFORM)$(COMPONENT_PATH)

LIBS_PATH += $(SOLIBS_PATH)




ifeq ($(MMSYSTEM), android)
SOLIBNAMES += stagefright_ste_mp3decode
LIBOBJ_stagefright_ste_mp3decode = ste_mp3decode_wrapper


INSTALL_SOLIB += stagefright_ste_mp3decode
  FLAGS_stagefright_ste_mp3decode += \
    -L$(MMBUILDOUT)/lib/cortexA9-x-rvct4gnu-x/ -lmp3decode


endif

todo: solib
#-------------------------------
# Generic Rules - Builds executables / libs according to above
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

