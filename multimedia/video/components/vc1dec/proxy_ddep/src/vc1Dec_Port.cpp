/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Contains the class of vc1dec port (part of the OMX component vc1dec)
 * \author  ST-Ericsson
 */


#define _CNAME_ vc1Dec_Proxy
#include "osi_trace.h"

#include "vc1Dec_Proxy.h"
#include "vc1Dec_Port.h"
#include "VFM_Component.h"
#include "VFM_Index.h"
#include "VFM_Utility.h"
#include "ENS_Client.h"
#include "VFM_Memory.h"
#include "VFM_DDepUtility.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_proxy_ddep_src_vc1Dec_PortTraces.h"
#endif

vc1Dec_Port::vc1Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp) :
            VFM_Port(commonPortData, enscomp)
{
    #ifdef PACKET_VIDEO_SUPPORT
 mSuggestedBufferSize = 0;
#endif
}


// vc1Dec_Port::checkSetFormatInPortDefinition() is automatically called by the VFM when setting
// the parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE vc1Dec_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
    IN0("");
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Port: checkSetFormatInPortDefinition()\n");
    OMX_U32 maxSupportedWidth, maxSupportedHeight;
    DBC_ASSERT(mParamPortDefinition.nPortIndex==portDef->nPortIndex);
    switch (portDef->nPortIndex) {
    case 0:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirInput, OMX_ErrorBadParameter);
        break;
    case 1:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirOutput, OMX_ErrorBadParameter);
        break;
    default:
        OUT0("");
        OstTraceInt0(TRACE_ERROR,  "VC1Dec: vc1Dec_Port::checkSetFormatInPortDefinition() -> OMX_ErrorBadParameter\n");
        return OMX_ErrorBadParameter;
    }
    RETURN_XXX_IF_WRONG(portDef->eDomain==OMX_PortDomainVideo, OMX_ErrorBadParameter);
    // no check on nBufferCountActual, nBufferCountMin, nBufferSize, bEnabled, bPopulated, bBuffersContiguous, nBufferAlignment

    OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portDef->format.video));
    // pt_video->cMIMEType: no check
    RETURN_XXX_IF_WRONG(pt_video->pNativeRender == 0, OMX_ErrorBadParameter);

    /* + Changes for CR 334359 */
	if(VFM_SocCapabilityMgt::isSoc1080pCapable() == OMX_TRUE)
	{
		maxSupportedWidth = 1920;
		maxSupportedHeight = 1088;
		OstTraceInt0(TRACE_FLOW, "vc1Dec_Port::checkSetFormatInPortDefinition - isSoc1080pCapable returned TRUE");
	}
	else
	{
		maxSupportedWidth = 1280;
		maxSupportedHeight = 720;
		OstTraceInt0(TRACE_FLOW, "vc1Dec_Port::checkSetFormatInPortDefinition - isSoc1080pCapable returned FALSE");
	}

	OstTraceInt2(TRACE_FLOW, "vc1Dec_Port::max supported Width %d Height %d",maxSupportedWidth,maxSupportedHeight);
	if (portDef->nPortIndex == 0 && !getProxy()->mParam.CropWidth && !getProxy()->mParam.CropHeight)
    {
	    getProxy()->mParam.CropWidth = pt_video->nFrameWidth;
		getProxy()->mParam.CropHeight = pt_video->nFrameHeight;
		OstTraceFiltInst2(TRACE_FLOW, "H264Dec_Port::Actual Width %d and Actual Height %d",getProxy()->mParam.CropWidth,getProxy()->mParam.CropHeight);
    }
	/* making width and height as multiple of 16 */
    pt_video->nFrameHeight = (pt_video->nFrameHeight + 0xF)&(~0xF);
    pt_video->nFrameWidth  = (pt_video->nFrameWidth + 0xF)&(~0xF);

    OstTraceFiltInst2(TRACE_FLOW, "Vc1Dec_Port::Set New Width %d and Actual Height %d",pt_video->nFrameWidth,pt_video->nFrameHeight);
    /* + Changes for ER346056*/
    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)>=(16*16), OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)<=(maxSupportedWidth*maxSupportedHeight), OMX_ErrorBadParameter);
    /* - Changes for ER346056*/
    /* - Changes for CR 334359 */

    // FIXME: nothing on pt_video->nStride
    // FIXME: nothing on pt_video->nSliceHeight
    // FIXME: nothing on pt_video->nBitRate
    // FIXME: nothing on pt_video->xFramerate
    // FIXME: nothing on pt_video->bFlagErrorConcealment
    RETURN_XXX_IF_WRONG(pt_video->pNativeWindow == 0, OMX_ErrorBadParameter);
    if (portDef->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG( (pt_video->eCompressionFormat==(OMX_VIDEO_CODINGTYPE)OMX_SYMBIAN_VIDEO_CodingVC1)
                // +CR334366 CHANGE START FOR
                // +VC1 codec validation
                || (pt_video->eCompressionFormat==(OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingWMV)
                // -VC1 codec validation
                // -CR334366 CHANGE END OF
                || (pt_video->eCompressionFormat==9), // VC1 previous value
                OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt_video->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt_video->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(
                pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar
                || pt_video->eColorFormat==0x7FFFFFFE // YUV420MB previous value
                || pt_video->eColorFormat==OMX_COLOR_FormatYUV420Planar,
                OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }
    #ifdef PACKET_VIDEO_SUPPORT
   if(portDef->nBufferSize > getBufferSize()) {
        mSuggestedBufferSize = portDef->nBufferSize;
    }
#endif
    return OMX_ErrorNone;
}

// vc1Dec_Port::checkIndexParamVideoPortFormat() is automatically called by the VFM when setting
// the video parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE vc1Dec_Port::checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt)
{
    IN0("");
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Port: checkIndexParamVideoPortFormat()\n");
    DBC_ASSERT(mParamPortDefinition.nPortIndex==pt->nPortIndex);
    DBC_ASSERT(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1);

   if (pt->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==(OMX_VIDEO_CODINGTYPE)OMX_SYMBIAN_VIDEO_CodingVC1
        // +CR334366 CHANGE START FOR
        // +VC1 codec validation
        || (pt->eCompressionFormat==(OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingWMV)
        // -VC1 codec validation
        // -CR334366 CHANGE END OF
        || (pt->eCompressionFormat==(OMX_VIDEO_CODINGTYPE)9), // VC1 previous value
        OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(
                pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar
                || pt->eColorFormat==(OMX_COLOR_FORMATTYPE)0x7FFFFFFE // YUV420MB previous value
                || pt->eColorFormat==OMX_COLOR_FormatYUV420Planar,
                OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }

    OUT0("");
    return OMX_ErrorNone;
}


// Automatically called by the VFM when the parameter of the port are set.
// It returns the minimum sizeof the buffer
OMX_U32 vc1Dec_Port::getBufferSize() const
{
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Port: getBufferSize()\n");
    switch (getDirection()) {
    case OMX_DirInput:
    {
        t_uint32 tmp_width = getFrameWidth();
        t_uint32 tmp_height = getFrameHeight();
        tmp_width = (tmp_width+0xF)&(~0xF);  /* round to a multiple of 16, this is needed for cropping */
        tmp_height = (tmp_height+0xF)&(~0xF);

        // VC1 standard: max size of one row is MAX(6144, nb_horizontal_macroblocks*1536) in bits
        // plus other info (1-MV, 4-MV).
        OMX_U32 size = MAX(6144,((tmp_width/16)*1536)) * (tmp_height/16) / 8;
        size = MIN(size, 100*1024); // max size of internal buffers ( can be reallocated if needed)

        //size = tmp_width*tmp_height*4; // previous setting
#ifdef REQUIRE_EOF
        return size;
#else
#ifdef PACKET_VIDEO_SUPPORT
             if(mSuggestedBufferSize != 0)
       return mSuggestedBufferSize;
        else
        return tmp_width*tmp_height*4; // previous setting
#endif  // PACKET_VIDEO_SUPPORT
        return size;
#endif  // REQUIRE_EOF
    }
    case OMX_DirOutput:
    {
        t_uint16 tmp_width = getFrameWidth();
        t_uint16 tmp_height = getFrameHeight();
        tmp_width = (tmp_width+0xF)&(~0xF);  /* round to a multiple of 16, this is needed for cropping */
        tmp_height = (tmp_height+0xF)&(~0xF);
        return (getSizeFrame(getColorFormat(), tmp_width, tmp_height));
    }
    default:
            DBC_ASSERT(0==1);
    }
    return 0;
}

OMX_U32 vc1Dec_Port::getBufferCountMin() const
{
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Port: getBufferCountMin()\n");
    int buffer_min=0;
    switch (getDirection()) {
    case OMX_DirInput:
        buffer_min = mParamPortDefinition.nBufferCountMin;
        break;
    case OMX_DirOutput:
        // MC: for VC1, DPB size is always 3, then use 4 for display system, +3 for pipe size
        buffer_min = 3+4+3;
         /* +ER 349396 CHANGE START FOR */
        buffer_min  += NB_BUFFERS_IN_DISPLAY;
        buffer_min += getProxy()->getParamAndConfig()->getRecyclingDelay()->get();
         /* +ER 349396 CHANGE START FOR */
    break;
    default:
        DBC_ASSERT(0==1);
    }
    return buffer_min;
}


// Set the default value of the port. This function is used by the construct() function of the proxy
// when creating a new instance of the proxy
void vc1Dec_Port::setDefault()
{
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Port: setDefault()\n");
    DBC_ASSERT(mParamPortDefinition.eDomain==OMX_PortDomainVideo);
    vc1Dec_Proxy *comp = getProxy();

    mParamPortDefinition.format.video.cMIMEType = (char *)"video/vc1";
    mParamPortDefinition.format.video.pNativeRender = 0;
    mParamPortDefinition.format.video.nFrameHeight = 144;      // from OMX spec 1.1.1
    mParamPortDefinition.format.video.nFrameWidth = 176;       // from OMX spec 1.1.1
    mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
    mParamPortDefinition.format.video.pNativeWindow = 0;

    switch (mParamPortDefinition.nPortIndex) {
    case 0:
        DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirInput);
        // +CR334366 CHANGE START FOR
        // mParamPortDefinition.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_SYMBIAN_VIDEO_CodingVC1;
        mParamPortDefinition.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingWMV;
        // -CR334366 CHANGE END OF
        mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
        mParamPortDefinition.format.video.nBitrate = 64000;        // from OMX spec 1.1.1
        mParamPortDefinition.format.video.xFramerate = 15;         // from OMX spec 1.1.1
        mParamPortDefinition.format.video.nStride = 0;             // unused for compressed data
        mParamPortDefinition.format.video.nSliceHeight = 1;        // unused for compressed data
        comp->mParam.setProfileLevel(OMX_VIDEO_VC1ProfileMain, OMX_VIDEO_VC1LevelHigh);  // from OMX spec 1.1.1
        break;
    case 1:
        DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirOutput);
        mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
#ifdef NO_HAMAC
        mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
#else
        mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
#endif
        mParamPortDefinition.format.video.nBitrate = 0;
        mParamPortDefinition.format.video.xFramerate = 0;
        mParamPortDefinition.format.video.nStride = (mParamPortDefinition.format.video.nFrameWidth * 3) / 2;
        mParamPortDefinition.format.video.nSliceHeight = 1;        // a single row in the buffer is required. FIXME
        break;
    default:
        DBC_ASSERT(0==1);
    }

    // mParamPortDefinition.nBufferCountActual updated by the component when buffers are allocated
    // mParamPortDefinition.nBufferCountMin set at the creation of the port (check constructor of the proxy)
    // mParamPortDefinition.bEnabled set by the component
    // mParamPortDefinition.bPopulated set by the component
    mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
    mParamPortDefinition.nBufferAlignment = 0x100;
    mParamPortDefinition.nBufferSize = getBufferSize();
    mParamPortDefinition.nBufferCountMin = getBufferCountMin();
  mParamPortDefinition.nBufferCountActual = mParamPortDefinition.nBufferCountMin; /* +ER 349396 CHANGE START FOR */
}

