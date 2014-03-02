MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = ToneGenerator ToneGeneratorNmfHost
INSTALL_HEADER = ToneGenerator.hpp

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_ToneGenerator_debug
INSTALL_LIB=libst_ToneGenerator_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug
else
LIBNAME = st_ToneGenerator
INSTALL_LIB=libst_ToneGenerator.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
