# vim:syntax=make:
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
MODE16 = 1

LIBNAME = gsmfr
LIBOBJ  = c_rx_dtx_handler \
          c_tx_dtx_handler \
          f_basic_oper \
          f_code \
          f_decode \
          f_long_term \
          f_lpc \
          f_preprocess \
          f_rpe \
          f_short_term \
          f_tables \
          gsm_decoder \
          gsm_exp_imp \
          gsm_wav_serial \
          n_sid_decode \
          n_sid_encode \
          v_acf_average \
          v_adap_filt \
          v_pred_vals \
          v_priod_updt \
          v_spect_comp \
          v_thresh_adpt \
          v_tone_detect \
          v_vad_compute	\
		  generate_rand

CPPFLAGS+=-I../include -DTABLE_EXTERN 
INSTALL_LIB=libgsmfr.a


ifeq ($(MMPROCESSOR),cortexA9)
 ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
#Must suppress some wrong warnings
  CFLAGS+=--diag_suppress=2523 --diag_suppress=C3017

  ifeq ($(PPROFILER),true)
   CPPFLAGS+=-I$(PPROFILER_PATH)/libperf/include/ -UPPROFILER
  # CFLAGS+=--gnu-instrument
  endif
  CPPFLAGS+= -DTHUMB_OPT
  #CFLAGS+= -Otime --thumb
  CFLAGS+= -Otime 
 endif	
endif


#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
