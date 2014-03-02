/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4enc 
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_ParamAndConfigTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#include "MPEG4Enc_ParamAndConfig.h"
#include "MPEG4Enc_Proxy.h"
#include "MPEG4Enc_Port.h"

#ifndef DEFAULT_FRAME_RATE
	#define DEFAULT_FRAME_RATE 15
#endif

#ifndef DEFAULT_BITRATE
	#define DEFAULT_BITRATE 64000
#endif



#define PROFILE_LEVEL_STRICT_CHECKING
#define RETURN_XXX_IF_WRONG_ELSE_BREAK(_x, _error)                { if (!(_x)) { return (_error); } else break;}

//	SP levels  									{ 0		, 0b	, 1 	, 2		, 3			, 4			, 4a		, 5			, max		}
static OMX_U32 sp_bitrate_level_values[9] 	= 	{64000	, 64000	, 64000	, 128000, 384000	, 4000000	, 4000000	, 8000000	, 12000000 	};
static OMX_U32 sp_framerate_level_values[9]	= 	{15		, 15 	, 15 	, 15 	, 30 		, 30		, 30		, 30		, 30 		};
static OMX_U32 sp_mbpersec_level_values[9]	=	{1485	, 1485	, 1485	, 5940	, 11880		, 36000		, 36000		, 40500		, 108000 	};

//	SH levels									{ 10	, 20	, 30	, 40		, 45 }
static OMX_U32 sh_bitrate_level_values[5] = 	{128000	, 128000, 384000, 2000000	, 128000 };
static OMX_U32 sh_framerate_level_values[5] = 	{15		, 15	, 30	, 30		, 15 };

MPEG4Enc_ParamAndConfig::MPEG4Enc_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::MPEG4Enc_ParamAndConfig() constructor");

	m_crop_param.nSize = sizeof(OMX_CONFIG_RECTTYPE);
	m_crop_param.nPortIndex = VPB+1;
	m_crop_param.nLeft = 0;
	m_crop_param.nTop = 0;
	m_crop_param.nWidth = 0;
	m_crop_param.nHeight = 0;

	m_Qp.nSize = sizeof(OMX_VIDEO_PARAM_QUANTIZATIONTYPE);
	m_Qp.nPortIndex = VPB+0;
	m_Qp.nQpI = 5;
	m_Qp.nQpP = 5;
	m_Qp.nQpB = 5;

	m_fast_update.nSize = sizeof(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE);
	m_fast_update.nPortIndex = VPB+0;
	m_fast_update.bEnableVFU = OMX_FALSE;
	m_fast_update.nFirstGOB = 0;
	m_fast_update.nFirstMB = 0;
	m_fast_update.nNumMBs = 0;

	m_bitrate.nSize = sizeof(OMX_VIDEO_PARAM_BITRATETYPE);
	m_bitrate.nPortIndex = VPB+1;
	m_bitrate.eControlRate = OMX_Video_ControlRateDisable;	//rate control type
	m_bitrate.nTargetBitrate = DEFAULT_BITRATE;

	m_intra_ref.nSize = sizeof(OMX_VIDEO_PARAM_INTRAREFRESHTYPE);
	m_intra_ref.nPortIndex = VPB+0;
	m_intra_ref.eRefreshMode = OMX_VIDEO_IntraRefreshMax;
	m_intra_ref.nAirMBs = 0;
	m_intra_ref.nAirRef = 0;
	m_intra_ref.nCirMBs = 0;

	m_err_corr.nSize = sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE);
	m_err_corr.nPortIndex = VPB+0;
	m_err_corr.bEnableHEC = OMX_FALSE;
	m_err_corr.bEnableResync = OMX_FALSE;     
	m_err_corr.nResynchMarkerSpacing = 0;
	m_err_corr.bEnableDataPartitioning = OMX_FALSE;
	m_err_corr.bEnableRVLC = OMX_FALSE;

	m_enc_param.nSize = sizeof(OMX_VIDEO_PARAM_MPEG4TYPE);
	m_enc_param.nPortIndex = VPB+0;
	m_enc_param.nSliceHeaderSpacing = 0;
	m_enc_param.bSVH = OMX_FALSE;
	m_enc_param.bGov = OMX_FALSE;
	m_enc_param.nPFrames = 14;
	m_enc_param.nBFrames = 0;
	m_enc_param.nIDCVLCThreshold = 0;
	m_enc_param.bACPred = OMX_FALSE;
	m_enc_param.nMaxPacketSize = 0x0100;
	m_enc_param.nTimeIncRes = DEFAULT_FRAME_RATE;
	m_enc_param.eProfile = OMX_VIDEO_MPEG4ProfileSimple;
	m_enc_param.eLevel = OMX_VIDEO_MPEG4LevelMax; //< in header writing we are interpreting LevelMax as Level 6.
	m_enc_param.nAllowedPictureTypes = 2;
	m_enc_param.nHeaderExtension = 0;
	m_enc_param.bReversibleVLC = OMX_FALSE;

	// initialization of attributes of VFM_ParamAndConfig
	setProfileLevel(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4LevelMax);

	m_enc_h263_param.nSize = sizeof(OMX_VIDEO_PARAM_H263TYPE);
	m_enc_h263_param.nPortIndex = VPB+0;
	m_enc_h263_param.nPFrames = 14;
	m_enc_h263_param.nBFrames = 0;
	m_enc_h263_param.eProfile = OMX_VIDEO_H263ProfileBaseline;
	m_enc_h263_param.eLevel = OMX_VIDEO_H263Level40;
	m_enc_h263_param.bPLUSPTYPEAllowed = OMX_FALSE;
	m_enc_h263_param.nAllowedPictureTypes = 2;
	m_enc_h263_param.bForceRoundingTypeToZero = OMX_FALSE;
	m_enc_h263_param.nPictureHeaderRepetition = 14;
	m_enc_h263_param.nGOBHeaderInterval = 0;

	//< m_vos_header.nSize = sizeof(VFM_VIDEO_PARAM_WRITEVOSHEADER);
	//< m_vos_header.isSystemHeaderAddBeforeIntra = OMX_TRUE;

	//configs

	m_framerate.nSize = sizeof(OMX_CONFIG_FRAMERATETYPE);
	m_framerate.nPortIndex = VPB+0;
	m_framerate.xEncodeFramerate = DEFAULT_FRAME_RATE << 16;

	m_force_intra.nSize = sizeof(OMX_CONFIG_INTRAREFRESHVOPTYPE);
	m_force_intra.nPortIndex = VPB+1;
	m_force_intra.IntraRefreshVOP = OMX_FALSE;

	m_mb_errormap.nSize = sizeof(OMX_CONFIG_MACROBLOCKERRORMAPTYPE);
	m_mb_errormap.nPortIndex = VPB+0;
	m_mb_errormap.nErrMapSize = 0;
	m_mb_errormap.ErrMap[0] = 0;

	m_motion_vec.nSize = sizeof(OMX_VIDEO_PARAM_MOTIONVECTORTYPE);
	m_motion_vec.nPortIndex = VPB+0;
	//< dummy variables, these features in FW are not controllable by user, they are
	//< automatically chosen by FW, just added here for purpose of compatibility
	m_motion_vec.eAccuracy = OMX_Video_MotionVectorHalfPel; 
	m_motion_vec.bUnrestrictedMVs = OMX_TRUE ;
	m_motion_vec.bFourMV = OMX_FALSE;
	m_motion_vec.sXSearchRange = 0;
	m_motion_vec.sYSearchRange = 0;

	//>m_color_primary.nSize = sizeof(OMX_PARAM_COLORPRIMARY);
	//>m_color_primary.nPortIndex = VPB+0;
	m_color_primary = OMX_ColorPrimaryUnknown;

	m_pixel_aspect_ratio.nSize = sizeof(OMX_PARAM_PIXELASPECTRATIO);
	m_pixel_aspect_ratio.nPortIndex = VPB+0;
	m_pixel_aspect_ratio.nHorizontal = 1;
	m_pixel_aspect_ratio.nVertical = 1;

	m_deblocking.nSize = sizeof(OMX_PARAM_DEBLOCKINGTYPE);
	m_deblocking.nPortIndex = VPB+1;
	m_deblocking.bDeblocking = OMX_FALSE;

	/* +Change start for CR338066 MPEG4Enc DCI */
	m_short_dci.nSize = sizeof(VFM_PARAM_MPEG4SHORTDCITYPE);
	m_short_dci.nPortIndex = VPB+1;
	m_short_dci.bEnableShortDCI = OMX_FALSE;
	/* -Change end for CR338066 MPEG4Enc DCI */

	/* +Change start for CR343589 Rotation */
	m_rotation.nSize = sizeof(OMX_CONFIG_ROTATIONTYPE);
	m_rotation.nPortIndex = VPB+1;
	m_rotation.nRotation = 0;
	/* -Change end for CR343589 Rotation */

	m_nSvaMcps = 50;

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::MPEG4Enc_ParamAndConfig()");
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setInitialCropParams()
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setInitialCropParams()");

	MPEG4Enc_Port *port = (MPEG4Enc_Port*)(getComponent()->getPort(0));

	m_crop_param.nLeft = 0;
	m_crop_param.nTop = 0;
	m_crop_param.nWidth = port->getFrameWidth();
	m_crop_param.nHeight = port->getFrameHeight();
	OstTraceInt3(TRACE_FLOW, "MPEG4EncParam: Initial Crop Params width (%d) height (%d) line no %d \n",m_crop_param.nWidth,m_crop_param.nHeight,__LINE__);

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setInitialCropParams()");
	
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::reset()
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoQuantization(OMX_VIDEO_PARAM_QUANTIZATIONTYPE *p_Qp, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoQuantization()");
	OstTraceFiltInst2(TRACE_FLOW, "nQpI = %d, nQpP = %d", p_Qp->nQpI, p_Qp->nQpP);
	
	*p_has_changed = (OMX_BOOL)((m_Qp.nQpI != p_Qp->nQpI) || (m_Qp.nQpP != p_Qp->nQpP));
	m_Qp = *p_Qp;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoQuantization()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoQuantization(OMX_VIDEO_PARAM_QUANTIZATIONTYPE *p_Qp) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoQuantization()");

	*p_Qp = m_Qp;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoQuantization()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoFastUpdate(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *p_fast_update, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoFastUpdate()");

	OstTraceFiltInst4(TRACE_FLOW, "p_fast_update->bEnableVFU = %d, p_fast_update->nFirstGOB = %d, p_fast_update->nFirstMB = %d, p_fast_update->nNumMBs = %d", p_fast_update->bEnableVFU, p_fast_update->nFirstGOB, p_fast_update->nFirstMB, p_fast_update->nNumMBs);
	*p_has_changed = OMX_TRUE;
	m_fast_update = *p_fast_update;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoFastUpdate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoFastUpdate(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *p_fast_update) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoFastUpdate()");

	*p_fast_update = m_fast_update;
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoFastUpdate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE *p_bitrate, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoBitrate()");
	OstTraceFiltInst2(TRACE_FLOW, "eControlRate = %d, nTargetBitrate = %d", p_bitrate->eControlRate, p_bitrate->nTargetBitrate);
	
	OMX_ERRORTYPE err = OMX_ErrorNone;
	
	*p_has_changed = OMX_FALSE;

#ifdef PROFILE_LEVEL_STRICT_CHECKING
	err = check_profile_level(BITRATE_TYPE, (OMX_PTR)p_bitrate);
#endif

	if(err == OMX_ErrorNone)
	{
		*p_has_changed = (OMX_BOOL)((m_bitrate.eControlRate != p_bitrate->eControlRate)||(m_bitrate.nTargetBitrate != p_bitrate->nTargetBitrate));
		m_bitrate = *p_bitrate; 
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoBitrate()");

	return err;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE *p_bitrate) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoBitrate()");

	*p_bitrate = m_bitrate;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoBitrate()");

	return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoMotionVector(OMX_VIDEO_PARAM_MOTIONVECTORTYPE *p_motion_vec, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoMotionVector()");
	OstTraceFiltInst3(TRACE_FLOW, "p_motion_vec->bFourMV = %d, p_motion_vec->bUnrestrictedMVs = %d, p_motion_vec->eAccuracy = %d", p_motion_vec->bFourMV, p_motion_vec->bUnrestrictedMVs, p_motion_vec->eAccuracy);
	OstTraceFiltInst2(TRACE_FLOW, "p_motion_vec->sXSearchRange = %d, p_motion_vec->sYSearchRange = %d", p_motion_vec->sXSearchRange, p_motion_vec->sYSearchRange);
	m_motion_vec = *p_motion_vec;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoMotionVector()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoMotionVector(OMX_VIDEO_PARAM_MOTIONVECTORTYPE *p_motion_vec) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoMotionVector()");
	
	*p_motion_vec = m_motion_vec;	
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoMotionVector()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoIntraRefresh(OMX_VIDEO_PARAM_INTRAREFRESHTYPE *p_intra_ref , OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoIntraRefresh()");
	OstTraceFiltInst4(TRACE_FLOW, "p_intra_ref->eRefreshMode = %d, p_intra_ref->nAirMBs = %d, p_intra_ref->nAirRef = %d, p_intra_ref->nCirMBs = %d",p_intra_ref->eRefreshMode, p_intra_ref->nAirMBs, p_intra_ref->nAirRef, p_intra_ref->nCirMBs);
	
	*p_has_changed = OMX_TRUE;
	m_intra_ref = *p_intra_ref;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoIntraRefresh()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoIntraRefresh(OMX_VIDEO_PARAM_INTRAREFRESHTYPE *p_intra_ref) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoIntraRefresh()");

	*p_intra_ref = m_intra_ref;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoIntraRefresh()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoErrorCorrection(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *p_err_corr, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoErrorCorrection()");
	OstTraceFiltInst4(TRACE_FLOW, "p_err_corr->bEnableDataPartitioning = %d, p_err_corr->bEnableHEC = %d, p_err_corr->bEnableResync = %d, p_err_corr->bEnableRVLC = %d", p_err_corr->bEnableDataPartitioning, p_err_corr->bEnableHEC, p_err_corr->bEnableResync, p_err_corr->bEnableRVLC);
	OstTraceFiltInst1(TRACE_FLOW, "p_err_corr->nResynchMarkerSpacing = %d", p_err_corr->nResynchMarkerSpacing);

	*p_has_changed = OMX_TRUE;
	m_err_corr = *p_err_corr;

	m_enc_param.bReversibleVLC = m_err_corr.bEnableRVLC;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoErrorCorrection()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoErrorCorrection(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *p_err_corr) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoErrorCorrection()");

	*p_err_corr = m_err_corr;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoErrorCorrection()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoMpeg4(OMX_VIDEO_PARAM_MPEG4TYPE *p_enc_param, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoMpeg4()");
	OstTraceFiltInst4(TRACE_FLOW, "bSVH= %d, eProfile= %d, eLevel= %d, nTimeIncRes= %d", p_enc_param->bSVH, p_enc_param->eProfile, p_enc_param->eLevel, p_enc_param->nTimeIncRes);
	OstTraceFiltInst3(TRACE_FLOW, "nPFrames = %d, nBFrames = %d, nMaxPacketSize = %d", p_enc_param->nPFrames, p_enc_param->nBFrames, p_enc_param->nMaxPacketSize);

	if((m_enc_param.eProfile != p_enc_param->eProfile) || (m_enc_param.eLevel != p_enc_param->eLevel))
	{
		setProfileLevel(p_enc_param->eProfile, p_enc_param->eLevel);
	}
	
	//nMaxPacketSize (in bytes) should be less than 1024, because FW puts a limit
	// of 8192 bits on the video packet size.
	if(p_enc_param->nMaxPacketSize > 1024)
	{
		OstTraceInt0(TRACE_ERROR, "setIndexParamVideoMpeg4(): ERROR: (nMaxPacketSize > 1024) Returning OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}
	*p_has_changed = OMX_TRUE;
	m_enc_param = *p_enc_param;

	//< FIXME err = check_profile_level(FRAMERATE_TYPE, (OMX_PTR)p_bitrate);

	
	m_err_corr.bEnableRVLC = m_enc_param.bReversibleVLC;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoMpeg4()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoMpeg4(OMX_VIDEO_PARAM_MPEG4TYPE *p_enc_param) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoMpeg4()");

	*p_enc_param = m_enc_param;

	//OstTraceFunctionExitExt(MPEG4ENC_PNC_GETMPEG4_EXIT, this, OMX_ErrorNone);	// OST Trace
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoMpeg4()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamVideoH263(OMX_VIDEO_PARAM_H263TYPE *p_enc_h263_param, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamVideoH263()");
	OstTraceFiltInst4(TRACE_FLOW, "p_enc_h263_param->eProfile = %d, p_enc_h263_param->eLevel = %d, p_enc_h263_param->nPFrames = %d, p_enc_h263_param->nGOBHeaderInterval = %d", p_enc_h263_param->eProfile, p_enc_h263_param->eLevel, p_enc_h263_param->nPFrames, p_enc_h263_param->nGOBHeaderInterval);
	OstTraceFiltInst4(TRACE_FLOW, "p_enc_h263_param->bForceRoundingTypeToZero = %d, p_enc_h263_param->bPLUSPTYPEAllowed = %d, p_enc_h263_param->nAllowedPictureTypes = %d, p_enc_h263_param->nPictureHeaderRepetition = %d", p_enc_h263_param->bForceRoundingTypeToZero, p_enc_h263_param->bPLUSPTYPEAllowed, p_enc_h263_param->nAllowedPictureTypes, p_enc_h263_param->nPictureHeaderRepetition);
	
	*p_has_changed = OMX_TRUE;
	m_enc_h263_param = *p_enc_h263_param;

	//OstTraceFunctionExitExt(MPEG4ENC_PNC_SETH263_EXIT, this, OMX_ErrorNone);	// OST Trace
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamVideoH263()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamVideoH263(OMX_VIDEO_PARAM_H263TYPE *p_enc_h263_param) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamVideoH263()");

	*p_enc_h263_param = m_enc_h263_param;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamVideoH263()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamPixelAspectRatio(OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio, OMX_BOOL *has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamPixelAspectRatio()");
	OstTraceFiltInst2(TRACE_FLOW, "p_pixel_aspect_ratio->nHorizontal = %d, p_pixel_aspect_ratio->nVertical = %d", p_pixel_aspect_ratio->nHorizontal, p_pixel_aspect_ratio->nVertical);

	*has_changed = OMX_TRUE;
	
	m_pixel_aspect_ratio = *p_pixel_aspect_ratio;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamPixelAspectRatio()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamPixelAspectRatio(OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamPixelAspectRatio()");

	 *p_pixel_aspect_ratio = m_pixel_aspect_ratio;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamPixelAspectRatio()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamColorPrimary(OMX_PARAM_COLORPRIMARY *p_color_primary, OMX_BOOL *has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamColorPrimary()");
	OstTraceFiltInst1(TRACE_FLOW, "p_color_primary->eColorPrimary = %d", p_color_primary->eColorPrimary);

	*has_changed = OMX_TRUE;
	
	if(m_enc_param.bSVH == OMX_TRUE && p_color_primary->eColorPrimary != (OMX_SYMBIAN_COLORPRIMARYTYPE)OMX_ColorPrimaryBT601LimitedRange) 
	{
		OstTraceInt1(TRACE_ERROR, "setIndexParamColorPrimary(): OMX Error OMX_ErrorUnsupportedSetting at line no. %d", __LINE__);
		return OMX_ErrorUnsupportedSetting;
	}

	m_color_primary = (OMX_COLORPRIMARYTYPE)(p_color_primary->eColorPrimary);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamColorPrimary()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamColorPrimary(OMX_PARAM_COLORPRIMARY *p_color_primary) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamColorPrimary()");

	p_color_primary->eColorPrimary = (OMX_SYMBIAN_COLORPRIMARYTYPE)m_color_primary;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamColorPrimary()");

	return OMX_ErrorNone;
}


#ifdef PACKET_VIDEO_SUPPORT
OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getPVCapability(OMX_PTR pt_org)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getPVCapability()");

    PV_OMXComponentCapabilityFlagsType *pt = (PV_OMXComponentCapabilityFlagsType *)pt_org;
    if (pt == NULL) {
        return OMX_ErrorBadParameter;
    }
    
    // iIsOMXComponentMultiThreaded=OMX_TRUE as the OMX component can run in a different thread
    // from the thread in which PV framework is running
    pt->iIsOMXComponentMultiThreaded = OMX_TRUE;
    
    // iOMXComponentSupportsExternalOutputBufferAlloc=OMX_FALSE as we mandates to allocate the output
    // buffers as we require HW buffers internally
    pt->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_FALSE;
    
    // iOMXComponentSupportsExternalInputBufferAlloc=OMX_TRUE as the buffer can provide from
    // the camera component. Note: buffers can't come from an other source as camera.
    pt->iOMXComponentSupportsExternalInputBufferAlloc = OMX_TRUE;
    
    // iOMXComponentSupportsMovableInputBuffers=OMX_TRUE
    // this is mandatory for the optimized data path selection at opencore level.
    // if we get this flag as false a buffer copy will be done from camera output to encoder input
    pt->iOMXComponentSupportsMovableInputBuffers = OMX_TRUE;
    
    // iOMXComponentSupportsPartialFrames=OMX_FALSE as input buffers should contain complete frames
    pt->iOMXComponentSupportsPartialFrames = OMX_FALSE;
    
    // iOMXComponentUsesNALStartCode=OMX_FALSE
    pt->iOMXComponentUsesNALStartCode = OMX_FALSE;
    
    // iOMXComponentCanHandleIncompleteFrames=OMX_FALSE. The input frames have to be complete YUV frames.
    pt->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;
    
    // iOMXComponentUsesFullAVCFrames=OMX_TRUE as we only supports frame mode, and not nal mode
    pt->iOMXComponentUsesFullAVCFrames = OMX_TRUE;
    
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getPVCapability()");

    return OMX_ErrorNone;
}
#endif

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexConfigVideoBitrate(OMX_VIDEO_CONFIG_BITRATETYPE *p_dyn_bitrate, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexConfigVideoBitrate()");
	OstTraceFiltInst1(TRACE_FLOW, "nEncodeBitrate = %d", p_dyn_bitrate->nEncodeBitrate);

	OMX_ERRORTYPE err = OMX_ErrorNone;

	*p_has_changed = OMX_FALSE;

#ifdef PROFILE_LEVEL_STRICT_CHECKING
	err = check_profile_level(DYN_BITRATE_TYPE, (OMX_PTR)p_dyn_bitrate);
#endif

	if(err == OMX_ErrorNone)
	{
		if(m_bitrate.nTargetBitrate != p_dyn_bitrate->nEncodeBitrate)
		{
			m_bitrate.nTargetBitrate = p_dyn_bitrate->nEncodeBitrate;
			*p_has_changed = OMX_TRUE;
		}
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexConfigVideoBitrate()");

	return err;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexConfigVideoBitrate(OMX_VIDEO_CONFIG_BITRATETYPE *p_dyn_bitrate) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexConfigVideoBitrate()");

	p_dyn_bitrate->nEncodeBitrate = m_bitrate.nTargetBitrate;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexConfigVideoBitrate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexConfigVideoFramerate(OMX_CONFIG_FRAMERATETYPE *p_framerate, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexConfigVideoFramerate()");
	OstTraceFiltInst1(TRACE_FLOW, "p_framerate->xEncodeFramerate = %d", p_framerate->xEncodeFramerate);

	OMX_ERRORTYPE err = OMX_ErrorNone;

	//OstTraceFunctionEntryExt(MPEG4ENC_PNC_SETFRAMERATE_ENTRY, this);	// OST Trace

	*p_has_changed = OMX_FALSE;

#ifdef PROFILE_LEVEL_STRICT_CHECKING
	err = check_profile_level(FRAMERATE_TYPE, (OMX_PTR)p_framerate);
#endif

	if(err == OMX_ErrorNone)
	{
		if(m_framerate.xEncodeFramerate != p_framerate->xEncodeFramerate)
		{
			m_framerate = *p_framerate;
			*p_has_changed = OMX_TRUE;
		}
	}
	else
	{
		OstTraceInt2(TRACE_DEBUG, "MPEG4Enc_ParamAndConfig: Error : 0x%x for check_profile_level at line no %d \n",err,__LINE__);
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexConfigVideoFramerate()");

	return err;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexConfigVideoFramerate(OMX_CONFIG_FRAMERATETYPE *p_framerate) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexConfigVideoFramerate()");

	*p_framerate = m_framerate;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexConfigVideoFramerate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexConfigVideoIntraVOPRefresh(OMX_CONFIG_INTRAREFRESHVOPTYPE *p_force_intra, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexConfigVideoIntraVOPRefresh()");
	OstTraceFiltInst1(TRACE_FLOW, "IntraRefreshVOP = %d", p_force_intra->IntraRefreshVOP);
	
	*p_has_changed = OMX_TRUE;
	m_force_intra = *p_force_intra;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexConfigVideoIntraVOPRefresh()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexConfigVideoIntraVOPRefresh(OMX_CONFIG_INTRAREFRESHVOPTYPE *p_force_intra) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexConfigVideoIntraVOPRefresh()");

	*p_force_intra = m_force_intra;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexConfigVideoIntraVOPRefresh()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamCommonDeblocking(OMX_PARAM_DEBLOCKINGTYPE *p_deblocking, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamCommonDeblocking()");
	OstTraceFiltInst1(TRACE_FLOW, "bDeblocking = %d", p_deblocking->bDeblocking);
	*p_has_changed = OMX_TRUE;
	m_deblocking = *p_deblocking;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamCommonDeblocking()");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamCommonDeblocking(OMX_PARAM_DEBLOCKINGTYPE *p_deblocking) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamCommonDeblocking()");
	p_deblocking->bDeblocking = m_deblocking.bDeblocking;
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamCommonDeblocking()");

	return OMX_ErrorNone;
}

/* +Change start for CR343589 Rotation */
OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexConfigCommonRotate(OMX_CONFIG_ROTATIONTYPE *p_rotation, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexConfigCommonRotate()");

	m_rotation = *p_rotation;

	*p_has_changed = OMX_TRUE;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexConfigCommonRotate()");

	return OMX_ErrorNone;

}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexConfigCommonRotate(OMX_CONFIG_ROTATIONTYPE *p_rotation) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexConfigCommonRotate()");

	p_rotation->nRotation = m_rotation.nRotation;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexConfigCommonRotate()");

	return OMX_ErrorNone;
}
/* -Change end for CR343589 Rotation */

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexConfigVideoMacroBlockErrorMap(OMX_CONFIG_MACROBLOCKERRORMAPTYPE *p_mb_errormap, OMX_BOOL *p_has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexConfigVideoMacroBlockErrorMap()");

	*p_has_changed = OMX_FALSE;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexConfigVideoMacroBlockErrorMap() Returning OMX_ErrorNotImplemented");

	return OMX_ErrorNotImplemented;

}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexConfigVideoMacroBlockErrorMap(OMX_CONFIG_MACROBLOCKERRORMAPTYPE *p_mb_errormap) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexConfigVideoMacroBlockErrorMap()");

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexConfigVideoMacroBlockErrorMap() Returning OMX_ErrorNotImplemented");
	
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamPortDefinition(OMX_PTR pt_org) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamPortDefinition()");

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamPortDefinition() Returning OMX_ErrorNotImplemented");

	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamResourceSvaMcps(OMX_PTR pPtr)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamResourceSvaMcps()");
	
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    OMX_U32 mcps = pMcpsType->nSvaMips;
    if (mcps > 200) {
        return OMX_ErrorBadParameter;
    }
    resourceSvaMcps(mcps);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamResourceSvaMcps()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamResourceSvaMcps(OMX_PTR pPtr) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamResourceSvaMcps()");

    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    pMcpsType->nSvaMips = resourceSvaMcps();

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamResourceSvaMcps()");

    return OMX_ErrorNone;
}

/* +Change start for CR338066 MPEG4Enc DCI */
OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::setIndexParamMpeg4ShortDCI(VFM_PARAM_MPEG4SHORTDCITYPE *p_short_dci, OMX_BOOL *p_has_changed)
{
    OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::setIndexParamMpeg4ShortDCI()");
    OstTraceFiltInst1(TRACE_API, "bEnableShortDCI = %d", p_short_dci->bEnableShortDCI);

    m_short_dci = *p_short_dci;

    OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::setIndexParamMpeg4ShortDCI()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::getIndexParamMpeg4ShortDCI(VFM_PARAM_MPEG4SHORTDCITYPE *p_short_dci) const
{
    OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ParamAndConfig::getIndexParamMpeg4ShortDCI()");

    p_short_dci->bEnableShortDCI  = m_short_dci.bEnableShortDCI;

    OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ParamAndConfig::getIndexParamMpeg4ShortDCI()");

    return OMX_ErrorNone;
}
/* -Change end for CR338066 MPEG4Enc DCI */

OMX_U16 to_index(OMX_U32 value)
{
	OMX_U16 ret_val = 0;

	if(value  == OMX_VIDEO_MPEG4LevelMax)
		return 8;
	do
	{
		value = value>>1;
		ret_val++;
	}
	while(value);

	return ret_val-1;
}

OMX_ERRORTYPE MPEG4Enc_ParamAndConfig::check_profile_level(t_ptr_type ptr_type, OMX_PTR ptr)
{
	OMX_CONFIG_FRAMERATETYPE *p_framerate=0;
	OMX_VIDEO_PARAM_BITRATETYPE *p_bitrate=0;
	OMX_VIDEO_CONFIG_BITRATETYPE *p_dyn_bitrate=0;

	MPEG4Enc_Port *port;
	OMX_U32 vcv_decoder_rate;

	OstTraceInt1(TRACE_FLOW, "MPEG4Enc_ParamAndConfig: Inside check_profile_level %d\n",__LINE__);

	if(ptr_type == BITRATE_TYPE)
		p_bitrate = (OMX_VIDEO_PARAM_BITRATETYPE *)ptr;
	else if(ptr_type == FRAMERATE_TYPE)
		p_framerate = (OMX_CONFIG_FRAMERATETYPE *)ptr;
	else if(ptr_type == DYN_BITRATE_TYPE)
		p_dyn_bitrate = (OMX_VIDEO_CONFIG_BITRATETYPE *)ptr;

	if(m_enc_param.bSVH == OMX_FALSE)
	{
		if(m_enc_param.eProfile == OMX_VIDEO_MPEG4ProfileSimple)
		{
			switch(ptr_type)
			{
				case BITRATE_TYPE:
					RETURN_XXX_IF_WRONG_ELSE_BREAK((p_bitrate->nTargetBitrate <= sp_bitrate_level_values[to_index(m_enc_param.eLevel)]), OMX_ErrorBadParameter);
				case FRAMERATE_TYPE:
				{
					port = (MPEG4Enc_Port*)(getComponent()->getPort(1));

					//>RETURN_XXX_IF_WRONG_ELSE_BREAK(((p_framerate->xEncodeFramerate)>>16 <= sp_framerate_level_values[to_index(m_enc_param.eLevel)]), OMX_ErrorBadParameter);
					
					//VCV decoder rate is in Macroblocks per second (MB/s); it has to be checked against the level constraints as 
					// specified in the MPEG4 standard.					
					vcv_decoder_rate = ((port->getFrameWidth() * port->getFrameHeight()) / 256 ) * ((p_framerate->xEncodeFramerate)>>16);
					
					// Here we check that both the vcv_decoder_rate and frame_rate satisfy the level constraints:
					RETURN_XXX_IF_WRONG_ELSE_BREAK(((vcv_decoder_rate <= sp_mbpersec_level_values[to_index(m_enc_param.eLevel)]) && ((p_framerate->xEncodeFramerate)>>16 <= sp_framerate_level_values[to_index(m_enc_param.eLevel)])) , OMX_ErrorBadParameter);
				}
				case DYN_BITRATE_TYPE:
					RETURN_XXX_IF_WRONG_ELSE_BREAK((p_dyn_bitrate->nEncodeBitrate <= sp_bitrate_level_values[to_index(m_enc_param.eLevel)]), OMX_ErrorBadParameter);
			}
		}
		else
		{
			OstTraceInt1(TRACE_ERROR, "MPEG4Enc_ParamAndConfig: Returning OMX_ErrorBadParameter (check_profile_level) at line no %d \n",__LINE__);
			return OMX_ErrorBadParameter;
		}
	}
	else
	{
		if((m_enc_h263_param.eProfile == OMX_VIDEO_H263ProfileBaseline) || (m_enc_h263_param.eProfile == OMX_VIDEO_H263ProfileISWV2))
		{
			switch(ptr_type)
			{
				case BITRATE_TYPE:
					RETURN_XXX_IF_WRONG_ELSE_BREAK((p_bitrate->nTargetBitrate <= sh_bitrate_level_values[to_index(m_enc_h263_param.eLevel)]), OMX_ErrorBadParameter);
				case FRAMERATE_TYPE:
					RETURN_XXX_IF_WRONG_ELSE_BREAK(((p_framerate->xEncodeFramerate)>>16 <= sh_framerate_level_values[to_index(m_enc_h263_param.eLevel)]), OMX_ErrorBadParameter);
				case DYN_BITRATE_TYPE:
					RETURN_XXX_IF_WRONG_ELSE_BREAK((p_dyn_bitrate->nEncodeBitrate <= sh_bitrate_level_values[to_index(m_enc_h263_param.eLevel)]), OMX_ErrorBadParameter);
			}
		}
		else
		{
			OstTraceInt1(TRACE_ERROR, "MPEG4Enc_ParamAndConfig: Returning OMX_ErrorBadParameter (check_profile_level) at line no %d \n",__LINE__);
			return OMX_ErrorBadParameter;
		}
	}

	//OstTraceFunctionExitExt(MPEG4ENC_PNC_CHECKPROFILELEVEL_EXIT, this, OMX_ErrorNone);	// OST Trace
	return OMX_ErrorNone;
}
