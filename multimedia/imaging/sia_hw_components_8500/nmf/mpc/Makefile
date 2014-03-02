# -*- Mode: Makefile -*-
# vim:syntax=make:

#this following is necessary for ost trace on MMDSP side
include $(COMPONENT_TOP_DIR)/sia_ost_conf.mk
OST_COMPONENT_ID = $(OST_COMPONENT_ID_NMF_DSP)

OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_dsp
OST_SRC_DIR = grab ispctl
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

CPPFLAGS+=-I$(OST_COMPONENT_INC)/autogentraces

ifeq ($(MYMMSYSTEM),symbian)
CPPFLAGS  += -I$(MMROOT)/shared/ens/nmf/mpc/fsm/generic/include
else 
CPPFLAGS += -I$(MM_NMF_COMP_SRC_MMDSP_DIR)/fsm/generic/include
endif

#end of specific part for ost trace

CPPFLAGS+= -DNVERBOSE
#CPPFLAGS += -DTRACE_NMFPRINTF
ifndef DEBUG
#CFLAGS+=-O0
endif

ifeq ($(findstring _v1,$(MYMMBOARD)),_v1)
	MCPROJECT=$(DSPTOOLS)/target/project/nomadik/STn8500/imaging
	CPPFLAGS+= -DSOC8500V1
else
ifeq ($(findstring 500_sia,$(MYMMBOARD)),500_sia)
	MCPROJECT=$(DSPTOOLS)/target/project/nomadik/STn8500/imaging
	CPPFLAGS+= -DSOC8500V1
else
ifeq ($(findstring _v2,$(MYMMBOARD)),_v2)
	MCPROJECT=$(DSPTOOLS)/target/project/nomadik/STn8500/imaging
	CPPFLAGS+= -DSOC8500V1
else
$(error unknown platform! MYMMBOARD=$(MYMMBOARD))
endif
endif
endif
export MCPROJECT

SRCPATH+=$(MM_NMF_COMP_SRC_COMMON_DIR)
SHAREPATH+=$(COMPONENT_TOP_DIR)/share

USER2MPCITFS= \
	ispctl.api.cmd \
	ispctl.api.cfg \
	api.set_debug \
	grab.api.cmd 

MPC2USERITFS= \
	ispctl.api.alert \
	grab.api.alert 

USER2MPCITFS_CPP= \
	grabctl.api.configure \
	ispctl.api.cmd \
	ispctl.api.cfg \
	api.set_debug \
	grab.api.cmd \
	memgrabctl.api.configure	

MPC2USERITFS_CPP= \
	grabctl.api.error \
	grabctl.api.acknowledges \
	grabctl.api.reportcurves \
	ispctl.api.alert \
	grab.api.alert

ASYNCITFS =\
	grabctl.api.returnbuffer \
	grab.api.alert 

COMPONENTS= ispctl grab grabctl memgrabctl

MMINSTALL_ARGS += -n trace.idt.h -n '.*\.cpp' -n '.*\.c'

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

ELF4NMF_DIR=$(BASE_REPOSITORY)
SYM_FILE_PREFIX=sia_hw_components_8500
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

