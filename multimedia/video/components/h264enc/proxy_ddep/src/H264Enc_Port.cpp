/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ H264Enc_Port
#include "osi_trace.h"

#include "H264Enc_Proxy.h"
#include "H264Enc_Port.h"
#include "VFM_Component.h"
#include "VFM_Index.h"
#include "VFM_Utility.h"
#include "ENS_Client.h"
#include "VFM_Memory.h"
#include "VFM_DDepUtility.h"

#ifdef OMX_COLOR_FormatYUV420MBPackedSemiPlanar
#undef OMX_COLOR_FormatYUV420MBPackedSemiPlanar
#endif

#define OMX_COLOR_FormatYUV420MBPackedSemiPlanar 0x7FFFFFFE

//Trace
#ifndef H264ENC_TRACE_GROUP_PROXY
    #include "OstTraceDefinitions_ste.h"
    #include "OpenSystemTrace_ste.h"
    #ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_proxy_ddep_src_H264Enc_PortTraces.h"
    #endif
#endif

H264Enc_Port::H264Enc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp) :
    VFM_Port(commonPortData, enscomp)
{

}


// H264Enc_Port::checkSetFormatInPortDefinition() is automatically called by the VFM when setting
// the parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE H264Enc_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Port::checkSetFormatInPortDefinition");
    /* + Changes for CR 334359 */
    OMX_U32 maxSupportedWidth, maxSupportedHeight;
    /* - Changes for CR 334359 */
    DBC_ASSERT(mParamPortDefinition.nPortIndex==portDef->nPortIndex);
    switch (portDef->nPortIndex) {
    case 0:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirInput, OMX_ErrorBadParameter);
        break;
    case 1:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirOutput, OMX_ErrorBadParameter);
        break;
    default:
        OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Port::checkSetFormatInPortDefinition");
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
    }
    else
    {
    	maxSupportedWidth = 1280;
    	maxSupportedHeight = 720;
	}

    /* condition added for CR 332873 , new proposal */
    if (getProxy()->isSpecialCTCase)
    {
		/* + change for ER 346056 */
		maxSupportedWidth = 1280;
    	maxSupportedHeight = 720;
    	//RETURN_XXX_IF_WRONG(1 <= pt_video->nFrameHeight && pt_video->nFrameHeight <= 720, OMX_ErrorBadParameter);
    	/* - change for ER 346056 */
	}
	/* + change for ER 346056 */
	//else
	//{
	//	RETURN_XXX_IF_WRONG(1 <= pt_video->nFrameHeight && pt_video->nFrameHeight <= maxSupportedHeight, OMX_ErrorBadParameter);
	//}

    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)>=(16*16), OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)<=(maxSupportedWidth*maxSupportedHeight), OMX_ErrorBadParameter);
    /* - change for ER 346056 */
    /* - Changes for CR 334359 */
    // FIXME: nothing on pt_video->nStride
    // FIXME: nothing on pt_video->nSliceHeight
    // FIXME: nothing on pt_video->nBitRate
    // FIXME: nothing on pt_video->xFramerate
    // FIXME: nothing on pt_video->bFlagErrorConcealment

    RETURN_XXX_IF_WRONG(pt_video->pNativeWindow == 0, OMX_ErrorBadParameter);
    if (portDef->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG(pt_video->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG( (pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar ||
                              pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar ||
 pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque ) , OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt_video->eCompressionFormat==OMX_VIDEO_CodingAVC, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt_video->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
		//RETURN_XXX_IF_WRONG(pt_video->xFramerate >= (1<<16), OMX_ErrorBadParameter); removing the check as we can accept 0 frame rate value too
        // nothing to be check on pt->nIndex
    }

    // update codec parameters with new port definition
    getProxy()->getParamAndConfig()->updateParams(portDef->eDir,*pt_video);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Port::checkSetFormatInPortDefinition");
    return OMX_ErrorNone;
}

// H264Enc_Port::checkIndexParamVideoPortFormat() is automatically called by the VFM when setting
// the video parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE H264Enc_Port::checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Port::checkSetFormatInPortDefinition");
    DBC_ASSERT(mParamPortDefinition.nPortIndex==pt->nPortIndex);
    DBC_ASSERT(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1);

   if (pt->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG( (pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar ||
                              pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar ||
pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque), OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
   } else  {        // output port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==OMX_VIDEO_CodingAVC, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Port::checkSetFormatInPortDefinition");
    return OMX_ErrorNone;
}


// Automatically called by the VFM when the parameter of the port are set.
// It returns the minimum size of the buffer
OMX_U32 H264Enc_Port::getBufferSize() const
{
    OMX_U32 min_buffer_size = 0;

    switch (getDirection()) {
    case OMX_DirInput:
        if (VFM_SocCapabilityMgt::getOutputBufferCount())
        {
            min_buffer_size = sizeof(video_metadata_t);
        }
        else
        {
            min_buffer_size = getSizeFrame(getColorFormat(), getFrameWidth(), getFrameHeight());
        }
        OstTraceFiltInst1(H264ENC_TRACE_GROUP_PROXY, "[PROXY] H264Enc_Port::Value of min_buffer_size at input port : %d",min_buffer_size);
        break;
    case OMX_DirOutput:
        {
            t_frameinfo info = getProxy()->getParamAndConfig()->getParams();

            if (info.specific_frameinfo.BrcType == 0) // constant Qp
            {
                min_buffer_size = getFrameWidth() * getFrameHeight() * 2;
            }
            else
            {
                // todo: possible to be more accurate as some profile/level requires less than that
                // warning: OK if using CBR/VBR - possible issue with constant Qp
                min_buffer_size = (getFrameWidth() * getFrameHeight() * 3) / 4; // max buffer size if no NAL header (SPS/PPS/SEI)
                min_buffer_size += 1000; // max NAL header size (SPS+PPS+SEI) + security margin
            }
        }
        break;
    default:
        DBC_ASSERT(0==1);
    }

    return min_buffer_size;
}


OMX_U32 H264Enc_Port::getBufferCountMin() const
{
    switch (getDirection()) {
    case OMX_DirInput:
        return 1;
    case OMX_DirOutput:
        return 2;
    default:
        DBC_ASSERT(0==1);
    }
    return 0;
}


// Set the default value of the port. This function is used by the construct() function of the proxy
// when creating a new instance of the proxy
// FIXME: some parameters are missing => refer to H264Dec_Port
void H264Enc_Port::setDefault()
{
    DBC_ASSERT(mParamPortDefinition.eDomain==OMX_PortDomainVideo);

    switch (mParamPortDefinition.nPortIndex) {
    case 0:
        DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirInput);

        // from OMX spec 1.1.2 (p. 366)
        mParamPortDefinition.format.video.nFrameWidth           = 176;
        mParamPortDefinition.format.video.nFrameHeight          = 144;
        mParamPortDefinition.format.video.eCompressionFormat    = OMX_VIDEO_CodingUnused;
        /* +Change for 369621 */
        mParamPortDefinition.format.video.xFramerate            = 15<<16;
        /* -Change for 369621 */
        mParamPortDefinition.format.video.eColorFormat          = static_cast<OMX_COLOR_FORMATTYPE>(OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar);
        mParamPortDefinition.format.video.nSliceHeight = 1;
        mParamPortDefinition.nBufferCountActual = 2;
        break;
    case 1:
        DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirOutput);

        // from OMX spec 1.1.2 (p. 367)
        mParamPortDefinition.format.video.nFrameWidth           = 176;
        mParamPortDefinition.format.video.nFrameHeight          = 144;
        mParamPortDefinition.format.video.nBitrate              = 64000;
        mParamPortDefinition.format.video.xFramerate            = 15<<16;
        mParamPortDefinition.format.video.eCompressionFormat    = OMX_VIDEO_CodingAVC;
        mParamPortDefinition.format.video.eColorFormat          = OMX_COLOR_FormatUnused;
        mParamPortDefinition.format.video.nSliceHeight = 1;
        /* +Change for 369344 */
        mParamPortDefinition.nBufferCountActual = 5;
        {
            OMX_U32 tempvalue=0;
            tempvalue = VFM_SocCapabilityMgt::getOutputBufferCount();
            OstTraceFiltInst1(TRACE_FLOW, "H264Enc_Port::Output buffer count from environment : %d",tempvalue);
            if (tempvalue)
            {
                 mParamPortDefinition.nBufferCountActual = tempvalue;
            }
            OstTraceFiltInst1(TRACE_FLOW, "H264Enc_Port::value of output buffer count actual %d",mParamPortDefinition.nBufferCountActual);
        }
        /* -Change for 369344 */

        break;

    default:
        DBC_ASSERT(0==1);
    }

    mParamPortDefinition.format.video.cMIMEType = (char *)"video/H264";
    mParamPortDefinition.format.video.pNativeRender = NULL;
    mParamPortDefinition.format.video.pNativeWindow = NULL;

    mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
    mParamPortDefinition.nBufferAlignment = 0x100;
    mParamPortDefinition.nBufferSize = getBufferSize();
    mParamPortDefinition.nBufferCountMin = getBufferCountMin();
}



OMX_ERRORTYPE H264Enc_Port::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Port::setParameter");

    switch (nParamIndex) {
        case OMX_IndexParamVideoAvc:
        case OMX_IndexParamVideoQuantization:
        case OMX_IndexParamVideoSliceFMO:
        case OMX_IndexParamVideoFastUpdate:
        case OMX_IndexParamVideoIntraRefresh:
        case OMX_IndexParamVideoBitrate:
        case OMX_IndexParamVideoVBSMC:
        case OMX_IndexParamVideoMotionVector:
            return getProxy()->setParameter(nParamIndex,pt);
        default:
            // SHAI extension
            if ( nParamIndex == getProxy()->mIndexParamPixelAspectRatio ||
                 nParamIndex == getProxy()->mIndexParamColorPrimary     ||
                 nParamIndex == getProxy()->mIndexParamVideoAvcSei      ||
                 nParamIndex == getProxy()->mIndexParamCpbBufferSize)
            {
                return getProxy()->setParameter(nParamIndex,pt);
            }
            else
            {
                RETURN_OMX_ERROR_IF_ERROR(VFM_Port::setParameter(nParamIndex, pt));
            }
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Port::setParameter");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_Port::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Port::getParameter");

    switch (nParamIndex) {
		case OMX_IndexParamPortDefinition:
		ENS_Port::getParameter(nParamIndex,pt);
        case OMX_IndexParamVideoAvc:
        case OMX_IndexParamVideoQuantization:
        case OMX_IndexParamVideoSliceFMO:
        case OMX_IndexParamVideoFastUpdate:
        case OMX_IndexParamVideoIntraRefresh:
        case OMX_IndexParamVideoBitrate:
        case OMX_IndexParamVideoVBSMC:
        case OMX_IndexParamVideoMotionVector:
            return getProxy()->getParameter(nParamIndex,pt);
        default:
            // SHAI extension
            if ( nParamIndex == getProxy()->mIndexParamPixelAspectRatio ||
                 nParamIndex == getProxy()->mIndexParamColorPrimary     ||
                 nParamIndex == getProxy()->mIndexParamVideoAvcSei      ||
                 nParamIndex == getProxy()->mIndexParamCpbBufferSize)
            {
                return getProxy()->getParameter(nParamIndex,pt);
            }
            else
            {
                RETURN_OMX_ERROR_IF_ERROR(VFM_Port::getParameter(nParamIndex, pt));
            }
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Port::getParameter");
    return OMX_ErrorNone;
}


