# -*- Mode: Makefile -*-
# vim:syntax=make:
#------------------------------
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
#DEP_DEP  : Dependencies of dependency files.
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#HEADER   : List of link to be generated in lib/include
#                       (header files visible to everyone)
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#
#----------------------------
LIBNAME = softpfloat
LIBOBJ  = spfloat_add spfloat_sub spfloat_mul spfloat_imul \
	  spfloat_cmp spfloat_conv spfloat_float spfloat_log\
	  spfloat_sqrt spfloat_atan spfloat_power spfloat_div

INSTALL_LIB = libsoftpfloat.a
CPPFLAGS+=-DNDEBUG

#WARNING todo target is MANDATORY
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
generate: bin/spfloat_mantisse.exe bin/spfloat_exponent.exe
	cd src; perl spfloat_val.pl ../../../l3enc/*/*/*.c*  ../../../aacenc/*/*/*.c* ../*/*.c

bin/spfloat_%.exe : obj/spfloat_%.o
	mkdir -p bin
	gcc -O3 -o $@ $<  $(DSPTOOLS)/bit_true/lib/$(UNAME)/lldsplib24.a -lm

obj/spfloat_%.o : spfloat_%.c
	mkdir -p obj
	gcc -g -Wall -c -I$(DSPTOOLS)/bit_true/include -Iinclude -I../common/include -DUNIX -DFIXED_POINT -DBIT_TRUE $< -o $@

.PHONY: generate
