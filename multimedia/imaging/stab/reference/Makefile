MMBOARD=x
MMSYSTEM=x

# This will make use of RVCT under Linux and allow to deliver pre-build library
USE_OPT_COMPILER=true

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#REV := $(shell svn info . | grep "Last Changed Revision" | gawk '{print $$2 ; }')
#CPPFLAGS += -DREVISION=$(REV)

#CPPFLAGS += DSP_BUILD
#CPPFLAGS += DEBUG
#CPPFLAGS += ENABLE_TRACE

ifdef DEBUG
LIBNAME = st_stab_debug
#INSTALL_LIB = libst_stab_debug.a
OBJ_EXT = odebug
CFLAGS = -g -O0
CPPFLAGS += -DDEBUG
else
LIBNAME = st_stab
#INSTALL_LIB = libst_stab.a
endif

LIBOBJ = stab_lib

INSTALL_LIB = lib$(LIBNAME).a

ifeq ($(findstring x86, $(MYMMBOARD)),x86)
EXEOBJ += stab_main\
	stab_hw \
	stab_dsp_fw \
	stab_io \
	stab_dsp_dd \
	stab_lib
EXENAME = stab
CPPFLAGS += -DSTANDALONE
endif

INSTALL_HEADER = stab_common.h stab_lib.h

todo: lib exe 

include $(MM_MAKEFILES_DIR)/SharedRules.mk

