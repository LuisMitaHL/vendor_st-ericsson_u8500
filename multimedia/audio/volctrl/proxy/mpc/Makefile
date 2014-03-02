MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

ifeq ($(CORE_NUMBER),2)
#For MMDSP
LIBOBJ = NmfMpcVolCtrl 
LIBOBJ += volctrl_cpp-user2mpc-stubs 
LIBNAME = st_VolCtrl
INSTALL_HEADER  =  NmfMpcVolCtrl.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_VolCtrl_debug
INSTALL_LIB=libst_VolCtrl_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_VolCtrl
INSTALL_LIB=libst_VolCtrl.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
