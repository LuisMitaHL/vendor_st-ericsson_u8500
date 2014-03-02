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

LIBNAME = amrcommon
LIBOBJ  = int_lpc lsp_az lsp_lsf reorder agc weight_a syn_filt \
	  pred_lt residu gc_pred inv_sqrt sqrt_l log2 pow2 r_fft \
	  calc_cor convolve copy cor_h count gmed_n mac_32 oper_32b \
	  set_sign set_zero basicop2 efr_dtx common_amr_alloc_tables \
	  bitno_tab c2_9pf_tab gray_tab q_plsf_3_tab qgain475_tab \
	  c2_11pf_tab gains_tab lsp_tab q_plsf_5_tab qua_gain_tab \
	  window_tab bit_reorder_tab amr_memory_preset crc b_cn_cod

CPPFLAGS+=-I../include -DVAD1  -DNDEBUG -DEN_ALTERNATIVE_EFR
INSTALL_LIB = libamrcommon.$(LIB_EXT)

include ../Make.amr.flags

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

