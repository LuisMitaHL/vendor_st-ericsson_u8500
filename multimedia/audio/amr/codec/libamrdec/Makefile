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

LIBNAME = amrdec
LIBOBJ  = sp_dec            \
          d_homing          \
          bits2prm          \
          dec_amr           \
          dtx_dec           \
          d_plsf_3          \
          d_plsf_5          \
          a_refl            \
          lsp_avg           \
          dec_lag3          \
          dec_lag6          \
          d2_9pf            \
          d2_11pf           \
          d3_14pf           \
          d4_17pf           \
          d8_31pf           \
          dec_gain          \
          ec_gains          \
          int_lsf           \
          c_g_aver          \
          ph_disp           \
          ex_ctrl           \
          bgnscd            \
          d_gain_c          \
          d_gain_p          \
          d1035pf           \
          d_plsf            \
          pstfilt           \
          preemph           \
          post_pro          \
          dec_amr_alloc_ymem\
          efr_dtx_rx

CPPFLAGS+=-I../include -DVAD1 -DNDEBUG -DEN_ALTERNATIVE_EFR
INSTALL_LIB = libamrdec.$(LIB_EXT)

include ../Make.amr.flags

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

