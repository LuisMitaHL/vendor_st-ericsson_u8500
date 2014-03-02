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

LIBNAME = amrenc
LIBOBJ  = sid_sync          \
          e_homing          \
          sp_enc            \
          pre_proc          \
          cod_amr           \
          vad1              \
          vad2              \
          vadname           \
          dtx_enc           \
          lpc               \
          autocorr          \
          lag_wind          \
          levinson          \
          lsp               \
          az_lsp            \
          q_plsf            \
          q_plsf_3          \
          q_plsf_5          \
          lsfwt             \
          cl_ltp            \
          pitch_fr          \
          inter_36          \
          enc_lag3          \
          enc_lag6          \
          g_pitch           \
          q_gain_p          \
          pre_big           \
          ol_ltp            \
          pitch_ol          \
          p_ol_wgh          \
          hp_max            \
          lflg_upd          \
          spreproc          \
          cbsearch          \
          c2_9pf            \
          c2_11pf           \
          c3_14pf           \
          c4_17pf           \
          c8_31pf           \
          c1035pf           \
          s10_8pf           \
          gain_q            \
          calc_en           \
          qgain475          \
          qgain795          \
          g_code            \
          q_gain_c          \
          qua_gain          \
          g_adapt           \
          ton_stab          \
          spstproc          \
          prm2bits          \
          cod_amr_alloc_ymem\
          efr_dtx_tx

CPPFLAGS+=-I../include -DVAD1 -DNDEBUG -DEN_ALTERNATIVE_EFR
#both EN_ALTERNATIVE_EFR and EN_COMPATIBILITY_EFR_REFCODER are necessary for compatibility with ref EFR CODER v5.1.0 6/26/96 
#CPPFLAGS+=-DEN_COMPATIBILITY_EFR_REFCODER
INSTALL_LIB = libamrenc.a	

include ../Make.amr.flags

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

