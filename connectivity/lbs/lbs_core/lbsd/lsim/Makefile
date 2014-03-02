
include ../../config/config.mk

################################################################################
#                                   LSIM                                       #
################################################################################

LSIM_MOD := lsim

LSIM_DIR := srce

LSIM_SRC := 

ifeq ($(ENABLE_CP),true)
LSIM_SRC += lsim0fsm.c
else
ifeq ($(ENABLE_UP),true)
LSIM_SRC += lsim0fsm.c
endif
endif

ifeq ($(ENABLE_FREQ_AIDING),true)
LSIM_SRC += lsim5cc.c lsim6ccgns.c
endif

ifeq ($(ENABLE_CP),true)
LSIM_SRC += lsim2cp.c lsim4cpgns.c
endif

ifeq ($(ENABLE_UP),true)
LSIM_SRC += lsim1up.c lsim3upgns.c
endif

LSIM_INCL := -Iincl -I../gns/incl -I../libgps/$(GNSSCHIP_VER)/incl -I../agpsmain/incl -I../halgps/incl -I../cgps/incl

LSIM_OBJ := obj

LSIM_FLAGS := 

LIB := LSIM

ifeq ($(LSIM_SRC), )
.PHONY: none
none:
	$(warning, "Neither UP/CP is defined")
else
include ../../config/build.mk
endif
