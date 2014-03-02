#compilation dependency for android

#--------------------------------------
# Library: libste_ens_audio_samplerateconv
#
$(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_ens_audio_samplerateconv.so: build-multimedia


$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_ens_audio_samplerateconv_intermediates/export_includes: $(TARGET_OUT_INTERMEDIATES)/lib/libste_ens_audio_samplerateconv.so
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_ens_audio_samplerateconv_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_ens_audio_samplerateconv_intermediates/export_includes

$(TARGET_OUT_INTERMEDIATES)/lib/libste_ens_audio_samplerateconv.so: $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_ens_audio_samplerateconv.so
	@echo target STE MM: $@
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_ens_audio_samplerateconv_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_ens_audio_samplerateconv_intermediates/export_includes
	$(hide) cp -f $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_ens_audio_samplerateconv.so $(TARGET_OUT_INTERMEDIATES)/lib/

#--------------------------------------
# Library: libste_audio_hwctrl
#
#$(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_audio_hwctrl.so: build-multimedia

#$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_hwctrl_intermediates/export_includes: $(TARGET_OUT_INTERMEDIATES)/lib/libste_audio_hwctrl.so
#	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_hwctrl_intermediates/
#	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_hwctrl_intermediates/export_includes

#$(TARGET_OUT_INTERMEDIATES)/lib/libste_audio_hwctrl.so: $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_audio_hwctrl.so
#	@echo target STE MM: $@
#	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_hwctrl_intermediates/
#	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_hwctrl_intermediates/export_includes
#	$(hide) cp -f $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libste_audio_hwctrl.so $(TARGET_OUT_INTERMEDIATES)/lib/

#--------------------------------------
# Library: libomxil-bellagio
#
$(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libomxil-bellagio.so: build-multimedia

$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libomxil-bellagio_intermediates/export_includes: $(TARGET_OUT_INTERMEDIATES)/lib/libomxil-bellagio.so
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libomxil-bellagio_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libomxil-bellagio_intermediates/export_includes

$(TARGET_OUT_INTERMEDIATES)/lib/libomxil-bellagio.so: $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libomxil-bellagio.so
	@echo target STE MM: $@
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libomxil-bellagio_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libomxil-bellagio_intermediates/export_includes
	$(hide) cp -f $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libomxil-bellagio.so $(TARGET_OUT_INTERMEDIATES)/lib/

#--------------------------------------
# Library: ste_omxcomponents/libste_audio_mixer
#
$(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/ste_omxcomponents/libste_audio_mixer.so: build-multimedia

$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_mixer_intermediates/export_includes: $(TARGET_OUT_INTERMEDIATES)/lib/libste_audio_mixer.so
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_mixer_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_mixer_intermediates/export_includes


$(TARGET_OUT_INTERMEDIATES)/lib/libste_audio_mixer.so: $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/ste_omxcomponents/libste_audio_mixer.so
	@echo target STE MM: $@
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_mixer_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libste_audio_mixer_intermediates/export_includes
	$(hide) cp -f $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/ste_omxcomponents/libste_audio_mixer.so $(TARGET_OUT_INTERMEDIATES)/lib/

#--------------------------------------
# Library: libstelpcutils
#
$(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libstelpcutils.so: build-multimedia

$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libstelpcutils_intermediates/export_includes: $(TARGET_OUT_INTERMEDIATES)/lib/libstelpcutils.so
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libstelpcutils_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libstelpcutils_intermediates/export_includes

$(TARGET_OUT_INTERMEDIATES)/lib/libstelpcutils.so: $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libstelpcutils.so
	@echo target STE MM: $@
	$(hide) mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libstelpcutils_intermediates/
	$(hide) touch $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/libstelpcutils_intermediates/export_includes
	$(hide) cp -f $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/libstelpcutils.so $(TARGET_OUT_INTERMEDIATES)/lib/

