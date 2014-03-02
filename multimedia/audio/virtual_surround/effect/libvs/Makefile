# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/AudioAlgoConfig.mk

#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#
#DEP_DEP  : Dependencies of dependency files.
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#
#MODE16   : Set this variable to compile in 16bit mode
#STACK    : Size of the user stack
#SSTACK   : Size of the supervisor stack
#
#LIBS: list of audiolibs libraries to be linked with executable
#LIBS: list of local libraries to be linked with executable
#PCMLIBS  : list of pcmfile libraries to be linked with executable
#           Do not specify the lib prefix nor the suffix (ex: bitstream to link
#           libbitstream.[a/lib])
#
#----------------------------

LIBNAME = vs
LIBOBJ  = spl_biquad_vint16 \
          spl_basic_ops \
          spl_fir \
          virtual_surround_create \
          virtual_surround_frame \
          virtual_surround_virtualization \
          virtual_surround_room_simulation \
          virtual_surround_delayline \
          virtual_surround_mixer \
          virtual_surround_allocate \
          virtual_surround_destroy \
          virtual_surround_reset \
          virtual_surround_getparam \
          virtual_surround_setparam \
          virtual_surround_execute

INSTALL_LIB = libvs.a
CPPFLAGS+=-I../include -I../include/spl

ifdef VIRTUAL_SURROUND_DEBUG
CFLAGS  += -g -O0
CPPFLAGS+= -DVIRTUAL_SURROUND_DEBUG
endif

ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
CPPFLAGS+=-I$(MMROOT)/audio/tools/ARM/cmodels/cortexA9/stm-arm-neon-c-models/include -DSTM_ARM_NEON_MODELS
endif

ifeq ($(MMPROCESSOR),cortexA9)
CPPFLAGS+= -DARM -DNEON
endif

cmodels:
ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
	$(GEN_LN) $(MMROOT)/audio/tools/ARM/cmodels/cortexA9/stm-arm-neon-c-models/lib/libstm-arm-neon.a $(MMROOT)/audio/lib/$(MMPROCESSOR)/libstm-arm-neon.a
	@echo "Use ST-Ericsson cmodels for x86 platform"
else
	@echo "Use hardware intrincsics"
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib cmodels

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk