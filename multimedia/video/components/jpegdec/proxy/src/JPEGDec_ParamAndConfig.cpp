/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_jpegdec
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_proxy_src_JPEGDec_ParamAndConfigTraces.h"
#endif


#include "JPEGDec_Proxy.h"

#include "VFM_Utility.h"
#include "JPEGDec_ParamAndConfig.h"

#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { jpegdec_param_assert(_error, __LINE__, OMX_FALSE); return (_error); } }


void JPEGDec_ParamAndConfig::jpegdec_param_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : jpegdec_param_assert");
    if (OMX_ErrorNone != omxError)
    {
        OstTraceFiltInst2(TRACE_ERROR, "JPEGDECParam : errorType : 0x%x error line no %d\n", omxError,line);
        NMF_LOG("JPEGDECParam : errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


OMX_U32 JPEGDec_ParamAndConfig::resourceSvaMcps() const
{ return m_nSvaMcps; }

void JPEGDec_ParamAndConfig::resourceSvaMcps(const OMX_U32& mcps)
{ m_nSvaMcps = mcps; }


OMX_ERRORTYPE JPEGDec_ParamAndConfig::setIndexParamResourceSvaMcps(OMX_PTR pPtr){
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : setIndexParamResourceSvaMcps");
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    OMX_U32 mcps = pMcpsType->nSvaMips;
    if (mcps > 200) {
        OstTraceFiltInst0(TRACE_ERROR, "JPEGDECParam : setIndexParamResourceSvaMcps OMX_ErrorBadParameter");
        return OMX_ErrorBadParameter;
    }
    resourceSvaMcps(mcps);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::getIndexParamResourceSvaMcps(OMX_PTR pPtr) const{
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : getIndexParamResourceSvaMcps");
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    pMcpsType->nSvaMips = resourceSvaMcps();
    return OMX_ErrorNone;
}



JPEGDec_ParamAndConfig::JPEGDec_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{

	horizontal_offset = 0;
    vertical_offset = 0;
	frameHeight = 0;
	frameWidth = 0;
	cropWindowHeight = 0;
	cropWindowWidth = 0;
    outputBufferFormat = (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
    downsamplingFactor = DOWNSAMPLING_FACTOR_1;
    isCroppingEnabled =0;
	isCroppingInvalid=0;
	isDownsamplingEnabled = 0;

	m_nSvaMcps = 50;

	configCroppingChanged = OMX_FALSE;
	configScalingChanged = OMX_FALSE;

}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::reset(){
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::setIndexConfigCommonScale(OMX_PTR pt_org,
														JPEGDec_Proxy_Downsampling_Factor *downsamplingFactor,
														OMX_BOOL *has_changed)
{
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : setIndexConfigCommonScale");
	OMX_CONFIG_SCALEFACTORTYPE *pt = (OMX_CONFIG_SCALEFACTORTYPE *)pt_org;
    JPEGDec_Proxy_Downsampling_Factor downsamplingFactorOld = *downsamplingFactor;
	if(pt->xHeight!=pt->xWidth) {// Firmware downsamples equally in both direction.
        OstTraceFiltInst0(TRACE_ERROR, "JPEGDECParam : setIndexConfigCommonScale OMX_ErrorUnsupportedSetting");
		return OMX_ErrorUnsupportedSetting;
	}
	else {
            OstTraceFiltInst1(TRACE_FLOW, "JPEGDECParam : setIndexConfigCommonScale pt->xHeight - %x\n",pt->xHeight);
		switch (pt->xHeight) {
		case 0x10000:		// No downsacaling
			*downsamplingFactor = DOWNSAMPLING_FACTOR_1;
			break;

		case 0x8000:
			*downsamplingFactor = DOWNSAMPLING_FACTOR_2;
			break;

		case 0x4000:
			*downsamplingFactor = DOWNSAMPLING_FACTOR_4;
			break;

		case 0x2000:
			*downsamplingFactor = DOWNSAMPLING_FACTOR_8;
			break;

		default:
			return OMX_ErrorUnsupportedSetting;
		}

	}

    *has_changed = (OMX_BOOL)(downsamplingFactorOld != *downsamplingFactor);

    OstTraceFiltInst2(TRACE_FLOW, "JPEGDECParam : Inside setIndexConfigCommonScale Downsampling factor : %d and has Cahnged : %d \n",*downsamplingFactor,*has_changed);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::getIndexConfigCommonScale(OMX_PTR pt_org,
													JPEGDec_Proxy_Downsampling_Factor downsamplingFactor) const
{

   OstTraceFiltInst0(TRACE_API, "JPEGDECParam : In getIndexConfigCommonScale");
  OMX_CONFIG_SCALEFACTORTYPE *pt = (OMX_CONFIG_SCALEFACTORTYPE *)pt_org;
   OstTraceFiltInst1(TRACE_FLOW, "JPEGDECParam : downsamplingFactor - %x\n",downsamplingFactor);
  switch (downsamplingFactor) {
	  case DOWNSAMPLING_FACTOR_1:
		pt->xHeight = 0x10000;
		pt->xWidth = 0x10000;
		break;

	case DOWNSAMPLING_FACTOR_2:
		pt->xHeight = 0x8000;
		pt->xWidth = 0x8000;
		break;

	case DOWNSAMPLING_FACTOR_4:
		pt->xHeight = 0x4000;
		pt->xWidth = 0x4000;
		break;

	case DOWNSAMPLING_FACTOR_8:
		pt->xHeight = 0x2000;
		pt->xWidth = 0x2000;
		break;

	default:
      OstTraceFiltInst0(TRACE_ERROR, "JPEGDECParam : getIndexConfigCommonScale OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
    }


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::setIndexParamImagePortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : In setIndexParamImagePortFormat");
    *has_changed = OMX_TRUE;
    OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    RETURN_XXX_IF_WRONG_OST(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
    if (pt->nPortIndex==IPB+1){            // Output port
                OstTraceFiltInst0(TRACE_API, "JPEGDECParam : In setIndexParamImagePortFormat output port");
		RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat == OMX_IMAGE_CodingUnused, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG_OST(pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar
							, OMX_ErrorBadParameter);
		outputBufferFormat = pt->eColorFormat;
        // nothing to be check on pt->nIndex
    }
	else{        // input port
            OstTraceFiltInst0(TRACE_API, "JPEGDECParam : In setIndexParamImagePortFormat input port");
		RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat == OMX_IMAGE_CodingJPEG, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG_OST(pt->eColorFormat == OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::getIndexParamImagePortFormat(OMX_PTR pt_org) const
{

	OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
    OstTraceFiltInst0(TRACE_API, "JPEGDECParam : In getIndexParamImagePortFormat ");
    if (pt->nPortIndex==IPB+1){            // output port
        OstTraceFiltInst0(TRACE_FLOW, "JPEGDECParam : In getIndexParamImagePortFormat output port");
        pt->eCompressionFormat = OMX_IMAGE_CodingUnused;
        pt->eColorFormat = outputBufferFormat;
    }
	else{        // input port
            OstTraceFiltInst0(TRACE_FLOW, "JPEGDECParam : In getIndexParamImagePortFormat input port");
		pt->eCompressionFormat = OMX_IMAGE_CodingJPEG;
		pt->eColorFormat = OMX_COLOR_FormatUnused;
        // nothing to be check on pt->nIndex
    }
    pt->nIndex = 0xFF;        // dummy value


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ParamAndConfig::getIndexParamPortDefinition(OMX_PTR pt_org) const{
    return OMX_ErrorNotImplemented;
}

void JPEGDec_ParamAndConfig::initializeParamAndConfig()
{

    horizontal_offset = 0;
    vertical_offset = 0;
	frameHeight = 0;
	frameWidth = 0;
	cropWindowHeight = 0;
	cropWindowWidth = 0;
    outputBufferFormat = (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
    downsamplingFactor = DOWNSAMPLING_FACTOR_1;        // no scaling by default

	isCroppingEnabled =0;
	isCroppingInvalid=0;
	isDownsamplingEnabled = 0;
	configCroppingChanged = OMX_FALSE;
	configScalingChanged = OMX_FALSE;

}

