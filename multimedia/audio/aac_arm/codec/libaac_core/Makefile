# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
NO_MMDSP=1

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
#AUDIOLIBS: list of audiolibs libraries to be linked with executable 
#LOCALLIBS: list of local libraries to be linked with executable
#PCMLIBS  : list of pcmfile libraries to be linked with executable
#           Do not specify the lib prefix nor the suffix (ex: bitstream to link
#           libbitstream.[a/lib])
#
#----------------------------


LIBNAME = aac_core

LIBOBJ  = \
         aac_huffman_lut \
         aac_memory \
         bitfield \
         close \
         config \
         crc \
         debug \
         decdata \
         decode_book \
         decoder \
         dequan \
         drc \
         extension \
         frac_inv_sqrt_div2_tab \
         huffdec1 \
         huffdec2 \
         huffdec3 \
         huffdec3_ram \
         huffinit \
         huffman_table \
         init \
         monopred \
         nok_lt_prediction \
         pulse \
         ram_heap \
         scale \
         tns \
         tns_table \
         filter \
         filter_compress_state \
         conceal \
         conceal_powtab \
         conceal_rom \
         conceal_ktab



INSTALL_LIB=libaac_core.a

CPPFLAGS+=-I../include  -DNDEBUG 

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CFLAGS += -O3 --asm --signed_chars
CPPFLAGS += -DBUILD_FIXED -DMC -DSBR_MC
CFLAGS  +=-Otime --min_array_alignment=8 --diag_suppress 550
endif
endif

ifeq ($(MMPROCESSOR),x86)
CPPFLAGS+=-DUSE_24BITS_LIBBITSTREAM -DGCC
endif

ifeq ($(MMPROCESSOR),x86_cortexA9)
CPPFLAGS+=-DMC -DSBR_MC -DBUILD_FIXED
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 


ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(PPROFILER_CA9),true)
CFLAGS+=--gnu-instrument
endif
endif
#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
