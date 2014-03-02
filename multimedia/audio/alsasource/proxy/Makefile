MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../$(BASE_REPOSITORY)/host
CPPFLAGS+= -I../$(BASE_REPOSITORY)

CPPFLAGS+= -I$(MMROOT)/audio/afm/proxy/include/

LIBOBJ = alsasource_debug AlsaSource AlsaSourceNmfHost

INSTALL_HEADER = AlsaSource.hpp

LIBNAME = st_AlsaSource
INSTALL_LIB=libst_AlsaSource.a
CPPFLAGS += -DRELEASE -D__RELEASE
CPPFLAGS += -DALSASOURCE_LOG_ERRORS
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  CPPFLAGS += -DMMPROBE_ENABLED
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
