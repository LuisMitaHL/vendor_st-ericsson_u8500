
include ../../config/config.mk

################################################################################
#                                    HALGPS                                    #
################################################################################

HALGPS_MOD := halgps

HALGPS_DIR := srce
ifeq ($(LBS_UBUNTU),true)
HALGPS_SRC := halgps0main.c halgps1lib.c halgps2uart.c halgps3time.c halgps4log.c halgps5gsmstring.c \
	halgps6spi.c halgps6spiLinux.c halgps8xosgps.c halgps9hci.c halgps20gsa.c halgps21l1rx.c halgps10hciLinux.c
else
ifeq ($(UBUNTU_X86),true)
HALGPS_SRC := halgps0main.c halgps1lib.c halgps2uart.c halgps3time.c halgps4log.c halgps5gsmstring.c \
	halgps6spi.c halgps6spiLinux.c halgps8xosgps.c halgps9hci.c halgps20gsa.c halgps21l1rx.c halgps10hciLinux.c
else

HALGPS_SRC := halgps0main.c halgps1lib.c halgps2uart.c halgps3time.c halgps4log.c halgps5gsmstring.c \
	halgps6spi.c halgps6spiLinux.c halgps8xosgps.c halgps9hci.c halgps20gsa.c halgps21l1rx.c 
endif
endif

ifeq ($(LBS_UBUNTU),true)
HALGPS_INCL := -Iincl -I../cgps/incl -I../lsim/incl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../lbscfg/incl
else
ifeq ($(UBUNTU_X86),true)
HALGPS_INCL := -Iincl -I../cgps/incl -I../lsim/incl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../lbscfg/incl
else


HALGPS_INCL := -Iincl -I../cgps/incl -I../lsim/incl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl
endif
endif

HALGPS_OBJ := obj

HALGPS_FLAGS := 

LIB := HALGPS

include ../../config/build.mk