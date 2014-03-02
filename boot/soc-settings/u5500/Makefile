CROSS_COMPILE=arm-eabi-
CC=arm-eabi-gcc
OBJCOPY=arm-eabi-objcopy
LDSCRIPT=soc_settings.ld
BASS_APP_SRCDIR=bass_app/src
BASS_APP_INCDIR=bass_app/include
PRCMU_HEADERS=prcmu

SRC_FILES = src/soc_settings_core.c \
	src/ddr_settings.c \
	src/soc_settings_utils.c
#Security API
SRC_FILES+= $(BASS_APP_SRCDIR)/apply_soc_settings.c $(BASS_APP_SRCDIR)/security.c
SRC_FILES+= $(BASS_APP_SRCDIR)/tee_client_api.c

TOOLCHAIN_DIRECTORY := $(shell dirname $$(dirname $$(which $(OBJCOPY))))
LIBGCC_A := $(wildcard $(TOOLCHAIN_DIRECTORY)/lib/gcc/$(CROSS_COMPILE:%-=%)/*/libgcc.a)
# include avs file if avs is enabled
SRC_FILES += src/avs/avutils_avs.c

MYCP = cp

CFLAGS=-Wall -T$(LDSCRIPT) -fomit-frame-pointer -nostdlib -march=armv6 -mtune=cortex-a9 -mabi=aapcs -fpic \
	-o soc_settings$(U5500_TYPE).elf -ffunction-sections
CFLAGS+= -I./src -I./src/$(PRCMU_HEADERS) -I$(BASS_APP_INCDIR) $(DEFINES) -DOS_FREE -DBOOTLOGBASEADDR=0x90000000

ifdef DEBUG
CFLAGS+= -g -O2 -DDEBUG
else
CFLAGS+= -O2
endif

ifeq ($(U5500_TYPE),C)
CFLAGS+= -DU5500C
SRC_FILES+= src/ddr_settings_333mhz.c
else
CFLAGS+= -DU5500B
SRC_FILES+= src/ddr_settings_266mhz.c
endif


LINKER_FLAGS=-Xlinker -M -Xlinker -Map=soc_settings$(U5500_TYPE).map

.PHONY: postbuild
postbuild: meminit.bin

meminit.bin : soc_settings$(U5500_TYPE).elf
	$(OBJCOPY) -O binary -S -v soc_settings$(U5500_TYPE).elf db5500$(U5500_TYPE).bin

soc_settings$(U5500_TYPE).elf : clean_build $(LIBGCC_A)
	$(CC) $(CFLAGS) $(SRC_FILES) $(LINKER_FLAGS) $(LIBGCC_A)
install: U5500_MEMINIT := $(PREFIX)/boot/meminits/u5500/
install:
	@echo U5500_MEMINIT : $(PREFIX)/boot/meminits/u5500/
	@$(if $(wildcard $(U5500_MEMINIT)),,mkdir -p $(U5500_MEMINIT))
	$(MYCP) -f db5500$(U5500_TYPE).bin $(U5500_MEMINIT)

clean : clean_build
	@rm -f  db5500$(U5500_TYPE).bin

clean_build :
	@rm -f  soc_settings$(U5500_TYPE).elf soc_settings$(U5500_TYPE).map

help :
	@echo "Following build options are available"
	@echo "make "
