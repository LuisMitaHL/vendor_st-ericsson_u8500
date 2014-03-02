# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

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
INSTALL_HEADER=audio_codecs_chipset_api.h audio_render_chipset_api.h \
	       audio_sourcesink_chipset_api.h \
	       audio_transducer_chipset_api.h haptics_chipset_api.h \
		   audio_av_sync_chipset_api.h \
	       audio_hw_control_chipset_api.h audio_chipset_api_index.h audio_hwctrl_common.h channel_types.h \
		   audio_codec_type.h hdmi_ctrl_sink.h hdmi_data_sink.h a2dp_ctrl_data.h 

#WARNING todo target is MANDATORY (exe and/or lib)
## Calling install to have the header installed in src case
todo: install

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------

test cleantest:
	@echo No Test...
