# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

OST_COMPONENT_ID = $(shell expr $(OST_MODULE_ID) + 3 )
OST_DICO_NAME = $(OST_COMPONENT_NAME)_nmf_mpc
OST_SRC_DIR = decoders misc bindings pcmprocessings

INSTALL_DICO=$(OST_DICO_NAME).xml

SRCPATH += $(COMPONENT_TOP_DIR)/nmf/mpc/common
SRCPATH += $(COMPONENT_TOP_DIR)/nmf/
SRCPATH += $(COMPONENT_TOP_DIR)/nmf/mpc/misc

ITF_PREFIX = afm

CPPFLAGS+=-DNVERBOSE 

INTERMPCITFS=\
	pcmsettings 

USER2MPCITFS_CPP= \
	pcmsettings \
	bindings.pcmadapter.configure \
	bindings.shm.configure \
	bindings.shmpcm.configure \
	pcmprocessings.wrapper.configure \
	pcmprocessings.mips.configure \
	malloc.setheap \
	misc.controller.configure \
	setmemory \
	misc.samplesplayed \
	decoders.streamed.configure \
	encoders.streamed.configure \
	pcmdump.configure

MPC2USERITFS_CPP= interruptdfc\
	requestmemory \
	pcmdump.complete

misc = \
	malloc \
	misc.controller   \
	misc.synchronisation \
	pcmdump

wrappers = \
	pcmprocessings.wrapper \
	decoders.framealigned.wrapper\
	encoders.framealigned.wrapper\
	encoders.streamed.wrapper\
	decoders.streamed.wrapper

bindings = \
	bindings.pcmadapter \
	bindings.shm.shmin \
	bindings.shm.shmout \
	bindings.shmpcm.shmin \
	bindings.shmpcm.shmout

pcmprocessings = \
	pcmprocessings.mips

COMPONENTS= $(misc) $(wrappers) $(bindings) $(pcmprocessings)

ASYNCITFS= \
	pcmsettings	\
	interruptdfc 

INSTALL_NMF_COMP_SRC_MMDSP +=   common \
				misc/*.idt \
				misc/*.itf \
				misc/malloc \
				misc/synchronisation \
				misc/afmtestmalloc \
				misc/afmdebug \
				misc/pcmdump \
				decoders \
				encoders \
				pcmprocessings/effect.itf \
				pcmprocessings/wrapper/wrapper.conf \
				pcmprocessings/wrapper/src/wrapper.c \
				pcmprocessings/wrapper/configure.itf \
				pcmprocessings/tester/tester.conf \
				pcmprocessings/tester/tester/tester.conf \
				pcmprocessings/tester/tester/testerconfig.itf \
				pcmprocessings/tester/tester/src/tester.c \
				pcmprocessings/tester/main/src/main.c \
				pcmprocessings/tester/main/main.conf

# TODO: should be removed inthe future since audiocodec does not use it anymore
INSTALL_NMF_COMP_SRC_MMDSP +=  misc/xbusdma/include/xbusdma.h \
			       misc/xbusdma/afmxdmaperiph.idt

INSTALL_NMF_COMP_SRC_MMDSP_PREFIX = afm/

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk


TESTLISTS = testlist.lst
DATABASE  = mpc
TEST_TAG  = $(MMDSPPROCESSOR)
include $(MM_MAKEFILES_DIR)/AudioTest.mk

