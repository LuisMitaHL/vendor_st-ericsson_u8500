# -*- Mode: Makefile -*-
# vim:syntax=make:
#------------------------------
# Platform config ( UNIX/WIN )
#------------------------------
#WARNING COMPONENT_PATH is MANDATORY for Make.algo.config and Make.rules
COMPONENT_PATH = ../#XXX no `/` at the end (win32 pb)

MMBOARD=x
MMSYSTEM=x
USE_OPT_COMPILER=true
include ../AudioLibEffectsConfig.mk
#--------------------------
# Local variable
#--------------------------

#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#LDPATH   : Search path for linked libs.(see default value in Make.rules)
#LDLIBS   : List of library name without lib prefix or any suffix
#SHLDLIBS : List of library name without lib prefix or any suffix.
#           Overides LDLIBS for shared libs if specified
#
#NOSTARTFILES : to be defined to generate .so with -nostartfiles
#
#DEPOBJ   : List of object files. (to generate dependencies).
#DEP_DEP  : Dependencies of dependency files.
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#HPATH    : Search Path for header file to be linked. WARNING only one path is possible (see default value in Make.rules)
#HEADER   : List of link to be generated in lib/include
#                       (header files visible to everyone)
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#
#RECOMPILED_PROJECT: list of project that must be recompiled (not for 8810 8815)
#----------------------------
RECOMPILED_PROJECT= tanami

LIBNAME = vector
LIBOBJ  = vector_set_value vector_window_copy vector_window \
	  vector_window_add vector_window_sub vector_copy_CA9 vector_copy \
	  vector_copy_2y vector_copy_neg vector_swap vector_add\
	  vector_sub vector_add_copy vector_ms vector_ms_CA9 vector_ms_scale \
	  vector_scale vector_scale_CA9 vector_scale_in_place \
	  vector_scale_copy_to_buffers vector_scale_add \
	  vector_scale_mix vector_abs vector_max_magnitude \
	  vector_stereo_interleave_pan buffer_copy\
	  buffer_calloc buffer_reset buffer_reset_CA9

INSTALL_LIB = libvector.a

CPPFLAGS+=-DNDEBUG

ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CFLAGS+= -O3 -Otime --min_array_alignment=8 --vectorize
endif

ifeq ($(MMCOMPILER),gnu)
CFLAGS+= -mfloat-abi=softfp -mfpu=neon -mfp=vfp
endif
endif
DEPOBJ=$(LIBOBJ)



ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(PPROFILER_CA9),true)
CFLAGS+=--gnu-instrument
endif
endif



#WARNING todo target is MANDATORY
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------

