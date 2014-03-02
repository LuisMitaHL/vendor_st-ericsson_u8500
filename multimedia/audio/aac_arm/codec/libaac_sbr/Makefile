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


LIBNAME     = aac_sbr


LIBOBJ  = \
        basic_op \
        cfftn \
        eaacPlus_close \
        eaacPlus_decode_frame \
        eaacPlus_open \
        env_calc \
        env_dec \
        env_extr \
        freq_sca \
        huff_dec \
        hybrid \
        lpp_tran \
        lpp_tran_CA9 \
        ps_bitdec \
        ps_dec \
        qmf_dec \
        sbr_bitb \
        sbr_close \
        sbr_crc \
        sbr_dec \
        sbrdecoder \
        sbr_decoder_main \
        sbr_fft \
        sbr_open \
        sbr_ram \
        sbr_rom \
        transcendent \
        lib_filterbank \
        lib_filterbank_CA9 \
        opt_flt_analysis_qmf \
        opt_flt_cossin_modulation \
        opt_flt_synthesis_qmf \
        opt_qmf_fwd_mod_rom \
        opt_qmf_rom \
        bs_byte_align \
        bs_copy_bit_32b \
        bs_delta_bit \
        bs_fast_move_bit_32b \
        bs_init_buffer \
        bs_init \
        bs_read_bit_32b \
        bs_write_byte_32b \
        btrue_arm \
        buffer_copy \
        buffer_reset \
        buffer_reset_CA9 \
        dolby_kbd_1024 \
        dolby_kbd_128 \
        dolby_kbd_256 \
        dolby_kbd_32 \
        fract_sqrt \
        fract_sqrt_tab \
        lib_fft \
        lib_filterbank \
        sin_1024 \
        sin_128 \
        sin_256 \
        sin_32 \
        vector_add \
        vector_add_copy \
        vector_copy \
        vector_scale_CA9 \
        vector_set_value \
        vector_stereo_interleave_pan \
        vector_sub \
        vector_window_add \
        vector_window \
        vector_window_copy \
        vector_window_sub \
		vector_scale_in_place \
		vector_copy_CA9 \
        crc_bitwise \
        crc_bytewise \
        crc_lib \
        crc_table_driven \
        dab_crc8 \
        sample_duration \
        sampling_freqs \
        bs_lookup_word_size_bit \
		bs_write_byte \
		bs_read_bit \
		bs_fast_move_bit \
        bs_write_bit 		



INSTALL_LIB=libaac_sbr.a

CPPFLAGS+=-I../include -DNDEBUG 



ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CPPFLAGS += -DMC -DSBR_MC
CFLAGS  += -O3 -Otime  --min_array_alignment=8 --signed_chars --diag_suppress 3017,550,223
endif
endif

ifeq ($(MMPROCESSOR),x86)
CPPFLAGS+=-DUSE_24BITS_LIBBITSTREAM -DGCC
endif

ifeq ($(MMPROCESSOR),x86_cortexA9)
CPPFLAGS+=-DMC -DSBR_MC 
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


