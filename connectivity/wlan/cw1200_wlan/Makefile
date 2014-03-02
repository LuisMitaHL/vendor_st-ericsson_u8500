ifeq ($(ARCH),)
        ARCH := arm
        KERNEL_ARCH = arm
endif

ifeq ($(src),)
        BASEDIR = $(realpath $(CURDIR))
else
        BASEDIR = $(realpath $(src))
endif

include $(BASEDIR)/.config
include $(BASEDIR)/$(ARCH).mk

COMPAT_WIRELESS ?= ../compat-wireless
UMAC_LIB ?= ../umac_lib

CP = cp
NOSTDINC_FLAGS = -I $(COMPAT_WIRELESS)/include

EXTRA_CFLAGS += -I$(BASEDIR)/int/common \
		-I$(BASEDIR)/int/eil \
		-I$(BASEDIR)/int/sbus \
		-I$(BASEDIR)/int/cil \
		-I$(UMAC_LIB)/mod_umi/export

ifeq ($(WLAN_SET_PLATFORM),u5500)
EXTRA_CFLAGS += -DRESOUTN_WLAN_ENABLE \
                -DNO_QUIESCENT_MODE
endif

ifeq ($(WLAN_SET_DUALBAND),true)
EXTRA_CFLAGS += -DENABLE_5GHZ
endif

EXTRA_CFLAGS += -DGPIO_BASED_IRQ
STERICCSON_WLAN_BUILT_IN := y
ifeq ($(STERICCSON_WLAN_BUILT_IN),y)
EXTRA_CFLAGS += -DCOMPAT_STATIC
endif

ifeq ($(BUS_TYPE),spi)
        EXTRA_CFLAGS += -DUSE_SPI
endif

ifeq ($(ARCH),armv5te)
EXTRA_CFLAGS += -DWORKAROUND
endif

EXTRA_CFLAGS += -DWAPI
#EXTRA_CFLAGS += -ENABLE_P2P_WPS

cw1200_wlan-objs := ./int/sbus/sbus.o ./int/eil/eil.o ./int/cil/cil.o  \
                    ./int/common/osif.o ./int/sbus/workaround.o ./int/sbus/$(BUS_TYPE)_wrapper.o \
		    ./lib/umac.lib

obj-m += cw1200_wlan.o

all:
	make ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELOUT) M=$(shell pwd) modules

clean:
	make ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELOUT) M=$(shell pwd) clean
	rm -f lib/umac.lib

$(BASEDIR)/.config: $(BASEDIR)/default.config
	@echo "Default .config has been created. Please modify if it is required."
	$(HIDE)$(CP) $(BASEDIR)/default.config $(BASEDIR)/.config
