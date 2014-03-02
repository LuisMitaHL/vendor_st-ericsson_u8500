# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------

#disable for YUV camera
#CPPFLAGS += -DSTAB #disable for YUV camera

ifeq ($(MMIO_ENABLE_NEW_IF),true)
  CPPFLAGS += -DMMIO_ENABLE_NEW_IF
endif

#enable dynamic debug trace
DEBUG=1

ifdef DEBUG
# dynamic debug: can be enable/disable by setprop command
CPPFLAGS += -D__DEBUG
LIBNAME   = st_ext_hsmcamera
INSTALL_LIB = libst_ext_hsmcamera.a
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = st_ext_hsmcamera
INSTALL_LIB = libst_ext_hsmcamera.a
endif

# check for aptina sensor
ifeq (MT9P111,$(filter MT9P111%,$(CAMERA_SET_PRIMARY_SENSOR)))
CPPFLAGS += -DSTE_SENSOR_MT9P111=1
endif #module configuration aptina sensor

CPPFLAGS  += -I$(MULTIMEDIA_PATH)/shared/utils/include
CPPFLAGS  += -I$(MULTIMEDIA_PATH)/shared/host_trace
CPPFLAGS  += -DDBGT_CONFIG_DEBUG -DDBGT_CONFIG_AUTOVAR 
CPPFLAGS  += -DDBGT_TAG=\"ExtCamera\"
LDFLAGS += -llog -lcutils -lstelpcutils
# check for aptina sensor
ifeq (MT9V113,$(filter  MT9V113%,$(CAMERA_SET_SECONDARY_SENSOR)))
CPPFLAGS += -DSTE_SENSOR_MT9V113=1
endif #module configuration aptina sensor

VPATH += src/sia_platform_manager \
		 src/opmode_transition \

LIBOBJ   = 	camera_ctrllib \
			opmodemgr \
			preview_statemachine \
			capture_statemachine \
			sia_platform_manager \
			boot_statemachine \
			camera \
			hsmcam \
			grabctl \
			grab \
			omxcamera \
			port_statemachine \
			hostnmf_cb \
			camport \
			capture_context

INSTALL_HEADER = $(subst include/,,$(shell find include -type f -name "*.h" ))

CPPFLAGS+= \
		-I . \
		-I $(MMROOT)/imaging/include \
		-I $(REPOSITORY) \
		-I include/sia_component_manager 

# Generation of solibs
IS_OMX_COMPONENT_LIB=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES = ste_ext_camera
LIBOBJ_$(SOLIBNAMES) = ste_omx_ens_component
IMAGE_LIBS = $(LIBNAME) st_ext_sia_itfs st_stab st_i2c st_ifm st_mmio host_trace camera_flash

IMAGING_NMF_HOST_COMPONENTS = ext_img_network

SOLIBS_ste_ext_camera += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_ste_ext_camera= $(IMAGE_LIBS) 

FLAGS_$(SOLIBNAMES) += -lste_ext_ens_image_common 
FLAGS_$(SOLIBNAMES) += $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -llos -lnmftimer
INSTALL_SOLIB = $(SOLIBNAMES)


todo: lib solib 

install: ramdisk
todo: ramdisk

# Copy the config file into the Linux ramdisk
RAMDISK_DIR = $(MMROOT)/imaging/rootfs/$(MMPLATFORM)

ifeq ($(findstring android,$(PLATFORM)),android)
RAMDISK_DIR = $(MMROOT)/imaging/rootfs/$(MMPLATFORM)/system
endif

ramdisk: Makefile

define ExportToRamdisk
	mkdir -p $(RAMDISK_DIR)/usr/share/camera/config_file; 
	rm -f $(RAMDISK_DIR)/usr/share/camera/config_file/*.*
	cp ./config_file/*.* $(RAMDISK_DIR)/usr/share/camera/config_file;
endef

ifeq ($(findstring linux,$(PLATFORM)),linux)
	$(ExportToRamdisk)
endif

ifeq ($(findstring android,$(PLATFORM)),android)
	$(ExportToRamdisk)
endif

TOBECLEAN = 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
