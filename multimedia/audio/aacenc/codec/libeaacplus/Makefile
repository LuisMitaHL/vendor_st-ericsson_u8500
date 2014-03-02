# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../..#XXX no `/` at the end (win32 pb)
#-------------------------------
# Default Config
#-------------------------------
#CORE=mmdsp
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

LIBNAME = eaacplus_stand
LIBOBJ  =  heaac_util aac_rom aacenc aac_adj_thr aac_band_nrg \
		  etsiop_basicop2 aac_bit_cnt sbr_bit_sbr bitbuf_bitbuffer aac_bitenc \
		  aac_block_switch aac_channel_map sbr_code_env resamp_downsample_FIR \
		  aac_dyn_bits sbr_env_bit sbr_env_est aac_fft \
		  sbr_fram_gen sbr_freq_sca aac_grp_data sbr_hybrid aac_interface \
		  ffr_intrinsics sbr_invf_est aac_line_pe sbr_mh_det aac_ms_stereo \
		  sbr_nf_est etsiop_oper_32b aac_pre_echo_control sbr_ps_bitenc \
		  sbr_ps_enc aac_psy_configuration aac_psy_main aac_qc_main sbr_qmf_enc \
		  sbr_main sbr_misc sbr_rom aac_sf_estim aac_trans_headers \
		  aac_stat_bits aac_stprepro aac_tns aac_tns_param sbr_ton_corr sbr_tran_det \
		  ffr_transcendent ffr_transcendent_enc aac_transform ffr_vector 
		  	  
INSTALL_LIB=libeaacplus_stand.a 

CPPFLAGS+=-I../include -DNDEBUG -DSCHEDULED1 -DBUILD_FIXED 


ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CFLAGS+=-Otime -O3 --no_vectorize
endif
endif

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(PPROFILER_ENV),true)
CFLAGS+= --gnu-instrument
endif
endif


ifeq ($(MMPROCESSOR),x86_cortexA9)
CPPFLAGS+=-Dplatform_host -D_STM_ARM_H_ -DNON_ASM
endif
#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/AudioRules.mk

