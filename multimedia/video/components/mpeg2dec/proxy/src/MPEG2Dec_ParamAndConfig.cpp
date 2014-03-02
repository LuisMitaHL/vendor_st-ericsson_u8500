/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_mpeg2dec
#include "osi_trace.h"


#include "MPEG2Dec_ParamAndConfig.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_proxy_src_MPEG2Dec_ParamAndConfigTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

MPEG2Dec_ParamAndConfig::MPEG2Dec_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig_Constructor");
	CropWidth = 0;
        CropHeight = 0;

	m_nSvaMcps=50;
    setProfileLevel(OMX_VIDEO_MPEG2ProfileSimple, OMX_VIDEO_MPEG2LevelLL);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ParamAndConfig_Constructor");
}

OMX_ERRORTYPE MPEG2Dec_ParamAndConfig::reset(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig::reset");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ParamAndConfig::reset");
	return OMX_ErrorNone;
}

/* +Change for CR 369593 */
OMX_ERRORTYPE MPEG2Dec_ParamAndConfig::getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>MPEG2Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    OMX_CONFIG_RECTTYPE *pt = (OMX_CONFIG_RECTTYPE *)pt_org;
    pt->nLeft = 0;
    pt->nTop = 0;
    pt->nWidth = CropWidth;
    pt->nHeight = CropHeight;
    OstTraceInt0(TRACE_API, "<=MPEG2Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    return OMX_ErrorNone;
}
/* -Change for CR 369593 */

OMX_ERRORTYPE MPEG2Dec_ParamAndConfig::setIndexParamResourceSvaMcps(OMX_PTR pPtr){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig::setIndexParamResourceSvaMcps");
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    OMX_U32 mcps = pMcpsType->nSvaMips;
    if (mcps > 200) {
        return OMX_ErrorBadParameter;
    }
    resourceSvaMcps(mcps);
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig::setIndexParamResourceSvaMcps");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ParamAndConfig::getIndexParamResourceSvaMcps(OMX_PTR pPtr) const{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig::getIndexParamResourceSvaMcps");
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    pMcpsType->nSvaMips = resourceSvaMcps();
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ParamAndConfig::getIndexParamResourceSvaMcps");
    return OMX_ErrorNone;
}


