# vim:syntax=make:
# -*- Mode: Makefile -*-

CPPFLAGS += -I${MM_HEADER_BASEDIR}/imaging/OMX_extensions 

#-------------------------------------------
# imaging required

IMAGING_LIBS += st_ifm st_hsmcamera st_tuning st_isp8500_firmware_secondary st_isp8500_firmware_primary st_sia_itfs st_imgcommon st_ispproc st_stab st_tuningdatabase

IMAGING_LIBS += st_nmx st_damper

# excluded for svp5500 ptfm
ifneq ($(findstring 5500,$(MMBOARD)),5500) 
  IMAGING_LIBS += st_omxnorcos st_omxredeye_detector st_omxredeye_corrector st_omxambr st_omxarmivproc st_omxBridge \
		st_wrapper_openmax st_aiq_common st_omxface_detector sti_face_detector st_omxsplitter st_omxFakeSink st_omxscene_detector 
endif

SHARED_LIBS += st_mmio st_variationif camera_flash st_i2c camera_flash flash_current_calculation als


# sinks and testing utilities
IMAGING_LIBS += imgfilter ite_omx_cmd ite_common ite_darkbox \
		imgStreamRecorder imgStreamPlayer st_omximageprocessor \
		st_imageprocessing fakesource fakesink 
IMAGING_LIBS += sti_face_detector


#-------------------------------------------
#  NMF ON HOST
IMAGING_NMF_HOST_COMPONENTS +=  img_network \
			imageproc_proc_wrp asyncflash_wrp

IMAGING_NMF_HOST_COMPONENTS += sw_3A_wrp_a

# excluded for svp5500 ptfm
ifneq ($(findstring 5500,$(MMBOARD)),5500) 
  IMAGING_NMF_HOST_COMPONENTS += openmax_processor_wrp
endif

# stab integration: resource_sharerWrapped
IMAGING_NMF_HOST_COMPONENTS += resource_sharerWrapped tuningloader_wrp 

# sinks and testing utilities
IMAGING_NMF_HOST_COMPONENTS +=  imgvalid_wrp fake_source_wrp  fake_sink_wrp \
			imgStreamRecorderNMF_wrp imgStreamPlayerNMF_wrp 

BELLAGIO_IMAGING_LIBS  += ite_common 


.DEFAULT_GOAL := all

