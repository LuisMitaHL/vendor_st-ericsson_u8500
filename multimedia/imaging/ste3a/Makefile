# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------

VPATH += nmxAawb/src/applications/art
VPATH += nmxAawb/src/applications/aawb
VPATH += STE3A/src
VPATH += STE3A_CMAL_Reference/src
VPATH += STE3A_FGAL_Reference/src
VPATH += STE3A_PPAL_8500V2/src

LIBNAME = ste3a

CFLAGS=-W -Wall -ansi
CFLAGS=-O3

CPPFLAGS += -I STE3A/include
CPPFLAGS += -I STE3A/include_debug
CPPFLAGS += -I nmxAawb/include
CPPFLAGS+= -I$(MMROOT)/imaging/include

LIBOBJ = cmal
LIBOBJ += fgal
LIBOBJ += nmxAawb_u8d
LIBOBJ += nmxAawb_u8f
LIBOBJ += nmxAawbClassifier_u8d
LIBOBJ += nmxAawbClassifier_u8f
LIBOBJ += nmxAawbColorMatrixInterpolation_d
LIBOBJ += nmxAawbColorMatrixInterpolation_f
LIBOBJ += nmxAawbGenLocusCoords_d
LIBOBJ += nmxAawbGenLocusCoords_f
LIBOBJ += nmxAawbGetKelvinFromLocusCoords_d
LIBOBJ += nmxAawbGetKelvinFromLocusCoords_f
LIBOBJ += nmxAawbGetLocusCoordsFromKelvins_d
LIBOBJ += nmxAawbIteration_u8d
LIBOBJ += nmxAawbIteration_u8f
LIBOBJ += nmxArtAdaptiveGamma_f
LIBOBJ += nmxArtBlackBoost_f
LIBOBJ += nmxArtComputeBlackBoostLut_f
LIBOBJ += nmxArtComputeGainBoostLut_f
LIBOBJ += nmxArtComputeLmhPoints_f
LIBOBJ += nmxArtComputeSCurveLut_f
LIBOBJ += nmxArtGainBoost_f
LIBOBJ += nmxArtNlToneMapping_fu8
LIBOBJ += nmxArtSCurve_f
LIBOBJ += ppal
LIBOBJ += ste3a
LIBOBJ += ste3a_types
LIBOBJ += steaec
LIBOBJ += steaeci
LIBOBJ += steafc
LIBOBJ += steafci
LIBOBJ += steart
LIBOBJ += stearti
LIBOBJ += steawb
LIBOBJ += steawbi
LIBOBJ += steawbi_initialize_tuning
LIBOBJ += stestatsi


INSTALL_LIB = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

todo: lib

VPATH += nmxAawb/include
VPATH += STE3A/include
VPATH += STE3A/include_debug
VPATH += STE3A_CMAL_Reference/include
VPATH += STE3A_FGAL_Reference/include
VPATH += STE3A_PPAL_8500V2/include

INSTALL_HEADER += cmal.h
INSTALL_HEADER += fgal.h
INSTALL_HEADER += nmxAawb.h
INSTALL_HEADER += nmxAawbFunctions.h
INSTALL_HEADER += ppal.h
INSTALL_HEADER += ste3a.h
INSTALL_HEADER += ste3a_aec_debug.h
INSTALL_HEADER += ste3a_aec_types.h
INSTALL_HEADER += ste3a_afc_debug.h
INSTALL_HEADER += ste3a_afc_types.h
INSTALL_HEADER += ste3a_art_debug.h
INSTALL_HEADER += ste3a_art_types.h
INSTALL_HEADER += ste3a_awb_debug.h
INSTALL_HEADER += ste3a_awb_types.h
INSTALL_HEADER += ste3a_calibration_types.h
INSTALL_HEADER += ste3a_cmal_types.h
INSTALL_HEADER += ste3a_debug.h
INSTALL_HEADER += ste3a_fgal_types.h
INSTALL_HEADER += ste3a_internal.h
INSTALL_HEADER += ste3a_ppal_types.h
INSTALL_HEADER += ste3a_stats_types.h
INSTALL_HEADER += ste3a_types.h
INSTALL_HEADER += steaec.h
INSTALL_HEADER += steaeci.h
INSTALL_HEADER += steafc.h
INSTALL_HEADER += steafci.h
INSTALL_HEADER += steart.h
INSTALL_HEADER += stearti.h
INSTALL_HEADER += steawb.h
INSTALL_HEADER += steawbi.h

showheaders: 
	echo ${INSTALL_HEADER}

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
NO_VARIANT=true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif



