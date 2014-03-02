# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
CORE=arm
MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk             



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
#         For multiple executable, suffix these to variable with _1, _2 ...  
#
#LOCALLIBS: list of local libraries to be linked with executable
#
#----------------------------


VPATH+= include lib

LIBNAME = speech_enhancement

INSTALL_LIB = libspl.a libspeech_enhancement.a

INSTALL_HEADERDIR  = include

#WARNING todo target is MANDATORY (exe and/or lib)
todo: export lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
test cleantest:
	@echo "no test"

export:
	@if test ! -d  $(LIB_DIR) ; then mkdir -p $(LIB_DIR); echo ***Creating directory $(LIB_DIR) ; fi 
ifeq ($(MMPROCESSOR),cortexA9)
	@cp delivery/arm/*.a $(LIB_DIR)/
else ifeq ($(MMPROCESSOR),x86)
	@cp delivery/x86/*.a $(LIB_DIR)/
else
	@echo "ERROR => usupported plarform $(MMPLATFORM)"; exit 1;
endif



.PHONY: test