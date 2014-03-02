# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
NO_MMDSP=1

include $(MMROOT)/audio/makefiles/Make.algo.config

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


LIBNAME = mp3decode

LIBOBJ  = close \
    description \
    description_2_5 \
    dynpart \
    error_check \
    getbits \
    huffdecfunctions \
    huffman_init \
    init \
    l3 \
    l3_dsp \
    l3crc \
    l3mod \
    l3stereo \
    dequan_table \
    ram_align_y \
    rom_mp3 \
    sync \
    sync_2_5 \
    mp3_dequan_CA9 \
    mp3_dequan_tables_CA9 \
    mp3dequan_init_table \
    polysyn_init_tables \
    polysyn_CA9 \
    crc \
    bs_copy_bit_32b \
    bs_delta_bit \
    bs_fast_move_bit_32b \
    bs_init \
    bs_init_buffer \
    bs_read_bit_32b \
    btrue_arm \
    vector_copy_CA9 \
    vector_ms_CA9 \
    buffer_reset_CA9 \
    vector_set_value \
    imdct_CA9 \
    mp3hybrid_CA9 \
    mp3_hybrid_common_CA9 \
	dewindow \
    alias_table\
    buffer_copy \
    bs_write_byte_32b\
	buffer_reset \
	vector_copy \
	vector_scale \
	vector_ms \
	polysyn \
	mp3hybrid \
	bs_copy_bit \
	bs_write_byte \
	bs_fast_move_bit \
	polysyn_bt \
	vector_set_value \
	dewindow \
	imdct_bt \
	imdct \
	mp3_hybrid_imdct_cos \
	mp3_hybrid_tables \
	mp3_hybrid_common \
	modulation_table \
	mp3hybrid \
	dequan \
	dequan_bt \
	mp3_dequan_bt \
	mp3_hybrid_init_tables


INSTALL_LIB=libmp3decode.a

CPPFLAGS+=-I../include_codec -I../include -DNDEBUG -DUSE_ANCILLARY 

LDFLAGS+= -D__FAIRBANKS

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CPPFLAGS+= -DBUILD_FIXED -DBIT32
CFLAGS  +=-Otime --min_array_alignment=8 
endif
endif

ifeq ($(MMPROCESSOR),x86)    
  CPPFLAGS+=-DUSE_24BITS_LIBBITSTREAM -DGCC 
endif

ifeq ($(MMPROCESSOR),x86_cortexA9)    
  CPPFLAGS+=-DBUILD_FIXED -DBIT32
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
include $(MMROOT)/shared/makefiles/Make.rules

