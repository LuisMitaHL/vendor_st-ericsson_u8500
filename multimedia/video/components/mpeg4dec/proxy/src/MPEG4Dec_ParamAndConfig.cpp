/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_mpeg4dec
#include "osi_trace.h"
#include "MPEG4Dec_ParamAndConfig.h"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_proxy_src_MPEG4Dec_ParamAndConfigTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

MPEG4Dec_ParamAndConfig::MPEG4Dec_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Dec_ParamAndConfig::MPEG4Dec_ParamAndConfig() constructor");

    // initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(0, 0);
    mMpeg4Param.nSize = sizeof(OMX_VIDEO_PARAM_MPEG4TYPE);
	//mMpeg4Param.nVersion = (OMX_VERSIONTYPE)0;
	mMpeg4Param.nPortIndex = 0;;                 
	mMpeg4Param.nSliceHeaderSpacing = 0;        
	mMpeg4Param.bSVH = OMX_FALSE;                      
	mMpeg4Param.bGov =OMX_FALSE;                      
	mMpeg4Param.nPFrames=0;                   
	mMpeg4Param.nBFrames=0;                   
	mMpeg4Param.nIDCVLCThreshold=0;           
	mMpeg4Param.bACPred=OMX_FALSE;                   
	mMpeg4Param.nMaxPacketSize=0;             
	mMpeg4Param.nTimeIncRes=0;                
	mMpeg4Param.eProfile=OMX_VIDEO_MPEG4ProfileSimple;
	mMpeg4Param.eLevel=OMX_VIDEO_MPEG4Level0;    
	mMpeg4Param.nAllowedPictureTypes=0;       
	mMpeg4Param.nHeaderExtension=0;           
	mMpeg4Param.bReversibleVLC=OMX_FALSE;            

	mMpeg4ProfileAndLevel.nSize = sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE);
	//mMpeg4ProfileAndLevel.nVersion = (OMX_VERSIONTYPE)0;
	mMpeg4ProfileAndLevel.nPortIndex = 0;
	mMpeg4ProfileAndLevel.nProfileIndex = 0;
	mMpeg4ProfileAndLevel.eLevel = 0;
	mMpeg4ProfileAndLevel.eProfile = 0;
	CropWidth = 0;
    CropHeight = 0;
	OstTraceInt0(TRACE_API, "<= MPEG4Dec_ParamAndConfig::MPEG4Dec_ParamAndConfig() constructor");
}

OMX_ERRORTYPE MPEG4Dec_ParamAndConfig::reset()
{
	OstTraceInt0(TRACE_API, "<=> MPEG4Dec_ParamAndConfig::reset()");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ParamAndConfig::getIndexParamPortDefinition(OMX_PTR pt_org) const
{
	OstTraceFiltInst0(TRACE_API, "<=> MPEG4Dec_ParamAndConfig::getIndexParamPortDefinition() Returning OMX_ErrorNotImplemented");
	return OMX_ErrorNotImplemented;
}
