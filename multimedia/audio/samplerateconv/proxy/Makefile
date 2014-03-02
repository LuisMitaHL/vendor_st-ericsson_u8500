MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = NmfHostSampleRateConv_PcmProcessing NmfHostSampleRateConv
INSTALL_HEADER  +=  NmfHostSampleRateConv.h

ifeq ($(CORE_NUMBER),2)
#For MMDSP
LIBOBJ += NmfMpcSampleRateConv 
LIBOBJ += samplerateconv_cpp-user2mpc-stubs 
INSTALL_HEADER  +=  NmfMpcSampleRateConv.h
LIBNAME = st_SampleRateConv
INSTALL_LIB=libst_SampleRateConv.a
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_SampleRateConv_debug
INSTALL_LIB=libst_SampleRateConv_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_SampleRateConv
INSTALL_LIB=libst_SampleRateConv.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
