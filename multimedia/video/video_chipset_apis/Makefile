# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.config

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
INSTALL_HEADER= video_generic_chipset_api.h video_chipset_api_index.h OMX_Symbian_VideoExt_Ste.h

#WARNING todo target is MANDATORY (exe and/or lib)
## Calling install to have the header installed in src case
todo: install

#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules

#-------------------------------
# local Rules
#-------------------------------

test cleantest:
	@echo No Test...
