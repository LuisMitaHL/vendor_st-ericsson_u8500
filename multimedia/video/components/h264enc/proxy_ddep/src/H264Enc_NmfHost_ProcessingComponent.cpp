/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ H264Enc_Proxy
#include "osi_trace.h"
#include "H264Enc_Proxy.h"
#include "H264Enc_NmfHost_ProcessingComponent.h"
#include "VFM_DDepUtility.h"
#include "VFM_Port.h"
#include "H264Enc_Port.h"
// include h264enc arm nmf component interfaces
#include <api/h264enc.h>

#ifndef H264ENC_TRACE_GROUP_PROXY
    #include "OstTraceDefinitions_ste.h"
    #include "OpenSystemTrace_ste.h"
    #ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_proxy_ddep_src_H264Enc_NmfHost_ProcessingComponentTraces.h"
    #endif
#endif

/* +Change for 372717 */
#define make16multiple(x) (((x + 15)/16)*16)
#define offset_to_next16_multiple(x) (x?(16-x):0)
/* -Change for 372717 */

class H264Enc_GetSpsPpsCB : public h264enc_arm_nmf_api_get_sps_ppsDescriptor
{
    friend class H264Enc_NmfHost_ProcessingComponent;
    public:
        H264Enc_GetSpsPpsCB(H264Enc_NmfHost_ProcessingComponent *pProcessingComponent) : mProcessingComponent(pProcessingComponent) { }

    private:
        virtual void get_sps_pps(t_sps_pps* pSequenceHeader) { mProcessingComponent->getSpsPpsCB(pSequenceHeader); };

        /// @brief Reference to the ENS component it belongs to
        H264Enc_NmfHost_ProcessingComponent *mProcessingComponent;
};


OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecInstantiate()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::codecInstantiate");

    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport",20));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport",20));
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_config", 4, &mISetConfig)==NMF_OK), OMX_ErrorInsufficientResources);
    //RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_param", 2, &mISetParam)==NMF_OK), OMX_ErrorInsufficientResources);
    //RETURN_XXX_IF_WRONG((mCodec->bindFromUser("PendingCommandAck", 2, &mIpendingCommandAck)==NMF_OK), OMX_ErrorInsufficientResources);

    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    mGetSpsPpsCB = new H264Enc_GetSpsPpsCB(this);
    RETURN_XXX_IF_WRONG(mGetSpsPpsCB!=0, OMX_ErrorInsufficientResources);
    RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, "get_sps_pps", mGetSpsPpsCB, 1)==NMF_OK), OMX_ErrorInsufficientResources);

    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecInstantiate");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecStart()
{
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecStop()
{
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst4(TRACE_API, "=> H264Enc_NmfHost_ProcessingComponent::emptyThisBuffer() pBuffer=0x%x, pBuffer->nTimeStamp=%d, pBuffer->nFilledLen=%d, pBuffer->nFlags=0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->nTimeStamp, pBuffer->nFilledLen, pBuffer->nFlags);
	H264Enc_Port *pt_port_in;
	OMX_VIDEO_PORTDEFINITIONTYPE * mvideo_port;
	pt_port_in = (H264Enc_Port *)pProxyComponent->getPort(VPB+0);
	mvideo_port = pt_port_in->getVideoPortDefinition();
	OMX_U32 computedStride = pt_port_in->getStride(mvideo_port->eColorFormat,mvideo_port->nFrameWidth);
	OstTraceFiltInst2(TRACE_FLOW,"H264Enc_NmfHost_ProcessingComponent:: Actual Stride : %d computedStride %d ",mvideo_port->nStride,computedStride);
	if((VFM_SocCapabilityMgt::getOutputBufferCount()) &&
	   ((unsigned)mvideo_port->nStride > computedStride))
	{
		pBuffer->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
	}
	return VFM_NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecConfigure()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::codecConfigure");
    //VFM_NmfHost_ProcessingComponent_SENDPARAM;
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecConfigure");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecDeInstantiate()
{
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_config")==NMF_OK), OMX_ErrorUndefined);
    //RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_param")==NMF_OK), OMX_ErrorUndefined);
    //RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("PendingCommandAck")==NMF_OK), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "get_sps_pps")==NMF_OK), OMX_ErrorUndefined);
    DBC_ASSERT(mGetSpsPpsCB);
    delete mGetSpsPpsCB;
    mGetSpsPpsCB = 0;

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));
    return OMX_ErrorNone;
}


void H264Enc_NmfHost_ProcessingComponent::registerStubsAndSkels()
{
#ifndef NO_HAMAC
#ifndef __ndk5500_a0__
    CM_REGISTER_STUBS_SKELS(video_h264enc_ddep_cpp);
#ifdef DEBUG_TRACES
    CM_REGISTER_SKELS(video_debug_dsp_printf_cpp);
#endif
    CM_REGISTER_SKELS(video_h264enc_algo);
#endif
#endif
}

void H264Enc_NmfHost_ProcessingComponent::unregisterStubsAndSkels()
{
#ifndef NO_HAMAC
#ifndef __ndk5500_a0__
    CM_UNREGISTER_STUBS_SKELS(video_h264enc_ddep_cpp);
#ifdef DEBUG_TRACES
    CM_UNREGISTER_SKELS(video_debug_dsp_printf_cpp);
#endif
    CM_UNREGISTER_SKELS(video_h264enc_algo);
#endif
#endif
}

OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::codecCreate");

#ifdef NO_HAMAC
    mCodec = h264enc_arm_nmf_proxynmf_stubCreate();
    if (mCodec) {
        // no domainId usage in the sw version
        OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecCreate");
        return OMX_ErrorNone;
    }
#else
    t_frameinfo frame_info;
    codecRetrieveConfig(&frame_info);
/* +Change for 372717 */
    OstTraceFiltInst1(H264ENC_TRACE_GROUP_PROXY, "pic_height: %d",make16multiple((frame_info.common_frameinfo).pic_height));
/* -Change for 372717 */
	if (select1080FW) // use HDTV firmware
	{
        OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "Creating hdtv NMF component");
        mCodec = h264enc_arm_nmf_proxynmf_hdtvCreate();
        if (mCodec) {
#ifndef __ndk5500_a0__
            ((h264enc_arm_nmf_proxynmf_hdtv *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
#endif
            OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecCreate");
            return OMX_ErrorNone;
		}
    }
	else
	{
        OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "Creating visio NMF component");
        mCodec = h264enc_arm_nmf_proxynmfCreate();
        if (mCodec) {
#ifndef __ndk5500_a0__
    	    ((h264enc_arm_nmf_proxynmf *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
#endif
            OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecCreate");
    	    return OMX_ErrorNone;
		}
    }
#endif
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecCreate");
   return OMX_ErrorInsufficientResources;
}

void H264Enc_NmfHost_ProcessingComponent::codecDestroy(void)
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::codecDestroy");

#ifdef NO_HAMAC
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "Destroy stub NMF component");
    h264enc_arm_nmf_proxynmf_stubDestroy((h264enc_arm_nmf_proxynmf_stub *&)mCodec);
#else
    t_frameinfo frame_info;
    codecRetrieveConfig(&frame_info);

    OstTraceInt1(H264ENC_TRACE_GROUP_PROXY, "pic_height: %d",(frame_info.common_frameinfo).pic_height);

    if (select1080FW)
	{
        OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "Destroy hdtv NMF component");
        h264enc_arm_nmf_proxynmf_hdtvDestroy((h264enc_arm_nmf_proxynmf_hdtv *&)mCodec);

    }
    else
    {
        OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "Destroy visio NMF component");
        h264enc_arm_nmf_proxynmfDestroy((h264enc_arm_nmf_proxynmf *&)mCodec);
    }
#endif

    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecDestroy");
}


void H264Enc_NmfHost_ProcessingComponent::getSpsPpsCB(t_sps_pps* pSequenceHeader)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::getSpsPpsCB");

    static_cast<H264Enc_Proxy*>(&mENSComponent)->getParamAndConfig()->setSpsPps(pSequenceHeader);

    //mIpendingCommandAck.pendingCommandAck(0);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::getSpsPpsCB");
}


void H264Enc_NmfHost_ProcessingComponent::sendConfigToCodec()
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::sendConfigToCodec");
    VFM_NmfHost_ProcessingComponent_SENDCONFIGTOCODEC;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::sendConfigToCodec");
}

void H264Enc_NmfHost_ProcessingComponent::codecRetrieveConfig(t_frameinfo *pFrameInfo)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_NmfHost_ProcessingComponent::codecRetrieveConfig");
    OMX_U32 delay = 0;
    memset(pFrameInfo, 0, sizeof(t_frameinfo));

    VFM_retrieveConfig(&(pFrameInfo->common_frameinfo));

    // specific_frameinfo to be added
    t_frameinfo info = static_cast<H264Enc_Proxy*>(&mENSComponent)->getParamAndConfig()->getParams();
    pFrameInfo->specific_frameinfo = info.specific_frameinfo;

    /* + changes for CR 343589 */
	/* swapping the values of width and height */
	if ( ((pFrameInfo->specific_frameinfo).nRotation == 90)  || ((pFrameInfo->specific_frameinfo).nRotation == 270) ||
	     ((pFrameInfo->specific_frameinfo).nRotation == -90) || ((pFrameInfo->specific_frameinfo).nRotation == -270) )
	{
		OMX_U32 tempValue;
		tempValue = (pFrameInfo->common_frameinfo).pic_width;
		(pFrameInfo->common_frameinfo).pic_width = (pFrameInfo->common_frameinfo).pic_height;
		(pFrameInfo->common_frameinfo).pic_height = tempValue;
	}
	/* - changes for CR 343589 */


    //todo: fix this temporary hack
    (pFrameInfo->specific_frameinfo).sfw = (pFrameInfo->common_frameinfo).pic_width;
    (pFrameInfo->specific_frameinfo).sfh = (pFrameInfo->common_frameinfo).pic_height;
    (pFrameInfo->specific_frameinfo).sho = 0;
    (pFrameInfo->specific_frameinfo).svo = 0;
    //(pFrameInfo->specific_frameinfo).Bitrate
    //(pFrameInfo->specific_frameinfo).FrameRate


	/* + CR 332873 new critera as proposed in PPT */
	if ((pFrameInfo->specific_frameinfo).CpbBufferSize == 0)
	{
		(pFrameInfo->specific_frameinfo).CpbBufferSize = (pFrameInfo->specific_frameinfo).Bitrate;
	}
	delay = (pFrameInfo->specific_frameinfo).CpbBufferSize / (pFrameInfo->specific_frameinfo).Bitrate;


    if (pProxyComponent->isSpecialCTCase == OMX_TRUE) //new handle
    {
		OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] In codecRetrieveConfig, NEW handle used");
		if ((pFrameInfo->specific_frameinfo).BrcType == 4) //VBR case
		{
			select1080FW = OMX_FALSE;
		}
		else
		{
			/* + change for ER 346056 */
			OMX_U32 thresholdValueMB = (960/16)*(544/16); //threshold value is qHD (960x544) resolution
			OMX_U32 actualMBs = ((pFrameInfo->common_frameinfo).pic_width/16)*((pFrameInfo->common_frameinfo).pic_height/16);
			if (actualMBs <= thresholdValueMB)
			{
			/* - change for ER 346056 */
				select1080FW = OMX_FALSE;
			}
			else
			{
				select1080FW = OMX_TRUE;
			}
		}
	}
	else //old handle
	{
		OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] In codecRetrieveConfig, old handle used");
		/* + change for ER 346056 */
		OMX_U32 thresholdValueMB = (1280/16)*(720/16); //threshold value is 1280x720 resolution
		OMX_U32 actualMBs = ((pFrameInfo->common_frameinfo).pic_width/16)*((pFrameInfo->common_frameinfo).pic_height/16);
		if (actualMBs < thresholdValueMB)
		{
		/* - change for ER 346056 */
			OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] In codecRetrieveConfig, 1080FW NOT selected");
			select1080FW = OMX_FALSE;
		}
		else
		{
			select1080FW = OMX_TRUE;
			OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] In codecRetrieveConfig, 1080FW selected");
		}
	}

	/*- code for CR 332873 new critera as proposed in PPT */


	// camcorder FW
	if (select1080FW)
    {
        (pFrameInfo->specific_frameinfo).EncoderComplexity  = 0;
        (pFrameInfo->specific_frameinfo).LowComplexity      = 3;
        (pFrameInfo->specific_frameinfo).TransformMode      = 0;
        (pFrameInfo->specific_frameinfo).disableH4D         = 1; // no deblocking with camcorder FW
#ifndef __STN_9540
#if __STN_8500 < 30 // V1 chip
        // Search window limited to 16 for V1
        (pFrameInfo->specific_frameinfo).SearchWindow       = 16;
#endif
#endif
    }
    else if ( (pFrameInfo->specific_frameinfo).ProfileIDC == 100 ) // visio FW - High profile
    {
        (pFrameInfo->specific_frameinfo).LowComplexity      = 0;
        (pFrameInfo->specific_frameinfo).TransformMode      = 1;
#ifndef __STN_9540
#if __STN_8500 < 30 // V1 chip
        // HW bug on V1 (deblocking not working if TransformMode=1)
        (pFrameInfo->specific_frameinfo).disableH4D         = 1;
#endif
#endif
    }
    else // visio FW - Baseline/Main profile
    {
        (pFrameInfo->specific_frameinfo).LowComplexity      = 0;
        (pFrameInfo->specific_frameinfo).TransformMode      = 0;
    }
//+ code for step 2 of CR 332873
    const OMX_U32 nbOfMacroBlocksWVGA = (864*480)/(16*16);
	OMX_U32 nbOfMacroBlocks = ((pFrameInfo->common_frameinfo).pic_width / 16) *((pFrameInfo->common_frameinfo).pic_height / 16);

	if ((nbOfMacroBlocks > nbOfMacroBlocksWVGA) && ((pFrameInfo->specific_frameinfo).BrcType == 2) )
	{
	    (pFrameInfo->specific_frameinfo).CBR_simplified_algo = 1;
	}

	/* specifically done for 1088 resolution to be played as 1080 resolution - introduced in new FW */
/* +Change for 372717 */
#if 0 // this code before
	if (((pFrameInfo->common_frameinfo).pic_height == 1088) && ((pFrameInfo->common_frameinfo).pic_width == 1920))
	{
		(pFrameInfo->specific_frameinfo).frame_cropping_flag = 1;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_left_offset     = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_right_offset    = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_top_offset      = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_bottom_offset   = 4; //internal fix done (!!SHOULD BE 4!!)
	}

	/* + change for CR 343589 */
	/* specifically done for 1088 resolution to be played as 1080 resolution - introduced in new FW */
	if (((pFrameInfo->common_frameinfo).pic_height == 1920) && ((pFrameInfo->common_frameinfo).pic_width == 1088))
	{
		(pFrameInfo->specific_frameinfo).frame_cropping_flag = 1;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_left_offset     = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_right_offset    = 4;//corrected value is 4 (!!SHOULD BE 4!!)
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_top_offset      = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_bottom_offset   = 0;
	}
	/* - change for CR 343589 */

#else

	if(((pFrameInfo->common_frameinfo).pic_height % 16)||((pFrameInfo->common_frameinfo).pic_width % 16))
	{
		(pFrameInfo->specific_frameinfo).frame_cropping_flag = 1;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_left_offset     = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_top_offset      = 0;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_right_offset    = (offset_to_next16_multiple(((pFrameInfo->common_frameinfo).pic_width % 16)))/2;
		(pFrameInfo->specific_frameinfo).frame_cropping_rect_bottom_offset	 = (offset_to_next16_multiple(((pFrameInfo->common_frameinfo).pic_height % 16)))/2;
		pProxyComponent->mParam.update_cropping_Params(pFrameInfo->specific_frameinfo);
	}

	(pFrameInfo->common_frameinfo).pic_height	= make16multiple((pFrameInfo->common_frameinfo).pic_height);
	(pFrameInfo->common_frameinfo).pic_width	= make16multiple((pFrameInfo->common_frameinfo).pic_width);
	(pFrameInfo->specific_frameinfo).sfw 		= make16multiple((pFrameInfo->specific_frameinfo).sfw);
	(pFrameInfo->specific_frameinfo).sfh 		= make16multiple((pFrameInfo->specific_frameinfo).sfh);

	pProxyComponent->setFrameSize(0,(pFrameInfo->common_frameinfo).pic_width,(pFrameInfo->common_frameinfo).pic_height);
	pProxyComponent->setFrameSize(1,(pFrameInfo->common_frameinfo).pic_width,(pFrameInfo->common_frameinfo).pic_height);
#endif
/* -Change for 372717 */

	if ((pFrameInfo->specific_frameinfo).ProfileIDC == 100)
	{
	        (pFrameInfo->specific_frameinfo).TransformMode      = 1;
    }
//- code for step 2 of CR 332873


    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_NmfHost_ProcessingComponent::codecRetrieveConfig");
}

OMX_BOOL H264Enc_NmfHost_ProcessingComponent::frameInfoDifferent(t_frameinfo *pFrameInfo1, t_frameinfo *pFrameInfo2)
{
    return OMX_TRUE;
}


EXPORT_C OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes,
        OMX_U8 **ppData,
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    switch (nPortIndex) {
    case 0:
        return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo, OMX_FALSE);
    case 1:
#ifdef _CACHE_OPT_
        return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo, OMX_TRUE);
#else
        return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo, OMX_FALSE);
#endif
    default:
        DBC_ASSERT(0==1);
        break;
    }
    return OMX_ErrorUndefined;
}



OMX_BOOL H264Enc_NmfHost_ProcessingComponent::isNMFPanicSource(t_panic_source ee_type, OMX_U32 faultingComponent){
	switch(ee_type)
	{
		case MPC_EE :
		OstTraceFiltInst0(TRACE_FLOW, "Checking for MPC_EE");
			if(mCodec->getMPCComponentHandle("osttracedsp")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cAdapter")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cResource_manager")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cDdep")==faultingComponent	||
			   mCodec->getMPCComponentHandle("cAlgo")==faultingComponent
			   )
			{
                if(mCodec->getMPCComponentHandle("osttracedsp")==faultingComponent){
                     OstTraceFiltInst0(TRACE_FLOW, "Panic::osttracedsp");
                }
                if(mCodec->getMPCComponentHandle("cAdapter")==faultingComponent){
                     OstTraceFiltInst0(TRACE_FLOW, "Panic::cAdapter");
                }
                if(mCodec->getMPCComponentHandle("cResource_manager")==faultingComponent){
                     OstTraceFiltInst0(TRACE_FLOW, "Panic::cResource_manager");
                }
                if(mCodec->getMPCComponentHandle("cDdep")==faultingComponent){
                     OstTraceFiltInst0(TRACE_FLOW, "Panic::cDdep");
                }
                if(mCodec->getMPCComponentHandle("cAlgo")==faultingComponent){
                     OstTraceFiltInst0(TRACE_FLOW, "Panic::cAlgo");
                }

				return OMX_TRUE;
			}
			else
			{
				return OMX_FALSE;
			}

            break;
		case HOST_EE :
		OstTraceFiltInst0(TRACE_FLOW, "Checking for HOSTEE");
			if((void*)mCodec == (void*)faultingComponent)
			{
                OstTraceFiltInst0(TRACE_FLOW, "Panic::ARM Component");
                return OMX_TRUE;
            }
			else
				return OMX_FALSE;
			break;
		default: return OMX_FALSE;
			break;

	}
}


OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::errorRecoveryDestroyAll(void)
{
    OstTraceInt0(TRACE_API,  "H264Enc: proxy_ddep : H264Enc_NmfHost_ProcessingComponent: errorRecoveryDestroyAll()");
    mCodec->stop();

    codecDeInstantiate();
    mCodec->destroy();
    codecDestroy();
    // deinstantiate generic interfaces
    unregisterStubsAndSkels();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_NmfHost_ProcessingComponent::configure(void)
{
    OstTraceInt0(TRACE_API,  "H264Enc: proxy_ddep : H264Enc_NmfHost_ProcessingComponent: configure()");
	RETURN_OMX_ERROR_IF_ERROR(VFM_NmfHost_ProcessingComponent::configure());
	H264Enc_Port *portInput = (H264Enc_Port *)pProxyComponent->getPort(VPB+0);
	OMX_VIDEO_PORTDEFINITIONTYPE *portdef_Input = portInput->getVideoPortDefinition();
	OMX_U32 OSWidth=0;
	OSWidth = (OMX_U32)(((float)(portdef_Input->nStride))/(portInput->getPixelDepth(portdef_Input->eColorFormat)));
    OstTraceInt2(TRACE_API,  "H264Enc: proxy_ddep : H264Enc_NmfHost_ProcessingComponent: OSWidth : %d OSHeight : %d",OSWidth,portdef_Input->nSliceHeight);
    /* +Change for 372717 */
	OSWidth = make16multiple(OSWidth);
	portdef_Input->nSliceHeight = make16multiple(portdef_Input->nSliceHeight);
	/* -Change for 372717 */
	mISetConfig.set_VideoStabParam(OSWidth,portdef_Input->nSliceHeight);
	OstTraceInt1(TRACE_API,  "H264Enc: proxy_ddep : H264Enc_NmfHost_ProcessingComponent: configure Value of Param : 0x%x \n",(OMX_U32)&(pProxyComponent->mParam));
	mISetConfig.set_param((void *)&(pProxyComponent->mParam));
    return OMX_ErrorNone;
}
