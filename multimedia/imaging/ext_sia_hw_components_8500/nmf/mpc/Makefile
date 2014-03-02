# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MMROOT)/shared/makefiles/Make.nmf.config

CPPFLAGS= -DNVERBOSE
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
ifeq ($(PLATFORM),peps8500_sia-think)
CPPFLAGS += -D__PEPS8500_SIA 
endif
export MCPROJECT

SRCPATH+=$(MMROOT)/shared/ens/nmf/mpc
SRCPATH+=$(MMROOT)/shared/ens/nmf/common
SRCPATH+=$(MMROOT)/shared/ens/nmf/mpc/common
SRCPATH+=$(MMROOT)/shared/nmf/osintegration/hostsrc

SHAREPATH=../../share
SHAREPATH+=$(MMROOT)/shared/common/inc
SHAREPATH+=$(MMROOT)/shared/ens/nmf/mpc/common

USER2MPCITFS= \
	ext_ispctl.api.cmd \
	ext_ispctl.api.cfg \
	ext.api.set_debug \
	ext_grab.api.cmd 

MPC2USERITFS= \
	ext_ispctl.api.alert \
	ext_grab.api.alert 

USER2MPCITFS_CPP= \
	ext_grabctl.api.configure \
	ext_ispctl.api.cmd \
	ext_ispctl.api.cfg \
	ext.api.set_debug \
	ext_grab.api.cmd \

MPC2USERITFS_CPP= \
	ext_grabctl.api.error \
	ext_grabctl.api.acknowledges \
	ext_grabctl.api.reportcurves \
	ext_ispctl.api.alert \
	ext_grab.api.alert

ASYNCITFS =\
	ext_grabctl.api.returnbuffer \
	ext_grab.api.alert 

COMPONENTS= ext_ispctl ext_grab ext_grabctl

MMINSTALL_ARGS += -n trace.idt.h -n '.*\.cpp' -n '.*\.c'

include $(MMROOT)/shared/makefiles/Make.nmf.rules

ELF4NMF_DIR=nmf_repository/mmdsp_8500
SYM_FILE_PREFIX=ext_sia_hw_components_8500
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

