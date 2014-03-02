ifeq ($(KERNELRELEASE),)

-include $(MMROOT)/linux/kernel/Make.config
include $(MMROOT)/shared/makefiles/Make.config


PWD := $(shell pwd)
SUBDIRS=$(PWD)

all: build install

build:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_BUILD_DIR) SUBDIRS=$(PWD) modules MMENV_BOARD=$(MMENV_BOARD)
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_BUILD_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(PWD) modules_install ; true

install:
	$(MMINSTALL) $(PWD)/st_readHwReg.ko $(INSTALL_ROOTFS_DIR)/system/usr/share/camera/st_readHwReg.ko

clean:
	rm -rf .*.cmd *.ko *.mod.c *.o Module.symvers .tmp_versions lib

realclean: clean

else

ifeq ($(MMENV_BOARD),mop500_ed)
EXTRA_CFLAGS += -DPLAT_MOP500_ED
else ifeq ($(MMENV_BOARD),href_v1)
EXTRA_CFLAGS += -DPLAT_HREF_V1
else ifeq ($(MMENV_BOARD),u8500_v2)
EXTRA_CFLAGS += -DPLAT_U8500_V2
else ifeq ($(MMENV_BOARD),u9540_v1)
EXTRA_CFLAGS += -DPLAT_U9540_V1
else ifeq ($(MMENV_BOARD),l8540_v1)
EXTRA_CFLAGS += -DPLAT_L8540_V1
else
$(error unsupported MMENV_BOARD=$(MMENV_BOARD))
endif
EXTRA_CFLAGS += -I$(SUBDIRS)/../common
EXTRA_CFLAGS += -DKANA_PWR_UP
obj-m := st_readHwReg.o

endif

