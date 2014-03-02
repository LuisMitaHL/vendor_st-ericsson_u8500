MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = NmfHostVolCtrl_PcmProcessing NmfHostVolCtrl Volume_PcmPort
INSTALL_HEADER  =  NmfHostVolCtrl.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_VolCtrl_host_debug
INSTALL_LIB=libst_VolCtrl_host_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_VolCtrl_host
INSTALL_LIB=libst_VolCtrl_host.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
