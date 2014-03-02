/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Contains specific parameter and configuration setting, from the
 *          OMX point of view, of the OMX component vc1dec.
 * \author  ST-Ericsson
 */


#define _CNAME_ vc1Dec_Proxy
#include "osi_trace.h"

#include "vc1Dec_ParamAndConfig.h"
#include "vc1Dec_Proxy.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_proxy_ddep_src_vc1Dec_ParamAndConfigTraces.h"
#endif
vc1Dec_ParamAndConfig::vc1Dec_ParamAndConfig(VFM_Component *pComponent):
    VFM_ParamAndConfig(pComponent),
	CropWidth(0),
CropHeight(0)
{
    IN0("");

    // initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(OMX_VIDEO_VC1ProfileMain, OMX_VIDEO_VC1LevelHigh);

    OUT0("");
}


OMX_ERRORTYPE vc1Dec_ParamAndConfig::getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>vc1Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    OMX_CONFIG_RECTTYPE *pt = (OMX_CONFIG_RECTTYPE *)pt_org;
    pt->nLeft = 0;
    pt->nTop = 0;
    pt->nWidth = CropWidth;
    pt->nHeight = CropHeight;
    OstTraceInt0(TRACE_API, "<=vc1Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    return OMX_ErrorNone;
}

// +CR349396 CHANGE START FOR
OMX_ERRORTYPE vc1Dec_ParamAndConfig::setRecyclingDelay(OMX_PTR pt_org){
    OMX_ERRORTYPE error = setIndexParamRecyclingDelay(pt_org);
    if (error!=OMX_ErrorNone) return error;

    //Set for output the required settings.
    VFM_Port *pPort = (VFM_Port *)(getComponent()->getPort(1));
    pPort->getParamPortDefinition()->nBufferCountMin = pPort->getBufferCountMin();
    pPort->getParamPortDefinition()->nBufferCountActual = pPort->getParamPortDefinition()->nBufferCountMin;
    return OMX_ErrorNone;
}
// -CR349396 CHANGE START FOR

OMX_ERRORTYPE vc1Dec_ParamAndConfig::reset()
{
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_ParamAndConfig: reset()\n");
    return OMX_ErrorNone;
}

#ifdef PACKET_VIDEO_SUPPORT
// See document openmax_call_sequences.pdf
// as well as omx_core_integration_guide.pdf
EXPORT_C OMX_ERRORTYPE vc1Dec_ParamAndConfig::getPVCapability(OMX_PTR pt_org)
{
    OstTraceFiltInst0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_ParamAndConfig: getPVCapability()\n");
    PV_OMXComponentCapabilityFlagsType *pt = (PV_OMXComponentCapabilityFlagsType *)pt_org;
    if (pt == NULL) {
        OstTraceInt0(TRACE_ERROR,  "VC1Dec: proxy_ddep : vc1Dec_ParamAndConfig::getPVCapability -> OMX_ErrorBadParameter\n");
        return OMX_ErrorBadParameter;
    }

    // iIsOMXComponentMultiThreaded=OMX_TRUE as the OMX component can run in a different thread
    // from the thread in which PV framework is running
    pt->iIsOMXComponentMultiThreaded = OMX_TRUE;

    // iOMXComponentSupportsExternalOutputBufferAlloc=OMX_TRUE as we can use OMX_UseBuffer
    // on the output port (provided that the buffers are allocated as HW buffers)
    pt->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_TRUE;

    // iOMXComponentSupportsExternalInputBufferAlloc=OMX_FALSE as we mandates to allocate the input
    // buffers as we require HW buffers internally
    pt->iOMXComponentSupportsExternalInputBufferAlloc = OMX_FALSE;

    // iOMXComponentSupportsMovableInputBuffers=OMX_FALSE as OMX buffers are not "movable", when passing an
    // input buffer to the OMX component, the "pBuffer" field in the  OMX_BUFFERHEADERTYPE in a
    // particular OMX buffer header always has to point to the same data buffer
    pt->iOMXComponentSupportsMovableInputBuffers = OMX_FALSE;

    // iOMXComponentSupportsPartialFrames=OMX_TRUE as input buffers may contain partial frames
    // Last buffer of a frame is flagged with OMX_BUFFERFLAG_ENDOFFRAME
#ifdef REQUIRE_EOF
    pt->iOMXComponentSupportsPartialFrames = OMX_TRUE;
#else
    pt->iOMXComponentSupportsPartialFrames = OMX_FALSE;
#endif

    // iOMXComponentUsesNALStartCode=OMX_TRUE as startcode 0x0001 must be contained in the input stream
    pt->iOMXComponentUsesNALStartCode = OMX_FALSE;

    // iOMXComponentCanHandleIncompleteFrames=OMX_TRUE as ERC is set on in the decoder, so we support
    // frames with missing parts
    pt->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;

    // iOMXComponentUsesFullAVCFrames=OMX_TRUE as we only supports frame mode, and not nal mode
    pt->iOMXComponentUsesFullAVCFrames = OMX_TRUE;

    return OMX_ErrorNone;
}
#endif

