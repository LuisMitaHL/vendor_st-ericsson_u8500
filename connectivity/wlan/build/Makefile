KERNEL_FLAGS = ARCH=$(ARCH) TOPLEVEL=$(TOPLEVEL) CROSS_COMPILE=$(CROSS_COMPILE) CROSS_PREFIX=$(CROSS_PREFIX) WLAN_SET_PLATFORM=$(WLAN_SET_PLATFORM) WLAN_SET_DUALBAND=$(WLAN_SET_DUALBAND)

CP = cp

CW1200_WLAN_DIR = $(realpath $(CURDIR)/../cw1200_wlan)
# MMC_PROXY_DIR = $(realpath $(CURDIR)/../mmc_proxy)
# KINTEROP_DIR = $(realpath $(CURDIR)/../kinterop)
UMAC_LIB_DIR = $(realpath $(CURDIR)/../umac_lib)
COMPAT_WIRELESS_DIR = $(realpath $(CURDIR)/../compat-wireless)

ifeq ($(MMC_PROXY_DIR),)
SDIO_DONGLE = n
else
SDIO_DONGLE = y
endif

.PHONY: default
default: help

.PHONY: help
help:
	@echo "****  WLAN Makefile for Fairbanks  ****"
	@echo "Available make targets"
	@echo "* all - runs build"
	@echo "* build-compat-wireless - builds the compat-wireless"
	@echo "* build - builds the WLAN subsystem"
	@echo "* clean - cleans all build files"
	@echo "* install - install WLAN kernel modules"

.PHONY: all
all:
	$(MAKE) -f fairbanks.mak build
	@echo $(DATE)

.PHONY: build
build: clean-umac_lib build-compat-wireless build-umac_lib build-cw1200_wlan build-mmc_proxy-$(SDIO_DONGLE)

.PHONY: install
install: install-compat-wireless install-cw1200_wlan install-mmc_proxy-$(SDIO_DONGLE)

.PHONY: clean
clean: clean-compat-wireless clean-umac_lib clean-cw1200_wlan clean-mmc_proxy-$(SDIO_DONGLE)

# Builds compat-wireless
.PHONY: build-compat-wireless
build-compat-wireless:
	$(MAKE) -C $(COMPAT_WIRELESS_DIR) $(KERNEL_FLAGS) KLIB_BUILD=$(KERNELOUT) -j$(JOBS) modules

# Builds STE UMAC library
.PHONY: build-umac_lib
build-umac_lib:
	$(MAKE) -C $(UMAC_LIB_DIR) -f proj_host_umac/build/Makefile $(KERNEL_FLAGS) KLIB_BUILD=$(KERNELDIR) ARCH_VERSION=$(ARCH_VERSION) -j$(JOBS)

build-cw1200_wlan_only: build-umac_lib
	$(CP) $(UMAC_LIB_DIR)/proj_host_umac.lib $(CW1200_WLAN_DIR)/lib/umac.lib
	$(MAKE) $(KERNEL_FLAGS) -C $(KERNELOUT)			\
		M=$(CW1200_WLAN_DIR)				\
		COMPAT_WIRELESS=$(COMPAT_WIRELESS_DIR)	 	\
		UMAC_LIB=$(UMAC_LIB_DIR) 			\
		-j$(JOBS) modules

# Builds STE CW1200 UMAC driver
.PHONY: build-cw1200_wlan
build-cw1200_wlan: build-umac_lib build-compat-wireless
	$(CP) $(COMPAT_WIRELESS_DIR)/Module.symvers $(CW1200_WLAN_DIR)/Module.symvers
	$(CP) $(UMAC_LIB_DIR)/proj_host_umac.lib $(CW1200_WLAN_DIR)/lib/umac.lib
	$(MAKE) $(KERNEL_FLAGS) -C $(KERNELOUT)			\
		M=$(CW1200_WLAN_DIR)				\
		COMPAT_WIRELESS=$(COMPAT_WIRELESS_DIR)	 	\
		UMAC_LIB=$(UMAC_LIB_DIR) 			\
		-j$(JOBS) modules

# Maybe build STE MMC proxy
.PHONY: build-mmc_proxy-n
build-mmc_proxy-n:

.PHONY: build-mmc_proxy-y
build-mmc_proxy-y: build-kinterop
	$(CP) $(KINTEROP_DIR)/Module.symvers $(MMC_PROXY_DIR)/Module.symvers
	$(MAKE) $(KERNEL_FLAGS) -C $(KERNELOUT) M=$(MMC_PROXY_DIR) INTEROPDIR=$(KINTEROP_DIR) -j$(JOBS) modules

.PHONY: build-kinterop
build-kinterop:
	$(MAKE) $(KERNEL_FLAGS) -C $(KERNELOUT) M=$(KINTEROP_DIR) -j$(JOBS) modules

.PHONY:	install-compat-wireless
install-compat-wireless:
	# compat-wireless: New location, matches upstream
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/net/mac80211/
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/net/rfkill/
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/net/wireless/
	rm -f  $(INSTALL_MOD_PATH)/lib/modules/*/kernel/drivers/net/usb/cdc_ether.ko
	rm -f  $(INSTALL_MOD_PATH)/lib/modules/*/kernel/drivers/net/usb/usbnet.ko
	rm -f  $(INSTALL_MOD_PATH)/lib/modules/*/kernel/drivers/net/usb/rndis_host.ko
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/drivers/net/wireless/
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/kernel/net/wireless/
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/kernel/net/mac80211/
	# Lets only remove the stuff we are sure we are providing
	# on the misc directory.
	rm -f $(INSTALL_MOD_PATH)/lib/modules/*/drivers/misc/eeprom/eeprom_93cx6.ko*
	rm -f $(INSTALL_MOD_PATH)/lib/modules/*/drivers/misc/eeprom_93cx6.ko*
	rm -f $(INSTALL_MOD_PATH)/lib/modules/*/drivers/net/b44.ko*
	make -C $(KERNELOUT) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)  M=$(COMPAT_WIRELESS_DIR) INSTALL_MOD_DIR=compat-wireless -j$(JOBS) modules_install
	# Remove all compat_wireless drivers.
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/compat-wireless/drivers/net/wireless
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/compat-wireless/drivers/misc

.PHONY: install-cw1200_wlan
install-cw1200_wlan: install-compat-wireless
	make -C $(KERNELOUT) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)  M=$(CW1200_WLAN_DIR) INSTALL_MOD_DIR=extra -j$(JOBS) modules_install

.PHONY: install-mmc_proxy-n
install-mmc_proxy-n:

.PHONY: install-mmc_proxy-y
install-mmc_proxy-y: install-kinterop
	make -C $(KERNELOUT) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)  M=$(MMC_PROXY_DIR) INSTALL_MOD_DIR=extra -j$(JOBS) modules_install

.PHONY: install-kinterop
install-kinterop:
	make -C $(KERNELOUT) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)  M=$(KINTEROP_DIR) INSTALL_MOD_DIR=extra -j$(JOBS) modules_install

.PHONY: clean-compat-wireless
clean-compat-wireless:
	$(MAKE) -C $(COMPAT_WIRELESS_DIR) $(KERNEL_FLAGS) KLIB_BUILD=$(KERNELOUT) -j$(JOBS) clean
.PHONY: clean-umac_lib
clean-umac_lib:
	$(MAKE) -C $(UMAC_LIB_DIR) -f proj_host_umac/build/Makefile $(KERNEL_FLAGS) KLIB_BUILD=$(KERNELDIR) ARCH_VERSION=$(ARCH_VERSION) -j$(JOBS) clean
.PHONY: clean-cw1200_wlan
clean-cw1200_wlan:
	$(MAKE) -C $(CW1200_WLAN_DIR) $(KERNEL_FLAGS) -j$(JOBS) clean
.PHONY: clean-mmc_proxy-y
clean-mmc_proxy-y: clean-kinterop
	$(MAKE) -C $(MMC_PROXY_DIR) $(KERNEL_FLAGS) -j$(JOBS) clean
.PHONY: clean-mmc_proxy-n
clean-mmc_proxy-n:
.PHONY: clean-kinterop
clean-kinterop:
	$(MAKE) -C $(KINTEROP_DIR) $(KERNEL_FLAGS) -j$(JOBS) clean

.PHONY: distclean config distclean-cw1200_wlan

config:

distclean: clean

distclean-cw1200_wlan: clean-cw1200_wlan