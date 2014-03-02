# vim:syntax=make:
# -*- Mode: Makefile -*-
# vim:syntax=make:

#WARNING COMPONENT_PATH is MANDATORY for Makeonfig and Make.rules
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
STACK  = 5120

LIBNAME = amrwbenc
LIBOBJ  = e_homing 	\
          cod_main 	\
          wb_vad	\
          lag_wind 	\
          levinson	\
          q_gain2	\
          az_isp	\
          gpclip   	\
          residu	\
          deemph	\
          lp_dec2	\
          pitch_f4	\
          updt_tar	\
          c2t64fx	\
          c4t64fx	\
          g_pitch	\
          q_pulse	\
          p_med_ol	\
          hp_wsp	\
          dtx_enc 	\
          q_isf_ns	\
          decim54	\
          autocorr	\
          convolve	\
          cor_h_x	\
          parm_serial	\
          cod_amrwb_alloc_ymem

CPPFLAGS+= -I../include -DNDEBUG
INSTALL_LIB = libamrwbenc.a

include ../Make.amrwb.flags

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

