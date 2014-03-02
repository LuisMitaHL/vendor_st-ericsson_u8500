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
include $(MM_MAKEFILES_DIR)/AudioAlgoConfig.mk
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
#----------------------------
LIBNAME = bitstream
LIBOBJ  = bs_copy_to_array bs_copy_from_array bs_write_array bs_init\
	  bs_init_buffer bs_bit_pointer_position bs_word_pointer_position\
	  bs_get_buf_add bs_read_1bit bs_read_bit bs_read_bit_long\
	  bs_read_byte bs_read_word bs_reset bs_reset_buffer bs_write_bit\
	  bs_write_byte bs_write_word bs_lookup_word_size_bit\
	  bs_read_sample_lsb bs_read_sample_msb bs_fast_move_bit\
	  bs_fast_move_byte bs_fast_move_word bs_copy_word_modulo_dest\
	  bs_copy_word_modulo_source bs_copy_bit bs_copy_byte bs_copy_word\
	  bs_delta_bit bs_byte_align bs_byte_align_struct\
	  bs_word_align bs_check_struct bs_copy_samples bs_dcumode16 \
	  bs_read_bit_32b bs_fast_move_bit_32b bs_write_byte_32b

LIBOBJ +=bs_copy_bit_32b 

INSTALL_LIB = libbitstream.a

ifneq ($(TARGET),unix)
CPPFLAGS+=-DNDEBUG
endif
ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
CFLAGS+= -Otime --signed_chars
endif
endif

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

