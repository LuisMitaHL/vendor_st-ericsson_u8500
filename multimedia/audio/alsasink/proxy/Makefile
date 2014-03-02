MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../$(BASE_REPOSITORY)/host
CPPFLAGS+= -I../$(BASE_REPOSITORY)

CPPFLAGS+= -I$(MMROOT)/audio/afm/proxy/include/

LIBOBJ = alsasink_debug AlsaSink AlsaSinkNmfHost

INSTALL_HEADER = AlsaSink.hpp

LIBNAME = st_AlsaSink
INSTALL_LIB=libst_AlsaSink.a
CPPFLAGS += -DRELEASE -D__RELEASE
CPPFLAGS += -DALSASINK_LOG_ERRORS
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  CPPFLAGS += -DMMPROBE_ENABLED
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
