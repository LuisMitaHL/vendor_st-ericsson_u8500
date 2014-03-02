
include ../../config/config.mk

################################################################################
#                                   CGPS                                       #
################################################################################

CGPS_MOD := cgps

CGPS_DIR := srce
ifeq ($(LBS_UBUNTU),true)
CGPS_SRC := cgps0main.c cgps1lib.c cgps2patch.c cgps3demo.c cgps4utils.c cgps5supl.c cgps6tat.c \
	cgps7cp.c cgps8gsa.c cgps9testMode.c cgps10HistoricalCell.c
else
ifeq ($(UBUNTU_X86),true)
CGPS_SRC := cgps0main.c cgps1lib.c cgps2patch.c cgps3demo.c cgps4utils.c cgps5supl.c cgps6tat.c \
	cgps7cp.c cgps8gsa.c cgps9testMode.c cgps10HistoricalCell.c
else

CGPS_SRC := cgps0main.c cgps1lib.c cgps2patch.c cgps3demo.c cgps4utils.c cgps5supl.c cgps6tat.c \
	cgps7cp.c cgps8gsa.c cgps9testMode.c cgps11ee.c
endif
endif

ifeq ($(LBS_UBUNTU),true)
CGPS_INCL := -Iincl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../agpsmain/incl -I../halgps/incl -I../lsim/incl -I../lbscfg/incl
else
ifeq ($(UBUNTU_X86),true)
CGPS_INCL := -Iincl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../agpsmain/incl -I../halgps/incl -I../lsim/incl -I../lbscfg/incl
else
CGPS_INCL := -Iincl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../agpsmain/incl -I../halgps/incl -I../lsim/incl
endif
endif

CGPS_OBJ := obj

CGPS_FLAGS := 

LIB := CGPS

include ../../config/build.mk
