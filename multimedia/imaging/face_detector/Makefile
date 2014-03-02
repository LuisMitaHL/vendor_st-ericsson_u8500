# vim:syntax=make:
# -*- Mode: Makefile -*-
# Just export header and lib if make.build is not present

#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x

# This will make use of RVCT under Linux and allow to deliver pre-build library
#export USE_OPT_COMPILER=true

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifdef DEBUG
LIBNAME   =st_omxface_detector_debug
SOLIBNAME =st_omxface_detector_debug
else
LIBNAME   =st_omxface_detector
SOLIBNAME =st_omxface_detector
endif

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarFace_Detector -DCONFIG_DEBUG_AUTOVAR

INSTALL_HEADER = OMX_extensions/face_detector_extensions.h
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#-------------------------------
# Generic Rules
#-------------------------------
all todo lib slib install: install_lib_sti
	if test -f STI_facedetect/Makefile; then echo "make -C STI_face_detector  $@" && $(MAKE) -C STI_facedetect $@ ;fi
	$(MAKE) -f Make_build $@

clean realclean uninstall:
	if test -f STI_facedetect/Makefile; then echo "make STI_face_detector $@" && $(MAKE) -C STI_facedetect $@ ;fi
	$(MAKE) -f Make_build $@

install_lib_sti:
	if test ! -f STI_facedetect/Makefile; then $(MMINSTALL) $(COMPONENT_TOP_DIR)/STI_facedetect/$(LIB_DIR_NAME)/cortexA9-x-rvct4gnu-x/libsti_face_detector.a $(INSTALL_LIB_DIRNAME)/cortexA9-x-rvct4gnu-x/libsti_face_detector.a;fi
ifeq ($(PLATFORM),x86-linux)
	if test ! -f STI_facedetect/Makefile; then $(MMINSTALL) $(COMPONENT_TOP_DIR)/STI_facedetect/$(LIB_DIR_NAME)/x86/libsti_face_detector.a $(INSTALL_LIB_DIRNAME)/x86/libsti_face_detector.a;fi
endif

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e "$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)"


