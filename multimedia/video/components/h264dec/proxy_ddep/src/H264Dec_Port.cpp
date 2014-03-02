/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ H264Dec_Proxy

#include "H264Dec_Proxy.h"
#include "H264Dec_Port.h"
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
#include "video_components_h264dec_proxy_ddep_src_H264Dec_PortTraces.h"
#endif

// Traces
//#ifdef PACKET_VIDEO_SUPPORT
//#undef LOG_TAG
//#define LOG_TAG "h264dec"
//#include<cutils/log.h>
//#define NMF_LOG LOGI
//#endif

H264Dec_Port::H264Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp) :
            VFM_Port(commonPortData, enscomp)
{
    OstTraceInt0(TRACE_FLOW, " H264Dec_Port::H264Dec_Port constructor");
// +CR324558 CHANGE START FOR
	mStaticDpbSize = 16;
// -CR324558 CHANGE END OF
#ifdef PACKET_VIDEO_SUPPORT
        mSuggestedBufferSize = 0;
#endif
	isUpdatedOnce = OMX_FALSE;
}

inline void H264Dec_Port::h264dec_assert(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceFiltInst1(TRACE_ERROR, "H264DEC: proxy_ddep: H264Dec_Port: h264dec_assert: VIDEOTRACE Error line %d\n", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

inline void H264Dec_Port::h264dec_assert_static(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceInt1(TRACE_ERROR, "H264DEC: proxy_ddep: H264Dec_Port: h264dec_assert: VIDEOTRACE Error line %d\n", line);
        NMF_LOG("H264DEC: proxy_ddep: H264Dec_Port: h264dec_assert: VIDEOTRACE Error line %d\n", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

// H264Dec_Port::checkSetFormatInPortDefinition() is automatically called by the VFM when setting
// the parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE H264Dec_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
    OstTraceFiltInst0(TRACE_API, "=> H264Dec_Port::checkSetFormatInPortDefinition()");
    /* + Changes for CR 334359 */
    OMX_U32 maxSupportedWidth, maxSupportedHeight;
    /* - Changes for CR 334359 */
    h264dec_assert((mParamPortDefinition.nPortIndex==portDef->nPortIndex), __LINE__, OMX_TRUE);
    switch (portDef->nPortIndex) {
    case 0:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirInput, OMX_ErrorBadParameter);
        break;
    case 1:
        RETURN_XXX_IF_WRONG(portDef->eDir==OMX_DirOutput, OMX_ErrorBadParameter);
        break;
    default:
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
    	OstTraceFiltInst0(TRACE_FLOW, "H264Dec_Port::checkSetFormatInPortDefinition - isSoc1080pCapable returned TRUE");
    }
    else
    {
    	maxSupportedWidth = 1280;
    	maxSupportedHeight = 720;
    	OstTraceFiltInst0(TRACE_FLOW, "H264Dec_Port::checkSetFormatInPortDefinition - isSoc1080pCapable returned FALSE");
    }
    OstTraceFiltInst2(TRACE_FLOW, "H264Dec_Port::max supported Width %d Height %d",maxSupportedWidth,maxSupportedHeight);

    /* - Changes for CR 334359 */

    /* + for CR 369593 */
    /* to check width and height of input port only */
    if(portDef->nPortIndex == 0 && !(getProxy()->mParam.CropWidth) && !(getProxy()->mParam.CropHeight)) /* Change for ER 426137 */
    {
		OstTraceFiltInst0(TRACE_FLOW, "H264Dec_Port:: New crop vectors initialized" );
		getProxy()->mParam.CropWidth = pt_video->nFrameWidth;
		getProxy()->mParam.CropHeight = pt_video->nFrameHeight;
		OstTraceFiltInst2(TRACE_FLOW, "H264Dec_Port::Actual Width %d and Actual Height %d",getProxy()->mParam.CropWidth,getProxy()->mParam.CropHeight);
    }

    /* making width and height as multiple of 16 */
    pt_video->nFrameHeight = (pt_video->nFrameHeight + 0xF)&(~0xF);
    pt_video->nFrameWidth  = (pt_video->nFrameWidth + 0xF)&(~0xF);
    OstTraceFiltInst2(TRACE_FLOW, "H264Dec_Port::Set New Width %d and Actual Height %d",pt_video->nFrameWidth,pt_video->nFrameHeight);
	/* - for CR 369593 */

    /* + Changes for ER346056*/
    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)>=(16*16), OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight)<=(maxSupportedWidth*maxSupportedHeight), OMX_ErrorBadParameter);
    /* - Changes for ER346056*/
    // FIXME: nothing on pt_video->nStride
    // FIXME: nothing on pt_video->nSliceHeight
    // FIXME: nothing on pt_video->nBitRate
    // FIXME: nothing on pt_video->xFramerate
    // FIXME: nothing on pt_video->bFlagErrorConcealment
    RETURN_XXX_IF_WRONG(pt_video->pNativeWindow == 0, OMX_ErrorBadParameter);

    if (portDef->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG(pt_video->eCompressionFormat==OMX_VIDEO_CodingAVC, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt_video->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt_video->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(
        ((pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)||
        (pt_video->eColorFormat==(OMX_COLOR_FORMATTYPE)0x7FFFFFFE )),
                OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }
#ifdef PACKET_VIDEO_SUPPORT
    if(portDef->nBufferSize > getBufferSize()) {
        mSuggestedBufferSize = portDef->nBufferSize;
    }
#endif
    OstTraceFiltInst0(TRACE_API, "<= H264Dec_Port::checkSetFormatInPortDefinition()");
    return OMX_ErrorNone;
}

// H264Dec_Port::checkIndexParamVideoPortFormat() is automatically called by the VFM when setting
// the video parameter of the port.
// This function checks the parameters that the user wants to set, regarding the OMX specification
// and regarding the capabilities of the components (in terms of Nomadik spec).
OMX_ERRORTYPE H264Dec_Port::checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt)
{
    h264dec_assert((mParamPortDefinition.nPortIndex==pt->nPortIndex), __LINE__, OMX_TRUE);
    h264dec_assert((pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1), __LINE__, OMX_TRUE);

   if (pt->nPortIndex==VPB+0) {            // input port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==OMX_VIDEO_CodingAVC, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat==OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(
        ((pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)||
        (pt->eColorFormat==(OMX_COLOR_FORMATTYPE)0x7FFFFFFE ))
        ,
                OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }

    return OMX_ErrorNone;
}


// Automatically called by the VFM when the parameter of the port are set.
// It returns the minimum sizeof the buffer
OMX_U32 H264Dec_Port::getBufferSize() const
{
    OstTraceInt0(TRACE_API, "=> H264Dec_Port::getBufferSize()");
    switch (getDirection()) {
    case OMX_DirInput:
        {
            OstTraceInt0(TRACE_FLOW, "In H264Dec_Port Computing buffer size for Input Port.");
            // Input buffer can be made of up to (256 PPS + 32 SPS), that is 176KBytes
            // plus 8 for safety bytes as the decoder needs to add some ==> 176KBytes + 8 bytes
            OMX_U32 default_return = 176*1024+8;
#ifdef REQUIRE_EOF
            OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferSize Input Port Size=%d",default_return);
            OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferSize()");
            return default_return;
#else
#ifdef PACKET_VIDEO_SUPPORT
            OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferSize(Android) Input Port Size=%d",1024*1024+8);
            OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferSize()");
           if(mSuggestedBufferSize != 0)
                default_return = mSuggestedBufferSize;
           else
                default_return = 1024*1024+8;
#endif  // PACKET_VIDEO_SUPPORT
            OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferSize Input Port Size=%d",default_return);
            OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferSize()");
            return default_return;
#endif  // REQUIRE_EOF

            /*
            // Input buffer can be made of up to (256 PPS + 32 SPS), that is 176KBytes
            OMX_U32 sps_pps = 176*1024;
            // Minimum compression ratio is 2. As MB-format pixel width is 3/2,
            // this gives a min_cr pixel width of 3/4
            OMX_U32 min_cr = ((getFrameWidth() * getFrameHeight() * 3) / 4);

            return sps_pps + min_cr + 8;
            */
        }
    case OMX_DirOutput:
        {
              OstTraceInt0(TRACE_FLOW, "H264Dec_Port::getBufferSize Output Port");
              return getSizeFrame(getColorFormat(), getFrameWidth(), getFrameHeight());
        }
    default:
        h264dec_assert_static((0==1), __LINE__, OMX_TRUE);
    }
    return 0;
}

OMX_U32 H264Dec_Port::getBufferCountMin() const
{
    OstTraceInt0(TRACE_API, "=> H264Dec_Port::getBufferCountMin()");
    VFM_Component *pComp = (VFM_Component *)(&getENSComponent());
    if (pComp->getParamAndConfig()->getThumbnailGeneration() == OMX_TRUE) {
        OstTraceInt0(TRACE_FLOW, "H264Dec_Port::getBufferCountMin-Thumbnail Mode");
        switch (getDirection()) {
        case OMX_DirInput:
            // we could have sps + pps + 1 frame
            OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferCountMin:Input Port Count=%d",3);
            OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferCountMin()");
            return 3;
        case OMX_DirOutput:
            // we have 1 frame to decode with the EOS directly
             OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferCountMin:Output Port Count=%d",1);
            OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferCountMin()");
            return 1;
        default:
            h264dec_assert_static((0==1), __LINE__, OMX_TRUE);
        }

    }

    int buffer_min=0;
    //OMX_U16 maxLevel;
    switch (getDirection()) {
    case OMX_DirInput:
        buffer_min  = (HAMAC_PIPE_SIZE+1);      // number of parallel processing
        buffer_min += NB_BUFFERS_INPUT_DDEP;    // received in the ddep
        buffer_min += NB_BUFFERS_IN_SOURCE;     // processed by the source
        OstTraceInt1(TRACE_FLOW, "H264Dec_Port::getBufferCountMin:Input Port Count=%d",buffer_min);
	    break;
// +CR324558 CHANGE START FOR
    case OMX_DirOutput:
        // FIXME: use the current level instead of Level4
            OMX_U32 /* profile, */ level;
            OMX_U32 internalDPBSize;

            // getProxy()->mParam.getProfileLevel(&profile, &level);
            OMX_U16 maxLevel;
            maxLevel = getProxy()->mParam.getLevelCapabilities();
            if(!maxLevel) {
                // Level not intilized
                maxLevel = VFM_SocCapabilityMgt::getLevelCapabilities();
                getProxy()->mParam.setLevelCapabilities(maxLevel);
            }

            //Fetch default level value
            OMX_U32 default_level;
			default_level = VFM_SocCapabilityMgt::getH264DecoderDefaultLevel();
			OstTraceInt2(TRACE_FLOW, "H264Dec_Port::getBufferCountMin::::maxlevel=%d, def_level=%d",maxLevel,default_level);
            if(default_level>maxLevel) {
                default_level = maxLevel;
            }

            switch (default_level)
            {
                case 9:
                    level = OMX_VIDEO_AVCLevel1b;
                    break;

                case 10:
                    level = OMX_VIDEO_AVCLevel1;
                    break;

                case 11:
                    level = OMX_VIDEO_AVCLevel11;
                    break;

                case 12:
                    level = OMX_VIDEO_AVCLevel12;
                    break;

                case 13:
                    level = OMX_VIDEO_AVCLevel13;
                    break;

                case 20:
                    level = OMX_VIDEO_AVCLevel2;
                    break;

                case 21:
                    level = OMX_VIDEO_AVCLevel21;
                    break;

                case 22:
                    level = OMX_VIDEO_AVCLevel22;
                    break;

                case 30:
                    level = OMX_VIDEO_AVCLevel3;
                    break;

                case 31:
                    level = OMX_VIDEO_AVCLevel31;
                    break;

                case 32:
                    level = OMX_VIDEO_AVCLevel32;
                    break;

                case 40:
                    level = OMX_VIDEO_AVCLevel4;
                    break;

                case 41:
                    level = OMX_VIDEO_AVCLevel41;
                    break;

                case 42:
                    level = OMX_VIDEO_AVCLevel42;
                    break;

                case 50:
                    level = OMX_VIDEO_AVCLevel5;
                    break;

                case 51:
                    level = OMX_VIDEO_AVCLevel51;
                    break;

                default:
                    level = OMX_VIDEO_AVCLevel42;
                    break;
            }


            if(getProxy()->mParam.isDpbSizeStatic()) {
                buffer_min = mStaticDpbSize;
                buffer_min = getProxy()->addExtraPipeBuffers(buffer_min);
                OstTraceInt2(TRACE_FLOW,"H264Dec_Port::getBufferCountMin buffer_min=%d, mStaticDpbSize = %d",buffer_min, mStaticDpbSize);
                return buffer_min;
            }else{
                buffer_min = H264Dec_Proxy::getDpbSizeInBytes((OMX_VIDEO_AVCLEVELTYPE)level) / getSizeFrame(getColorFormat(), getFrameWidth(), getFrameHeight());
                internalDPBSize =getProxy()->processingComponent.getInternalDPBSize();
                if (buffer_min<1) {
                    buffer_min = 1;
                } else if (buffer_min>16) {
                    buffer_min = 16;
                }
                OstTraceInt2(TRACE_FLOW,"\nH264Dec_Port::nonstatic_getBufferCountMin buffer_min=%d, internalDPBSize = %d",buffer_min, internalDPBSize);
                /*CR 324558:Calcuate on basis of dpb requiremennts*/
                buffer_min = (internalDPBSize)?(internalDPBSize):(buffer_min);
                buffer_min = getProxy()->addExtraPipeBuffers(buffer_min);

            }
            break;

// -CR324558 CHANGE END OF
    default:
        h264dec_assert_static((0==1), __LINE__, OMX_TRUE);
    }

    OstTraceInt0(TRACE_API, "<= H264Dec_Port::getBufferCountMin()");
    return buffer_min;
}


// Set the default value of the port. This function is used by the construct() function of the proxy
// when creating a new instance of the proxy
void H264Dec_Port::setDefault()
{
    h264dec_assert((mParamPortDefinition.eDomain==OMX_PortDomainVideo), __LINE__, OMX_TRUE);
    H264Dec_Proxy *comp = getProxy();

    mParamPortDefinition.format.video.cMIMEType = (char *)"video/H264";
    mParamPortDefinition.format.video.pNativeRender = 0;
    mParamPortDefinition.format.video.nFrameHeight = 144;      // from OMX spec 1.1.1
    mParamPortDefinition.format.video.nFrameWidth = 176;       // from OMX spec 1.1.1
    mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_TRUE;
    mParamPortDefinition.format.video.pNativeWindow = 0;

    switch (mParamPortDefinition.nPortIndex) {
    case 0:
        h264dec_assert((mParamPortDefinition.eDir==OMX_DirInput), __LINE__, OMX_TRUE);
        mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
        mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
        mParamPortDefinition.format.video.nBitrate = 64000;        // from OMX spec 1.1.1
        mParamPortDefinition.format.video.xFramerate = 0;          // Decrepency wrt to OMX spec 1.1.1 that specifies 15, that is 15<<16
        mParamPortDefinition.format.video.nStride = 0;             // unused for compressed data
        mParamPortDefinition.format.video.nSliceHeight = 1;        // unused for compressed data
        comp->mParam.setProfileLevel(OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1);  // from OMX spec 1.1.1
        break;
    case 1:
        h264dec_assert((mParamPortDefinition.eDir==OMX_DirOutput), __LINE__, OMX_TRUE);
        mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
// #ifdef PACKET_VIDEO_SUPPORT
#if 0
        mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)0x7FFFFFFE;
#else
        mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
#endif
        mParamPortDefinition.format.video.nBitrate = 0;
        mParamPortDefinition.format.video.xFramerate = 0;
        mParamPortDefinition.format.video.nStride = (mParamPortDefinition.format.video.nFrameWidth * 3) / 2;        // corresponds to a raw in OMX_COLOR_FormatYUV420Planar
        mParamPortDefinition.format.video.nSliceHeight = 16;        // a single raw in the buffer is required.

        // output buffers cannot be modified by the component we are tunneled to as they are still used as reference frames
        // setTunnelFlags(OMX_PORTTUNNELFLAG_READONLY);    FIXME: commented right now as it breaks on Symbian side
        break;
    default:
        h264dec_assert((0==1), __LINE__, OMX_TRUE);
    }

    // mParamPortDefinition.nBufferCountActual updated by the component when buffers are allocated
    // mParamPortDefinition.nBufferCountMin set at the creation of the port (check constructor of the proxy)
    // mParamPortDefinition.bEnabled set by the component
    // mParamPortDefinition.bPopulated set by the component
    mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
    mParamPortDefinition.nBufferAlignment = 0x100;
    mParamPortDefinition.nBufferSize = getBufferSize();
    mParamPortDefinition.nBufferCountMin = getBufferCountMin();
    mParamPortDefinition.nBufferCountActual = mParamPortDefinition.nBufferCountMin;
}


OMX_ERRORTYPE H264Dec_Port::setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure)
{
	//NMF_LOG("Inside H264Dec_Port::setParameter\n");
	OMX_ERRORTYPE errorCode;
	OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);
	OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portdef->format.video));

	if (portdef->nPortIndex == 0)
	{
		isUpdatedOnce = OMX_FALSE;
	}
	//NMF_LOG("Values are Portindex: %d width %d height %d \n",portdef->nPortIndex,pt_video->nFrameWidth,pt_video->nFrameHeight);
	errorCode = ENS_Port::setParameter(nParamIndex,pComponentParameterStructure);
	if (errorCode== OMX_ErrorNone && nParamIndex == OMX_IndexParamPortDefinition)
	{
		if ((portdef->nPortIndex == 1) && (!isUpdatedOnce))
		{
			//NMF_LOG("Inside H264Dec_Port::setParameter  abt to change the value \n");
			//NMF_LOG("Inside H264Dec_Port::setParameter  Before old value : %d \n",mParamPortDefinition.nBufferCountActual);
			mParamPortDefinition.nBufferCountActual = mParamPortDefinition.nBufferCountMin;
			//NMF_LOG("Inside H264Dec_Port::setParameter  Value changed : %d \n",mParamPortDefinition.nBufferCountActual);
			isUpdatedOnce = OMX_TRUE;
		}
	}
	return errorCode;
}

