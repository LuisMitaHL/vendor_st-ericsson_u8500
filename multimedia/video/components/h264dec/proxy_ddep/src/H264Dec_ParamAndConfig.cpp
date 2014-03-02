/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ H264Dec_Proxy

#include "H264Dec_ParamAndConfig.h"
#include "H264Dec_Proxy.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_proxy_ddep_src_H264Dec_ParamAndConfigTraces.h"
#endif
H264Dec_ParamAndConfig::H264Dec_ParamAndConfig(VFM_Component *pComponent) :
    VFM_ParamAndConfig(pComponent),
    mIsDeblockingOn(OMX_TRUE), mParallelDeblocking(OMX_TRUE),
    mNalFormat(OMX_NaluFormatStartCodes),
// +CR324558 CHANGE START FOR
#if _STE_H264DEC_DPB_CONFIGURATION==1
    mIsDPBSizeStatic(OMX_TRUE),
#else
    mIsDPBSizeStatic(OMX_FALSE),
#endif
    mRestrictMaxlevel(0),   //Signifies Uninitialized
// -CR324558 CHANGE END OF
/* +Change for CR 369593 */
    CropWidth(0),
    CropHeight(0)
/* -Change for CR 369593 */

{
    OstTraceInt0(TRACE_API, "=> H264Dec_ParamAndConfig::H264Dec_ParamAndConfig() constructor");
	// initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1);
#if _STE_H264DEC_DPB_CONFIGURATION==2
//Changes Start for ER435666
	OMX_U32 supportedExtensions = getSupportedExtension();
    setSupportedExtension((supportedExtensions | (1<<4)|(1<<0)));
//Changes End for ER435666
#endif
}


OMX_ERRORTYPE H264Dec_ParamAndConfig::reset()
{
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexConfigMemory(OMX_PTR pt_org) const
{
    VFM_MemoryStatus *pMemoryStatus =
        &((VFM_NmfHost_ProcessingComponent *)(&getComponent()->getProcessingComponent()))->mMemoryStatus;
    VFM_CONFIG_MEMORY *pt = (VFM_CONFIG_MEMORY *)pt_org;
    pt->nMemoryStatus = *pMemoryStatus;

    return OMX_ErrorNone;
}


/* +Change for CR 369593 */
OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    OMX_CONFIG_RECTTYPE *pt = (OMX_CONFIG_RECTTYPE *)pt_org;
    pt->nLeft = 0;
    pt->nTop = 0;
    pt->nWidth = CropWidth;
    pt->nHeight = CropHeight;
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexConfigCommonOutputCrop");
    return OMX_ErrorNone;
}
/* -Change for CR 369593 */

OMX_ERRORTYPE H264Dec_ParamAndConfig::setIndexParamCommonDeblocking(OMX_PTR pt_org)
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::setIndexParamCommonDeblocking");
    OMX_PARAM_DEBLOCKINGTYPE *pt = (OMX_PARAM_DEBLOCKINGTYPE *)pt_org;
    mIsDeblockingOn = pt->bDeblocking;
    OstTraceInt1(TRACE_FLOW, "H264Dec_ParamAndConfig::setIndexParamCommonDeblocking mIsDeblockingOn=%d",pt->bDeblocking);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::setIndexParamCommonDeblocking");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexParamCommonDeblocking(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexParamCommonDeblocking");
    OMX_PARAM_DEBLOCKINGTYPE *pt = (OMX_PARAM_DEBLOCKINGTYPE *)pt_org;
    pt->nPortIndex = 0;
    pt->bDeblocking = mIsDeblockingOn;
    OstTraceInt1(TRACE_FLOW, "H264Dec_ParamAndConfig::setIndexParamCommonDeblocking mIsDeblockingOn=%d",mIsDeblockingOn);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexParamCommonDeblocking");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Dec_ParamAndConfig::setIndexParamParallelDeblocking(OMX_PTR pt_org)
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::setIndexParamParallelDeblocking");
    OMX_PARAM_DEBLOCKINGTYPE *pt = (OMX_PARAM_DEBLOCKINGTYPE *)pt_org;
    mParallelDeblocking = pt->bDeblocking;
    OstTraceInt1(TRACE_FLOW, "H264Dec_ParamAndConfig::setIndexParamParallelDeblocking mParallelDeblocking=%d",mParallelDeblocking);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::setIndexParamParallelDeblocking");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::setIndexParamNalStreamFormat(OMX_PTR pt_org)
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::setIndexParamNalStreamFormat");
    OMX_NALSTREAMFORMATTYPE *pt = (OMX_NALSTREAMFORMATTYPE *)pt_org;
    if (!(pt->eNaluFormat==OMX_NaluFormatStartCodes || pt->eNaluFormat==OMX_NaluFormatFourByteInterleavedLength)) {
        OstTraceInt1(TRACE_ERROR, "H264Dec: H264Dec_ParamAndConfig: setIndexParamNalStreamFormat: Nal Format %u is not supported", pt->eNaluFormat);
        return OMX_ErrorBadParameter;
    }
    mNalFormat = pt->eNaluFormat;
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::setIndexParamNalStreamFormat");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexParamNalStreamFormat(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexParamNalStreamFormat");
    OMX_NALSTREAMFORMATTYPE *pt = (OMX_NALSTREAMFORMATTYPE *)pt_org;
    pt->eNaluFormat = mNalFormat;
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexParamNalStreamFormat");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexParamNalStreamFormatSupported(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexParamNalStreamFormatSupported");
    OMX_NALSTREAMFORMATTYPE *pt = (OMX_NALSTREAMFORMATTYPE *)pt_org;
    pt->eNaluFormat = (OMX_NALUFORMATSTYPE)(OMX_NaluFormatStartCodes | OMX_NaluFormatFourByteInterleavedLength);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexParamNalStreamFormatSupported");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexParamParallelDeblocking(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexParamParallelDeblocking");
    OMX_PARAM_DEBLOCKINGTYPE *pt = (OMX_PARAM_DEBLOCKINGTYPE *)pt_org;
    pt->nPortIndex = 0;
    pt->bDeblocking = mParallelDeblocking;
    OstTraceInt1(TRACE_FLOW, "H264Dec_ParamAndConfig::getIndexParamParallelDeblocking mParallelDeblocking=%d",mParallelDeblocking);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexParamParallelDeblocking");
    return OMX_ErrorNone;
}

// +CR324558 CHANGE START FOR
OMX_ERRORTYPE H264Dec_ParamAndConfig::setIndexParamStaticDpbSize(OMX_PTR pt_org)
{
	OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::setIndexParamStaticDpbSize");
    VFM_PARAM_STATICDPBSIZE_TYPE *pt = (VFM_PARAM_STATICDPBSIZE_TYPE *)pt_org;
    OstTraceInt2(TRACE_FLOW,"H264Dec_ParamAndConfig::setIndexParamStaticDpbSize mIsDPBSizeStatic=%d, pt->bStaticDpbSize =%d",mIsDPBSizeStatic, pt->bStaticDpbSize);
    mIsDPBSizeStatic = pt->bStaticDpbSize;
    //Set for output the required settings.
    VFM_Port *pPort = (VFM_Port *)(getComponent()->getPort(1));
    pPort->getParamPortDefinition()->nBufferCountMin = pPort->getBufferCountMin();
    pPort->getParamPortDefinition()->nBufferCountActual = pPort->getParamPortDefinition()->nBufferCountMin;
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::setIndexParamStaticDpbSize");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_ParamAndConfig::getIndexParamStaticDpbSize(OMX_PTR pt_org) const
{
    OstTraceInt0(TRACE_API, "=>H264Dec_ParamAndConfig::getIndexParamStaticDpbSize");
    VFM_PARAM_STATICDPBSIZE_TYPE *pt = (VFM_PARAM_STATICDPBSIZE_TYPE *)pt_org;
    pt->bStaticDpbSize = mIsDPBSizeStatic;
    OstTraceInt2(TRACE_FLOW,"\nH264Dec_ParamAndConfig::getIndexParamStaticDpbSize mIsDPBSizeStatic=%d, pt->bStaticDpbSize =%d",mIsDPBSizeStatic, pt->bStaticDpbSize);
    OstTraceInt0(TRACE_API, "<=H264Dec_ParamAndConfig::getIndexParamStaticDpbSize");
    return OMX_ErrorNone;
}
// -CR324558 CHANGE END OF

// +CR349396 CHANGE START FOR
OMX_ERRORTYPE H264Dec_ParamAndConfig::setRecyclingDelay(OMX_PTR pt_org){
    OMX_ERRORTYPE error = setIndexParamRecyclingDelay(pt_org);
    if (error!=OMX_ErrorNone) return error;

    //Set for output the required settings.
    VFM_Port *pPort = (VFM_Port *)(getComponent()->getPort(1));
    pPort->getParamPortDefinition()->nBufferCountMin = pPort->getBufferCountMin();
    pPort->getParamPortDefinition()->nBufferCountActual = pPort->getParamPortDefinition()->nBufferCountMin;
    return OMX_ErrorNone;
}
// -CR349396 CHANGE START FOR

#ifdef PACKET_VIDEO_SUPPORT
// cf document openmax_call_sequences.pdf
// as well as omx_core_integration_guide.pdf
EXPORT_C OMX_ERRORTYPE H264Dec_ParamAndConfig::getPVCapability(OMX_PTR pt_org)
{
    PV_OMXComponentCapabilityFlagsType *pt = (PV_OMXComponentCapabilityFlagsType *)pt_org;
    if (pt == NULL) {
        return OMX_ErrorBadParameter;
    }

    // iIsOMXComponentMultiThreaded=OMX_TRUE as the OMX component can run in a different thread
    // from the thread in which PV framework is running
    pt->iIsOMXComponentMultiThreaded = OMX_TRUE;

    // iOMXComponentSupportsExternalOutputBufferAlloc=OMX_TRUE as we can use OMX_UseBuffer
    // on the output port
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
    pt->iOMXComponentUsesNALStartCode = OMX_TRUE;

    // iOMXComponentCanHandleIncompleteFrames=OMX_FALSE as we do not want last part of one NAL and
    // first part of the next NAL together
    // frames with missing parts
    pt->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;

    // iOMXComponentUsesFullAVCFrames=OMX_TRUE as we supports frame mode.
    // We do not want a buffer to contain a single nal
    pt->iOMXComponentUsesFullAVCFrames = OMX_TRUE;

    return OMX_ErrorNone;
}
#endif
