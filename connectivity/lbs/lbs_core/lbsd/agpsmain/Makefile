
include ../../config/config.mk

################################################################################
#                                   AGPSMAIN                                   #
################################################################################

AGPSMAIN_MOD := agpsmain

AGPSMAIN_DIR := srce
ifeq ($(LBS_UBUNTU),true)
AGPSMAIN_SRC := agpsmain.c agpsserver.c ../../libgps/$(GNSSCHIP_VER)/srce/GN_EE_api_stubs.c
else
ifeq ($(UBUNTU_X86),true)
AGPSMAIN_SRC := agpsmain.c agpsserver.c ../../libgps/$(GNSSCHIP_VER)/srce/GN_EE_api_stubs.c ../../libgps/$(GNSSCHIP_VER)/srce/GN_Test_Stubs.c
else
AGPSMAIN_SRC := agpsmain.c agpsserver.c
endif
endif
ifeq ($(LBS_UBUNTU),true)
AGPSMAIN_INCL := -Iincl -I../libgps/$(GNSSCHIP_VER)/incl -I../cgps/incl -I../agpscodec/incl -I../amil/incl -I../gns/incl \
		-I../gnsi/incl -I../lbscfg/incl -I../incl
else
ifeq ($(UBUNTU_X86),true)
AGPSMAIN_INCL := -Iincl -I../libgps/$(GNSSCHIP_VER)/incl -I../cgps/incl -I../agpscodec/incl -I../amil/incl -I../gns/incl \
		-I../gnsi/incl -I../lbscfg/incl -I../incl
else
AGPSMAIN_INCL := -Iincl -I../libgps/$(GNSSCHIP_VER)/incl -I../cgps/incl -I../agpscodec/incl -I../amil/incl -I../gns/incl \
		-I../gnsi/incl
endif
endif
AGPSMAIN_OBJ := obj

AGPSMAIN_FLAGS := 

MOD := AGPSMAIN

PROG := lbsd

PROGOUT := ..
ifeq ($(LBS_UBUNTU),true)
LDFLAGS += -lagpsosa -lagpscodec -lcgps -lgns -lhalgps -llbscfg
else
ifeq ($(UBUNTU_X86),true)
LDFLAGS += -lagpsosa -lagpscodec -lcgps -lgns -lhalgps -llbscfg
ifeq ($(ENABLE_UP),true)
LDFLAGS += -llsim -lsupl
endif
ifeq ($(ENABLE_CP),true)
LDFLAGS += -llsim
endif
else
LDFLAGS += -lagpsosa -llsim -lagpscodec -lcgps -lgns -lhalgps
endif
endif

ifeq ($(LBS_UBUNTU),true)
LBS_LIB_PATH += -L../libagpsosa/lib/$(ARCH) -L../lsim/lib/$(ARCH) -L../halgps/lib/$(ARCH) -L../cgps/lib/$(ARCH) \
	-L../libgps/$(GNSSCHIP_VER)/lib/$(ARCH)	-L../amil/lib/$(ARCH) -L../lbscfg/lib/$(ARCH) -L../agpscodec/lib/$(ARCH) -L../gns/lib/$(ARCH) -L../supl/lib/$(ARCH)
else
ifeq ($(UBUNTU_X86),true)
LBS_LIB_PATH += -L../libagpsosa/lib/$(ARCH)/cg2900 -L../lsim/lib/$(ARCH) -L../halgps/lib/$(ARCH) -L../cgps/lib/$(ARCH) \
	-L../libgps/$(GNSSCHIP_VER)/lib/$(ARCH)	-L../amil/lib/$(ARCH) -L../lbscfg/lib/$(ARCH) -L../agpscodec/lib/$(ARCH) -L../gns/lib/$(ARCH) -L../supl/lib/$(ARCH)
else

LBS_LIB_PATH += -L../libagpsosa/lib/$(ARCH) -L../lsim/lib/$(ARCH) -L../halgps/lib/$(ARCH) -L../cgps/lib/$(ARCH) \
	-L../libgps/$(GNSSCHIP_VER)/lib/$(ARCH)	-L../amil/lib/$(ARCH) -L../agpscodec/lib/$(ARCH) -L../gns/lib/$(ARCH) -L../supl/lib/$(ARCH)
endif
endif
ifeq ($(ENABLE_UP),true)
LDFLAGS += -lsupl
endif
ifneq ($(UBUNTU_X86),true)
ifneq ($(LBS_UBUNTU),true)

ifeq ($(GENERIC_SOLUTION),false)
LDFLAGS += -llcsclient
endif
endif
endif

LDFLAGS += ../libgps/$(GNSSCHIP_VER)/lib/$(ARCH)/libgps.a

include ../../config/build.mk
