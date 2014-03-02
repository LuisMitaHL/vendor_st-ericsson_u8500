
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(MMSYSTEM),android)
CPPFLAGS+= -I$(KERNEL_BUILD_DIR)/include
  ## To find needed headers in case kernel objects are not generated in kernel source tree
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/kernel/include
endif

LIBNAME = ite_common
LIBOBJ = $(COMMON_OBJ) 

INSTALL_LIB = lib$(LIBNAME).a 

ifneq ($(PLATFORM),x86-linux)
LIBNAME_2 = ite_darkbox
LIBOBJ_2 = $(DARKYBOX_OBJ)
INSTALL_LIB += lib$(LIBNAME_2).a 
endif

INSTALL_HEADER = $(COMMON_HEADER) $(DARKYBOX_HEADER)


ifndef NO_USB
CPPFLAGS += -DMMTE_USB_TRUE 
endif

#override generic Makefile's -03
CFLAGS = -g -O0
ifdef METRICS_TRACE_WB
CPPFLAGS += -DMETRICS_TRACE_WB
endif

#ifdef STRIDE_IMPLEMENTATION_CORRECT
CPPFLAGS += -DSTRIDE_IMPLEMENTATION_CORRECT
#endif

VPATH += inc/metrics src/metrics 
VPATH += inc/common src/common 

COMMON_OBJ = \
algoautofocuseffect \
algoblackandwhiteeffect \
algobrightnesseffect \
algocolortoneeffect \
algoconfigfileparser\
algocontrasteffect \
algoembosseffect \
algoextractionroutines \
algofileroutines \
algofilterroutines \
algogammaeffect\
algointerface \
algomemoryroutines \
algomirroringeffect\
algonegativeeffect \
algorotationeffect\
algosaturationeffect \
algosepiaeffect \
algosolarizeeffect \
algospatialsimilareffect \
algostring \
algotestfileparser \
algoutilities \
algowhitebalanceeffect \
algozoomeffect \
algotestingmode \
algoexposurepreset \
algoexposurevalue \
algoluminancemean \
algocenterfieldofview \
ite_exif \

ifneq ($(PLATFORM),x86-linux)
DARKYBOX_OBJ = ssc ite_darky_box_cmd 
endif


COMMON_HEADER = \
algoautofocuseffect.h \
algoblackandwhiteeffect.h \
algocolortoneeffect.h \
algodebug.h \
algoembosseffect.h \
algoerror.h \
algoextractionroutines.h \
algofileroutines.h \
algofilterroutines.h \
algointerface.h \
algomemoryroutines.h \
algonegativeeffect.h \
algosepiaeffect.h \
algospatialsimilareffect.h \
algostring.h \
algotestfileparser.h \
algotypedefs.h \
algoutilities.h \
algowhitebalanceeffect.h \
algozoomeffect.h \
algobrightnesseffect.h \
algocontrasteffect.h \
algosaturationeffect.h \
algosolarizeeffect.h \
algoconfigfileparser.h\
algogammaeffect.h\
algorotationeffect.h\
algomirroringeffect.h\
algotestingmode.h \
algoexposurepreset.h \
algoexposurevalue.h \
algoluminancemean.h \
algocenterfieldofview.h \
ite_exif.h \

ifneq ($(PLATFORM),x86-linux)
DARKYBOX_HEADER = ssc.h ite_darky_box_cmd.h 
endif

MK_FILES := $(addprefix $(INSTALL_MAKEFILES_DIR)/, $(wildcard *.mk))


todo: lib ramdisk $(MK_FILES) install_make

$(INSTALL_MAKEFILES_DIR)/%: %
	@echo "  MMINSTALL $<"
	$(VERCHAR)$(MMINSTALL) $< $@


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 COMPONENT_TYPE=TEST
 include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif

MMVALIDROOT = usr/share/mm-valid

ramdisk: Makefile
# Copy the metric config file into the Linux ramdisk
ifeq ($(findstring linux,$(PLATFORM)),linux)
	$(MMINSTALL)  src/metrics/metricconfigfile.dat $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/ite/src/metrics/metricconfigfile.dat
	$(MMINSTALL) -d metric/inputfiles/colorformat $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/ite/metric/inputfiles/colorformat
endif

install_make:
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	@echo "Creating links for backward compatibility"
	ln -fs MMTE_Imaging.mk  Make.mmte
endif

ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
clean realclean: clean_make

clean_make:
	$(RM) Make.mmte
endif


help:
	@echo "  make realclean "
	@echo "  make [NO_USB=1] [FLAT=1] all  ->generate libite_common "
	@echo "  NO_USB=1 : causes a slightly different libite_common.a "
	@echo "  FLAT=1 : for buffer addressing logical=physical "

