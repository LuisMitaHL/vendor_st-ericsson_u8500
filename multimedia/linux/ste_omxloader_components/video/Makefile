
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

## Dirs to compile for u8500, u5500, x86 cores
DIRECTORIES = common

ifneq ($(CORE_NUMBER),1)
## u8500 only
DIRECTORIES += mpeg4dec mpeg4enc jpegenc jpegdec val_source_sink h264dec h264enc vc1dec mpeg2dec exif_mixer
else
## u5500 only
DIRECTORIES += mpeg4dec mpeg4enc jpegenc jpegdec val_source_sink h264dec h264enc exif_mixer
endif

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk

