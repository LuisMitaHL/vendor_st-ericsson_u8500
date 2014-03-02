
include ../config/config.mk

PROG := lbsd
ifeq ($(LBS_UBUNTU),true)
LD_FLAGS := -lagpsosa -llbscfg -lhalgps -lcgps -lgps -lagpscodec -lgns -lagpsmain -lm
else
ifeq ($(UBUNTU_X86),true)
LD_FLAGS := -lagpsosa -llbscfg -lhalgps -lcgps -lgps -lagpscodec -lgns -lagpsmain -lm -g3
else

LD_FLAGS := -lagpsosa -lsupl -llsim -lhalgps -lcgps -lgps -llcsclient -lagpscodec -lgns -lagpsmain -lm
endif
endif

include $(CURDIR)/../config/build.mk