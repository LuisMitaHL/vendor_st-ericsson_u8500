/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_proxy_src_MPEG2Dec_PortTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE
#include "VFM_DDepUtility.h"
#include "MPEG2Dec_Proxy.h"
#include "MPEG2Dec_Port.h"



#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { mpeg2dec_port_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { mpeg2dec_port_assert(_error, __LINE__, OMX_FALSE); return (_error); } }

void MPEG2Dec_Port::mpeg2dec_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "MPEG2Dec_Port : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}

MPEG2Dec_Port::MPEG2Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp) :
            VFM_Port(commonPortData, enscomp)
{
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port_constructor");
    if (commonPortData.mDirection == OMX_DirInput) {            // input port
        mParamPortDefinition.format.video.nStride = 0;
        mParamPortDefinition.format.video.pNativeRender = 0;
        mParamPortDefinition.format.video.pNativeWindow = 0;
        mParamPortDefinition.format.video.xFramerate = 0;
        mParamPortDefinition.format.video.nSliceHeight = 0;
        mParamPortDefinition.format.video.nFrameHeight = 0;
        mParamPortDefinition.format.video.nFrameWidth = 0;
        mParamPortDefinition.format.video.nBitrate = 0;
        mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
        mParamPortDefinition.format.video.cMIMEType = 0;

        mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
        mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;

        mParamPortDefinition.nBufferSize = 256; //any non-zero value

        mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
        mParamPortDefinition.nBufferAlignment = 16;

    } else  {        // output port
        mParamPortDefinition.format.video.nStride = 0;
        mParamPortDefinition.format.video.pNativeRender = 0;
        mParamPortDefinition.format.video.pNativeWindow = 0;
        mParamPortDefinition.format.video.xFramerate = 0;
        mParamPortDefinition.format.video.nSliceHeight = 0;
        mParamPortDefinition.format.video.nFrameHeight = 0;
        mParamPortDefinition.format.video.nFrameWidth = 0;
        mParamPortDefinition.format.video.nBitrate = 0;
        mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
        mParamPortDefinition.format.video.cMIMEType = 0;

        mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
        mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;

        mParamPortDefinition.nBufferSize = 256; //any non-zero value

        mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
        mParamPortDefinition.nBufferAlignment = 256;

    }

#ifdef PACKET_VIDEO_SUPPORT
        mSuggestedBufferSize = 0;
#endif

    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port_constructor");
}

void MPEG2Dec_Port::getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const
{
   OstTraceFiltInst0(TRACE_API, "=> MPEG2Dec_Port::getSlavedSizeFromGivenMasterSize()");

   *nWidthSlave   = nWidthMaster;
   *nHeightSlave  = nHeightMaster;
   OstTraceFiltInst2(TRACE_FLOW, "=> MPEG2Dec_Port::getSlavedSizeFromGivenMasterSize() nWidthMaster %d nHeightMaster %d",nWidthMaster,nHeightMaster);
   OstTraceFiltInst2(TRACE_FLOW, "=> MPEG2Dec_Port::getSlavedSizeFromGivenMasterSize() *nWidthSlave %d *nHeightSlave %d",*nWidthSlave,*nHeightSlave);

   OstTraceFiltInst0(TRACE_API, "<= MPEG2Dec_Port::getSlavedSizeFromGivenMasterSize()");

}


// MPEG2Dec_Port::checkSetFormatInPortDefinition() is automatically called by the VFM when setting
// the parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE MPEG2Dec_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
   MPEG2Dec_Proxy *pComp = (MPEG2Dec_Proxy *)(&getENSComponent());
   MPEG2Dec_ParamAndConfig *pParam = ((MPEG2Dec_ParamAndConfig *)(pComp->getParamAndConfig()));
   OstTraceInt0(TRACE_API, "=> MPEG2Dec_Port::checkSetFormatInPortDefinition()");
   OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portDef->format.video));

	OMX_U32 maxSupportedWidth=1280, maxSupportedHeight=720;
	RETURN_XXX_IF_WRONG(0 <=(signed)(pt_video->nFrameWidth*pt_video->nFrameHeight), OMX_ErrorBadParameter);
	RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight) <= (maxSupportedWidth*maxSupportedHeight), OMX_ErrorBadParameter);

    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::checkSetFormatInPortDefinition");
    DBC_ASSERT(mParamPortDefinition.nPortIndex==portDef->nPortIndex);
    switch (portDef->nPortIndex)
        {
        case 0:
            RETURN_XXX_IF_WRONG_OST(portDef->eDir==OMX_DirInput, OMX_ErrorBadParameter);
            break;
        case 1:
            RETURN_XXX_IF_WRONG_OST(portDef->eDir==OMX_DirOutput, OMX_ErrorBadParameter);
            break;
        default:
            RETURN_XXX_IF_WRONG_OST(false, OMX_ErrorBadParameter);
        }

    RETURN_XXX_IF_WRONG_OST(portDef->eDomain==OMX_PortDomainVideo, OMX_ErrorBadParameter);
    // no check on nBufferCountActual, nBufferCountMin, nBufferSize, bEnabled, bPopulated, bBuffersContiguous, nBufferAlignment

    // pt_video->cMIMEType: no check
    RETURN_XXX_IF_WRONG_OST(pt_video->pNativeRender == 0, OMX_ErrorBadParameter);
    // FIXME: nothing on pt_video->nStride
    // FIXME: nothing on pt_video->nSliceHeight
    // FIXME: nothing on pt_video->nBitRate
    // FIXME: nothing on pt_video->xFramerate
    // FIXME: nothing on pt_video->bFlagErrorConcealment
    RETURN_XXX_IF_WRONG_OST(pt_video->pNativeWindow == 0, OMX_ErrorBadParameter);
    if (portDef->nPortIndex==VPB+0)
        {
        // input port
        RETURN_XXX_IF_WRONG_OST(pt_video->eCompressionFormat==OMX_VIDEO_CodingMPEG2, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG_OST(pt_video->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
        }
    else
        {
        // output port
        RETURN_XXX_IF_WRONG_OST(pt_video->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG_OST(
                pt_video->eColorFormat==OMX_COLOR_FormatYUV420Planar
                || pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
        }

#ifdef PACKET_VIDEO_SUPPORT
                if(portDef->nBufferSize > getBufferSize()) {
                    mSuggestedBufferSize = portDef->nBufferSize;
                }
#endif

    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::checkSetFormatInPortDefinition");
    return OMX_ErrorNone;
}

// MPEG2Dec_Port::checkIndexParamVideoPortFormat() is automatically called by the VFM when setting
// the video parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE MPEG2Dec_Port::checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt)
{

    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::checkIndexParamVideoPortFormat");
    DBC_ASSERT(mParamPortDefinition.nPortIndex==pt->nPortIndex);
    DBC_ASSERT(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1);

    if (pt->nPortIndex==VPB+0)
        {
        // input port
        RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat==OMX_VIDEO_CodingMPEG2, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG_OST(pt->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
        }
    else
        {
        // output port
        RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG_OST( pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420PackedSemiPlanar
                          || pt->eColorFormat==OMX_COLOR_FormatYUV420Planar,OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
        }
    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::checkIndexParamVideoPortFormat");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG2Dec_Port::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
    OstTraceInt0(TRACE_API, "=> <PG> MPEG2Dec_Port::setFormatInPortDefinition()");
    MPEG2Dec_Proxy *pComp = (MPEG2Dec_Proxy *)(&getENSComponent());
    MPEG2Dec_ParamAndConfig *pParam = ((MPEG2Dec_ParamAndConfig *)(pComp->getParamAndConfig()));
    OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portDef.format.video));

    if(portDef.nPortIndex == 0 && !(pParam->CropWidth) && !(pParam->CropHeight)) /* Change for ER 426137 */
    {
        pParam->CropWidth = pt_video->nFrameWidth;
        pParam->CropHeight = pt_video->nFrameHeight;
        OstTraceInt2(TRACE_FLOW, "=> <PG> MPEG2Dec_Port::setFormatInPortDefinition() Updated Crop width : %d Crop height : %d ",pParam->CropWidth,pParam->CropHeight);
    }

        //Making it x16
	pt_video->nFrameWidth = ((pt_video->nFrameWidth + 0xF) & (~0xF));
	pt_video->nFrameHeight = ((pt_video->nFrameHeight + 0xF) & (~0xF));

	OstTraceInt3(TRACE_FLOW, "=> Inside NEW nPortIndex %d pt_video->nFrameHeight %d mParamPortDefinition.format.video.nFrameHeight %d",portDef.nPortIndex,pt_video->nFrameHeight,mParamPortDefinition.format.video.nFrameHeight);
	if ((pt_video->nFrameHeight - mParamPortDefinition.format.video.nFrameHeight) == 16)
	{
		pt_video->nFrameHeight -=16;
	}
	OstTraceInt3(TRACE_FLOW, "=> AFTER Inside NEW nPortIndex %d pt_video->nFrameHeight %d mParamPortDefinition.format.video.nFrameHeight %d",portDef.nPortIndex,pt_video->nFrameHeight,mParamPortDefinition.format.video.nFrameHeight);


	return VFM_Port::setFormatInPortDefinition(portDef);
}



OMX_ERRORTYPE MPEG2Dec_Port::setIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef){
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::setIndexParamVideoPortFormat");
	OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)portDef;
	RETURN_XXX_IF_WRONG_OST(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);
	if(pt->nPortIndex==VPB+1){
		RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat == OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG_OST(pt->eColorFormat== (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, OMX_ErrorBadParameter);
	}
	else{
		RETURN_XXX_IF_WRONG_OST(pt->eCompressionFormat == OMX_VIDEO_CodingMPEG2, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG_OST(pt->eColorFormat == OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
	}

	mParamPortDefinition.format.video.eColorFormat = pt->eColorFormat;
	mParamPortDefinition.format.video.eCompressionFormat = pt->eCompressionFormat;
	mParamPortDefinition.format.video.xFramerate = pt->xFramerate;
    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::setIndexParamVideoPortFormat");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_Port::getIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef){
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::getIndexParamVideoPortFormat");
	OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)portDef;

	RETURN_XXX_IF_WRONG_OST(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);

	if(pt->nPortIndex==VPB+1){
		pt->eCompressionFormat = OMX_VIDEO_CodingUnused;
        pt->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
	}
	else{
		pt->eCompressionFormat = OMX_VIDEO_CodingMPEG2;
		pt->eColorFormat = OMX_COLOR_FormatUnused;
	}
	pt->nIndex = 0xFF;		  // dummy value
    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::getIndexParamVideoPortFormat");
    return OMX_ErrorNone;
}

// Automatically called by the VFM when the parameter of the port are set.
// It returns the minimum sizeof the buffer
OMX_U32 MPEG2Dec_Port::getBufferSize() const
{
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::getBufferSize");
    OMX_U32 default_return = 256;

    switch (getDirection())
        {
        case OMX_DirInput:
	         //+CR369244
	         if(VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize())
	         {
	             default_return = VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize();
	         }
            break;

        case OMX_DirOutput:
            default_return = (getFrameWidth()*(getFrameHeight() + 16)*3)/2; // Need 16 more pixel in height for some interleaved streams
            break;
        default:
            DBC_ASSERT(0==1);
        }
	#ifdef PACKET_VIDEO_SUPPORT
            if(mSuggestedBufferSize != 0) {
                default_return = mSuggestedBufferSize;
            }
	#endif

    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::getBufferSize");
    return default_return;
}

OMX_U32 MPEG2Dec_Port::getBufferCountMin() const
{
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::getBufferCountMin");
    int buffer_min=0;

    switch (getDirection())
        {
        case OMX_DirInput:
            buffer_min = mParamPortDefinition.nBufferCountMin;
            break;
        case OMX_DirOutput:
            // FIXME: use the current level instead of Level4
            buffer_min = mParamPortDefinition.nBufferCountMin;
            break;
        default:
            DBC_ASSERT(0==1);
        }

    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::getBufferCountMin");
    return buffer_min;
}

// Set the default value of the port. This function is used by the construct() function of the proxy
// when creating a new instance of the proxy
void MPEG2Dec_Port::setDefault()
{
    OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Port::setDefault");
    DBC_ASSERT(mParamPortDefinition.eDomain==OMX_PortDomainVideo);
    //MPEG2Dec_Proxy *comp = getProxy();

    mParamPortDefinition.format.video.cMIMEType = (char *)"video/MPEG2";
    mParamPortDefinition.format.video.pNativeRender = 0;
    mParamPortDefinition.format.video.nFrameHeight = 16;      // from OMX spec 1.1.1
    mParamPortDefinition.format.video.nFrameWidth = 16;       // from OMX spec 1.1.1
    mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
    mParamPortDefinition.format.video.pNativeWindow = 0;

    switch (mParamPortDefinition.nPortIndex)
        {
        case 0:
            DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirInput);
            mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;
            mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
            mParamPortDefinition.format.video.nBitrate = 64000;        // from OMX spec 1.1.1
            mParamPortDefinition.format.video.xFramerate = 15;         // from OMX spec 1.1.1
            mParamPortDefinition.format.video.nStride = 0;             // unused for compressed data
            mParamPortDefinition.format.video.nSliceHeight = 1;        // unused for compressed data
            //comp->mParam.setProfileLevel(OMX_VIDEO_MPEG2ProfileSimple, OMX_VIDEO_MPEG2LevelLL);  // from OMX spec 1.1.1
            break;
        case 1:
            DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirOutput);
            mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
            mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
            mParamPortDefinition.format.video.nBitrate = 0;
            mParamPortDefinition.format.video.xFramerate = 0;
            mParamPortDefinition.format.video.nStride = (mParamPortDefinition.format.video.nFrameWidth * 3) / 2;        // corresponds to a raw in OMX_COLOR_FormatYUV420Planar
            mParamPortDefinition.format.video.nSliceHeight = 1;        // a single raw in the buffer is required. FIXME
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
    OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Port::setDefault");
}

OMX_ERRORTYPE MPEG2Dec_Port::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst0(TRACE_API, "[PROXY] > MPEG2Dec_Port::getParameter");
    OMX_ERRORTYPE errorType = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE *portdef;
    switch (nParamIndex)
    {
		case OMX_IndexParamPortDefinition:
		     OstTraceFiltInst0(TRACE_FLOW, "[PROXY] > MPEG2Dec_Port::case of OMX_IndexParamPortDefinition");
			 portdef = (OMX_PARAM_PORTDEFINITIONTYPE *)(pt);
             errorType = ENS_Port::getParameter(nParamIndex,pt);
             portdef->format.video.nFrameHeight += 16; //updated height
             OstTraceFiltInst2(TRACE_FLOW, "MPEG2Dec_Port::Updated width : %d Updated height : %d",portdef->format.video.nFrameWidth,portdef->format.video.nFrameHeight);
             break;
        default:
                RETURN_OMX_ERROR_IF_ERROR(VFM_Port::getParameter(nParamIndex, pt));
    }
    OstTraceFiltInst0(TRACE_API, "[PROXY] < MPEG2Dec_Port::getParameter");
    return errorType;
}
