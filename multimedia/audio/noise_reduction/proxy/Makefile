MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = NoiseReductionPcmProcessing NoiseReductionNmfHost_PcmProcessing
INSTALL_HEADER = NoiseReductionPcmProcessing.h omx_ste_noise_reduction.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_NoiseReduction_debug
INSTALL_LIB=libst_NoiseReduction_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug
else
#CFLAGS=-g -O0
LIBNAME = st_NoiseReduction
INSTALL_LIB=libst_NoiseReduction.a
#CPPFLAGS += -DDEBUG -D__DEBUG
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
