#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#-------------------------------------------

ifneq ($(PLATFORM),x86-linux)

RAMDISK_DIR = $(INSTALL_ROOTFS_DIR)
ifeq ($(MMSYSTEM),android)
RAMDISK_DIR = $(INSTALL_ROOTFS_DIR)/system
endif

TOBECLEAN += $(RAMDISK_DIR)/usr/share/camera/tuning/*010045*.*
TOBECLEAN += $(RAMDISK_DIR)/usr/share/camera/tuning/*020045*.*

install: ramdisk
todo: ramdisk

# Copy the tuning files into the Linux ramdisk
ramdisk: Makefile
	$(MMINSTALL) -d ./files/ $(RAMDISK_DIR)/usr/share/camera/tuning;

endif
#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
