# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk
include $(MM_MAKEFILES_DIR)/AudioFlags.mk

# keep mpc path even for ARM compilation because it may be needed by composite taht contains DSP and ARM components
SRCPATH+=$(MM_HEADER_DIR)

CPPFLAGS+=-I$(COMPONENT_TOP_DIR)/inc/autogentraces

CPPFLAGS+=-I$(MM_HEADER_DIR)/audiolibs/common/include -I$(MM_HEADER_DIR)/audiolibs

SRCPATH+=$(MM_NMF_COMP_SRC_COMMON_DIR)/audiolibs/common/include

SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/
SRCPATH+=$(COMPONENT_TOP_DIR)/..
SRCPATH+=$(COMPONENT_TOP_DIR)/../..

SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/afm
SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/afm/misc
SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/afm/common
SRCPATH+=$(MM_NMF_COMP_SRC_COMMON_DIR)/afm


ifneq ($(CORE),mmdsp)
SRCPATH+=$(MM_NMF_COMP_SRC_HOSTEE_DIR)/afm
SRCPATH+=$(MM_NMF_COMP_SRC_HOSTEE_DIR)/afm/hst
SRCPATH+=$(MM_NMF_COMP_SRC_HOSTEE_DIR)/afm/hst/common

SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/common
# SRCPATH+=$(MMROOT)/shared/ens/proxy/omxil
 SRCPATH+=$(SHARED_REPOSITORY_HOST)
 SRCPATH+=$(SHARED_REPOSITORY_HOST)/host
 SRCPATH+=$(AUDIO_REPOSITORY_HOST)
 SRCPATH+=$(AUDIO_REPOSITORY_HOST)/host

 CPPFLAGS+=-I$(MM_HEADER_DIR)/audiolibs/fake_dsptools
 CPPFLAGS+=-I$(MM_HEADER_DIR)/audiolibs/common/include

 # we use MMROOT here since mmprobe does not export its headers
 # (since out of the mm build system)
 CPPFLAGS += -I$(MMROOT)/audio/mmprobe/api

 SHAREPATH+=.
else
SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/mpcee

CPPFLAGS+=-D_NMF_MPC_
endif

ITF_PREFIX = $(lastword $(subst /,  , $(PWD)))
TOBECLEAN  += $(ITF_PREFIX)

# for Meego, do not install ens stuff
MMINSTALL_ARGS += -n /omxclock.idt.h \
                  -n /dummy.hpp \
                  -n /postevent.hpp \
                  -n /trace.idt.h \
                  -n /fsmInit.idt.h \
                  -n /fsminit.hpp \
                  -n /armnmf_emptythisbuffer.hpp \
                  -n /armnmf_buffer.idt.h \
                  -n /armnmf_fillthisbuffer.hpp \
                  -n /omxerror.idt.h \
                  -n /omxeventext.idt.h \
                  -n /omxevent.idt.h \
                  -n /eventhandler.hpp \
                  -n /omxstate.idt.h \
                  -n /omxcommand.idt.h \
                  -n /sendcommand.hpp \
                  -n /ee/api/panic.idt.h \
                  -n /ee/api/trace.hpp

# for Meego, do not install audiolibs stuff when building an audio component 
ifneq ($(notdir $(COMPONENT_TOP_DIR)), audiolibs)
MMINSTALL_ARGS += -n /memorypreset.idt.h \
                  -n /memorybank.idt.h \
                  -n /samplefreq.idt.h \
                  -n /host/memorypreset.idt.h \
                  -n /host/memorybank.idt.h \
                  -n /host/samplefreq.idt.h
endif

# for Meego, do not install afm stuff when building an audio component 
ifneq ($(notdir $(COMPONENT_TOP_DIR)), afm)
MMINSTALL_ARGS += -n /hst/encoders/encoder.hpp \
                  -n /pcmprocessings/effectconfig.idt.h \
                  -n /bool.idt.h \
                  -n /host/bool.idt.h \
                  -n /host/pcmprocessings/effectconfig.idt.h \
                  -n /host/common/channeltype.idt.h \
                  -n /host/common/bindings/shmpcmconfig.idt.h \
                  -n /host/common/bindings/shmconfig.idt.h \
                  -n /host/common/bindings/swap.idt.h \
                  -n /host/hst/misc/newcontroller/configure.hpp \
                  -n /host/hst/pcmprocessings/wrapper/configure.hpp \
                  -n /host/hst/encoders/encoder.hpp \
                  -n /host/hst/encoders/framealigned/configure.hpp \
                  -n /host/hst/common/pcmsettings.hpp \
                  -n /host/hst/common/displaymemorypreset.hpp \
                  -n /hst/common/displaymemorypreset.hpp \
                  -n /host/hst/decoders/decoder.hpp \
                  -n /host/hst/decoders/streamed/configure.hpp \
                  -n /host/hst/decoders/framealigned/configure.hpp \
                  -n /host/hst/bindings/shm/configure.hpp \
                  -n /host/hst/bindings/pcmadapter/configure.hpp \
                  -n /host/hst/bindings/shmpcm/configure.hpp \
                  -n /common/channeltype.idt.h \
                  -n /common/bindings/shmpcmconfig.idt.h \
                  -n /common/bindings/shmconfig.idt.h \
                  -n /common/bindings/swap.idt.h \
                  -n /hst/misc/newcontroller/configure.hpp \
                  -n /hst/pcmprocessings/effect.hpp \
                  -n /hst/pcmprocessings/wrapper/configure.hpp \
                  -n /hst/encoders/encoder.hpp \
                  -n /hst/encoders/framealigned/configure.hpp \
                  -n /hst/composite/dec_fa_wrp.hpp \
                  -n /hst/composite/controller.hpp \
                  -n /hst/composite/shmin.hpp \
                  -n /hst/composite/shmpcmin.hpp \
                  -n /hst/composite/pp_wrp.hpp \
                  -n /hst/composite/dec_st_wrp.hpp \
                  -n /hst/composite/shmpcmout.hpp \
                  -n /hst/composite/shmout.hpp \
                  -n /hst/composite/enc_fa_wrp.hpp \
                  -n /hst/common/pcmsettings.hpp \
                  -n /hst/decoders/decoder.hpp \
                  -n /hst/decoders/streamed/configure.hpp \
                  -n /hst/decoders/framealigned/configure.hpp \
                  -n /host/bool.idt.h \
                  -n /host/interruptdfc.hpp \
                  -n /host/pcmsettings.hpp \
                  -n /host/setmemory.hpp \
                  -n /host/requestmemory.hpp \
                  -n /host/misc/samplesplayed.idt.h \
                  -n /host/misc/samplesplayed.hpp \
                  -n /host/misc/controller/configure.hpp \
                  -n /host/pcmprocessings/wrapper/configure.hpp \
                  -n /host/pcmprocessings/mips/configure.hpp \
                  -n /host/pcmprocessings/mips/mips.idt.h \
                  -n /host/malloc/setheap.hpp \
                  -n /host/encoders/streamed/configure.hpp \
                  -n /host/common/bindings/shmpcmconfig.idt.h \
                  -n /host/common/bindings/shmconfig.idt.h \
                  -n /host/common/bindings/swap.idt.h \
                  -n /host/decoders/streamed/configure.hpp \
                  -n /host/bindings/shm/configure.hpp \
                  -n /host/bindings/pcmadapter/configure.hpp \
                  -n /host/bindings/shmpcm/configure.hpp

endif


