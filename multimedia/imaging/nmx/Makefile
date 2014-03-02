# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------

VPATH += nmx/src/core/base
VPATH += nmx/src/core/functions
VPATH += nmx/src/core/functions/lut
VPATH += nmx/src/core/operations
VPATH += nmx/src/core/util
VPATH += nmx/src/geometry
VPATH += nmx/src/image
VPATH += nmx/src/optimization
VPATH += nmx/src/io
VPATH += nmx/src/statistics
VPATH += nmx/src/filters
VPATH += nmx/src/resampling
VPATH += nmx/src/debug
VPATH += nmx/include

LIBNAME = st_nmx

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
LIBNAME  := $(addsuffix _debug, $(LIBNAME))
CFLAGS=-g
OBJ_EXT =odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
CFLAGS=-O3
endif

ifeq ($(MYMMBOARD),svp8500_v1)
CPPFLAGS += -D_SVP_
endif

CPPFLAGS+=-D__UI_REFERENCE

LIBOBJ  = nmxCopy_a
LIBOBJ += nmxMeshgrid_d
LIBOBJ += nmxMeshgridRC_d
LIBOBJ += nmxMeshRGrid_d
LIBOBJ += nmxOnes_d
LIBOBJ += nmxOnes_f
LIBOBJ += nmxZeros_d
LIBOBJ += nmxZeros_f
LIBOBJ += nmxMean_d
LIBOBJ += nmxMean_f
LIBOBJ += nmxSqr_d
LIBOBJ += nmxSqr_f
LIBOBJ += nmxSum_d
LIBOBJ += nmxSum_f
LIBOBJ += nmxLutDecimate_d
LIBOBJ += nmxLutDecimate_f
LIBOBJ += nmxLutLookup_d
LIBOBJ += nmxLutLookup_f
LIBOBJ += nmxAdd_d
LIBOBJ += nmxAdd_i16
LIBOBJ += nmxAdd_i
LIBOBJ += nmxAdd_u8
LIBOBJ += nmxDivide_d
LIBOBJ += nmxDot_d
LIBOBJ += nmxDot_i
LIBOBJ += nmxDotT1_f
LIBOBJ += nmxDotT2_d
LIBOBJ += nmxDotT2_i
LIBOBJ += nmxDotTR_f
LIBOBJ += nmxInv3x3_d
LIBOBJ += nmxMultiply_d
LIBOBJ += nmxSubtract_d
LIBOBJ += nmxAdd_f
LIBOBJ += nmxAdd_i8
LIBOBJ += nmxAdd_u16
LIBOBJ += nmxAdd_u
LIBOBJ += nmxDivide_f
LIBOBJ += nmxDot_f
LIBOBJ += nmxDotT1_d
LIBOBJ += nmxDotT1_i
LIBOBJ += nmxDotT2_f
LIBOBJ += nmxDotTR_d
LIBOBJ += nmxDotTR_i
LIBOBJ += nmxInv3x3_f
LIBOBJ += nmxMultiply_f
LIBOBJ += nmxSubtract_f
LIBOBJ += nmxMem
LIBOBJ += nmxExtendBoundaryGrid_d
LIBOBJ += nmxPlaneRGrid_d
LIBOBJ += nmxPlaneRGridSd_d
LIBOBJ += nmxPoints2PieceWiseLinearProjection_di
LIBOBJ += nmxResampleGrid_f
LIBOBJ += nmxExtendBoundaryGrid_f
LIBOBJ += nmxPlaneRGrid_f
LIBOBJ += nmxPlaneRGridSd_f
LIBOBJ += nmxResampleGrid_d
LIBOBJ += nmxBlur124_du
LIBOBJ += nmxBlur124_i8u
LIBOBJ += nmxBlur124_u8u
LIBOBJ += nmxBlur14_fu
LIBOBJ += nmxBlur14_iu
LIBOBJ += nmxBlur14_u
LIBOBJ += nmxBlur18_i16u
LIBOBJ += nmxBlur18_u16u
LIBOBJ += nmxImHessian_d
LIBOBJ += nmxImSdUpdate_f
LIBOBJ += nmxBlur124_fu
LIBOBJ += nmxBlur124_iu
LIBOBJ += nmxBlur124_u
LIBOBJ += nmxBlur14_i16u
LIBOBJ += nmxBlur14_u16u
LIBOBJ += nmxBlur18_du
LIBOBJ += nmxBlur18_i8u
LIBOBJ += nmxBlur18_u8u
LIBOBJ += nmxImHessian_f
LIBOBJ += nmxBlur124_i16u
LIBOBJ += nmxBlur124_u16u
LIBOBJ += nmxBlur14_du
LIBOBJ += nmxBlur14_i8u
LIBOBJ += nmxBlur14_u8u
LIBOBJ += nmxBlur18_fu
LIBOBJ += nmxBlur18_iu
LIBOBJ += nmxBlur18_u
LIBOBJ += nmxImSdUpdate_d
LIBOBJ += nmxPlane2SurfFit_di
LIBOBJ += nmxPlane2SurfFit_fi
LIBOBJ += nmxCentroid1_f
LIBOBJ += nmxEnumerate_if
LIBOBJ += nmxGaussMixtures2Pdfs_f
LIBOBJ += nmxGetMatrixP2DS_f
LIBOBJ += nmxLinearIntersection_f
LIBOBJ += nmxLog_f
LIBOBJ += nmxLutMonotonicLinear_f
LIBOBJ += nmxLutOversamplingLinear_f
LIBOBJ += nmxLutStepLinear_f
LIBOBJ += nmxRoundTypeConvert_fu8
LIBOBJ += nmxSetMatrixP2DS_f
LIBOBJ += nmxSetMatrixP2D_f
LIBOBJ += nmxSetMatrixP2D_u8
LIBOBJ += nmxSetMatrixP3DS_u8
LIBOBJ += nmxSetMatrixP3D_f
LIBOBJ += nmxSetScalar_f
LIBOBJ += nmxCentroid_f
LIBOBJ += nmxGet3Elements_f
LIBOBJ += nmxGetMatrixP2DS_u8
LIBOBJ += nmxGetMatrixP3DS_f
LIBOBJ += nmxGetMatrixP3DS_u8
LIBOBJ += nmxMax_f
LIBOBJ += nmxMin_f
LIBOBJ += nmxPoint2PieceWiseLinearProjection_fi
LIBOBJ += nmxTranspose_a
LIBOBJ += nmxGaussMix_f
LIBOBJ += nmxGetMatrix2D
LIBOBJ += nmxGet2Elements_f
LIBOBJ += nmxGetMatrix2S
LIBOBJ += nmxMinusInf_f
LIBOBJ += nmxInf_f
LIBOBJ += nmxRevision_u
LIBOBJ += nmxSetMatrixP3DS_f
LIBOBJ += nmxDivideNan0_f
LIBOBJ += nmxMin2_f
LIBOBJ += nmxEval1_f
LIBOBJ += nmxInterpolationLinear_d
LIBOBJ += nmxSqrt_f
LIBOBJ += nmxInterpolationLinear_f
LIBOBJ += nmxSetMatrixP2D_u
LIBOBJ += nmxGetMatrixP2DS_d 
LIBOBJ += nmxGetMatrixP3DS_d
LIBOBJ += nmxDecode_a
LIBOBJ += nmxSetMatrixP2D_d
LIBOBJ += nmxSetScalar_d
LIBOBJ += nmxFilter121_du
LIBOBJ += nmxFilter121_fu
LIBOBJ += nmxEval1_i
LIBOBJ += nmxEval1_i16
LIBOBJ += nmxEval1_i8
LIBOBJ += nmxEval1_u
LIBOBJ += nmxEval1_u16
LIBOBJ += nmxEval1_u8
LIBOBJ += nmxEval2_i
LIBOBJ += nmxEval2_i16
LIBOBJ += nmxEval2_i8
LIBOBJ += nmxEval2_u
LIBOBJ += nmxEval2_u16
LIBOBJ += nmxEval2_u8
LIBOBJ += nmxEval1_d
LIBOBJ += nmxEval2_d
LIBOBJ += nmxEval2_f
LIBOBJ += nmxGetMatrixP2D_f
LIBOBJ += nmxGetMatrixP2D_u
LIBOBJ += nmxZeros_u
LIBOBJ += nmxResampleGridBicubic_fu
LIBOBJ += nmxResampleCubic_fu
LIBOBJ += nmxMax2_f
LIBOBJ += nmxGet3Elements_d
LIBOBJ += nmxGet2Elements_d
LIBOBJ += nmxGetMatrixP2D_d
LIBOBJ += nmxSetMatrixP2DS_d
LIBOBJ += nmxPoint2PieceWiseLinearProjection_di
LIBOBJ += nmxCentroid_d
LIBOBJ += nmxDivideNan0_d
LIBOBJ += nmxMax_d
LIBOBJ += nmxLutStepLinear_d
LIBOBJ += nmxSetMatrixP3DS_d
LIBOBJ += nmxSqrt_d
LIBOBJ += nmxMin2_d
LIBOBJ += nmxMin_d
LIBOBJ += nmxLutOversamplingLinear_d
LIBOBJ += nmxMinusInf_d
LIBOBJ += nmxInf_d
LIBOBJ += nmxCopyMatrixPDS_a
LIBOBJ += nmxSetMatrix2D_a
LIBOBJ += nmxGetMatrixP3D_f
LIBOBJ += nmxFlipMatrixD_a
LIBOBJ += nmxGetMatrixP4DS_f
LIBOBJ += nmxRound_f
LIBOBJ += nmxSetMatrixP4D_u
LIBOBJ += nmxGetMatrixP4DS_u
LIBOBJ += nmxDecodeSet_a
LIBOBJ += nmxGetMatrixP4D_f
LIBOBJ += nmxCopyMatrixD_a
LIBOBJ += nmxCropMatrix_a
LIBOBJ += nmxReciprocal_f
LIBOBJ += nmxCubicMax_fu
LIBOBJ += nmxZeros_u8
LIBOBJ += nmxDivideBy00_d
LIBOBJ += nmxDivideBy00_f
LIBOBJ += nmxCropMatrix2D_a
LIBOBJ += nmxCropMatrix3D_a
LIBOBJ += nmxGetFirstElement_d
LIBOBJ += nmxSetElement_d
LIBOBJ += nmxMax2_d
LIBOBJ += nmxGetFirstElement_f
LIBOBJ += nmxSetElement_f
LIBOBJ += nmxRamp_f
LIBOBJ += nmxSetMatrixP2DS_u
LIBOBJ += nmxTypeConvert2Float_uf
LIBOBJ += nmxHistGainsRgbTransform_f
LIBOBJ += nmxHistSubtractOffsetTransform_f
LIBOBJ += nmxHistGainTransform_f
LIBOBJ += nmxCumsum_fu
LIBOBJ += nmxDiff00_fu
LIBOBJ += nmxGetVectorPDSD_f
LIBOBJ += nmxGetVectorVal_fu
LIBOBJ += nmxGamma_f
LIBOBJ += nmxLutComposeFunctions_f
LIBOBJ += nmxHistNlTransform_f
LIBOBJ += nmxLutInvFunction_f
LIBOBJ += nmxGetVectorDimension_a
LIBOBJ += nmxHistMinMaxPoints_f
LIBOBJ += nmxSigmoid_f
LIBOBJ += nmxPow_f



CPPFLAGS+= -I nmx/include 

INSTALL_HEADER  = nmx.h
INSTALL_HEADER += nmxDatatype.h
INSTALL_HEADER += nmxMem.h
INSTALL_HEADER += nmxMathdefs.h
INSTALL_HEADER += nmxMacros.h
INSTALL_HEADER += nmxError.h
INSTALL_HEADER += nmxFunctions.h

INSTALL_LIB = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))


todo: lib

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

