LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ARCH:=arm

KEXEC_SRCS = kexec/kexec.c
KEXEC_SRCS += kexec/ifdown.c
KEXEC_SRCS += kexec/kexec-elf.c
KEXEC_SRCS += kexec/kexec-elf-exec.c
KEXEC_SRCS += kexec/kexec-elf-core.c
KEXEC_SRCS += kexec/kexec-elf-rel.c
KEXEC_SRCS += kexec/kexec-elf-boot.c
KEXEC_SRCS += kexec/kexec-iomem.c
KEXEC_SRCS += kexec/firmware_memmap.c
KEXEC_SRCS += kexec/crashdump.c
KEXEC_SRCS += kexec/crashdump-xen.c
KEXEC_SRCS += kexec/phys_arch.c
KEXEC_SRCS += kexec/lzma.c
KEXEC_SRCS += kexec/zlib.c
KEXEC_SRCS += \
	kexec/proc_iomem.c \
	kexec/virt_to_phys.c \
	kexec/add_segment.c \
	kexec/add_buffer.c \
	kexec/arch_reuse_initrd.c \
	kexec/arch_init.c

KEXEC_SRCS += \
	util_lib/compute_ip_checksum.c \
	util_lib/sha256.c

include $(LOCAL_PATH)/kexec/arch/$(ARCH)/Makefile
KEXEC_SRCS += $($(ARCH)_KEXEC_SRCS) $($(ARCH)_PHYS_TO_VIRT)

LOCAL_SRC_FILES:=\
	$(KEXEC_SRCS)

srcdir := $(LOCAL_PATH)

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
# use another name as kexec, since kexec is a subdirectory and Android make
# wants to create kexec, but directory kexec already exists.
LOCAL_MODULE := kexec.dyn

LOCAL_REQUIRED_MODULES := libz

LOCAL_SHARED_LIBRARIES := libz

LOCAL_C_INCLUDES := $(LOCAL_PATH)/android_config/include $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/util_lib/include $(LOCAL_PATH)/kexec/arch/arm/include external/zlib

LOCAL_CFLAGS = -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes
# bionic elf.h is used instead of kexec's, but it does not define ELFOSABI_NONE.
LOCAL_CFLAGS += -DELFOSABI_NONE=0
LOCAL_CFLAGS += -DR_ARM_ABS32=2
LOCAL_CFLAGS += -DR_ARM_REL32=3

include $(BUILD_EXECUTABLE)
