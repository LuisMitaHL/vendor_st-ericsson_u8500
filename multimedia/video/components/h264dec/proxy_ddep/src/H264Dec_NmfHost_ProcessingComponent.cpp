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
#include "H264Dec_NmfHost_ProcessingComponent.h"
#include "VFM_DDepUtility.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_proxy_ddep_src_H264Dec_NmfHost_ProcessingComponentTraces.h"
#endif

// Traces
//#ifdef PACKET_VIDEO_SUPPORT
//#undef LOG_TAG
//#define LOG_TAG "h264dec"
//#include<cutils/log.h>
//#define NMF_LOG LOGI
//#endif

class H264Dec_GetConfigCB : public h264dec_arm_nmf_api_get_configDescriptor
{
    friend class H264Dec_NmfHost_ProcessingComponent;
    public:
        H264Dec_GetConfigCB(H264Dec_NmfHost_ProcessingComponent *pProcessingComponent) : mProcessingComponent(pProcessingComponent) { }
        virtual ~H264Dec_GetConfigCB() {  };

    private:
        virtual void get_config(t_frameinfo frame_info) { mProcessingComponent->getConfigCB(&frame_info); };
        virtual void get_memory(VFM_MemoryStatus memory_status) { mProcessingComponent->getMemoryCB(&memory_status); };

        /// @brief Reference to the ENS component it belongs to
        H264Dec_NmfHost_ProcessingComponent *mProcessingComponent;
};

inline void H264Dec_NmfHost_ProcessingComponent::h264dec_assert(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceFiltInst1(TRACE_ERROR, "H264DEC: proxy_ddep: H264Dec_NmfHost_ProcessingComponent: h264dec_assert: error line %d\n", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecInstantiate()
{
    IN0("");

#ifdef NO_HAMAC
#else
    RETURN_XXX_IF_WRONG((mMpcCodec->construct()==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindAsynchronous("iTraceInitOut",4, mMpcCodec ,"iTraceInit") ==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindAsynchronous("iStartCodec",4, mMpcCodec ,"iStartCodecMpc") ==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mMpcCodec->bindAsynchronous("iEndCodecMpc",4, mCodec ,"iEndCodec") ==NMF_OK), OMX_ErrorUndefined);
#endif

    H264Dec_Proxy *pComponent = (H264Dec_Proxy *)&(mENSComponent);

    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport", pComponent->getMaxBuffers(0)));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport", pComponent->getMaxBuffers(1)));
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_config", 5, &mISetConfig)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_configuration", 5, &mISetConfiguration)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("error_recovery", 2, &mIErrorRecovery)==NMF_OK), OMX_ErrorUndefined);

    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    mGetConfigCB = new H264Dec_GetConfigCB(this);
    RETURN_XXX_IF_WRONG((mGetConfigCB!=0), OMX_ErrorInsufficientResources);
    RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, "get_config", mGetConfigCB, 5)==NMF_OK), OMX_ErrorUndefined);

    OUT0("");
    return OMX_ErrorNone;
}

/* + Changes for CR 399075 */
void H264Dec_NmfHost_ProcessingComponent::resetDecodeImmediateRelease()
 {
	H264Dec_Proxy *pComponent = (H264Dec_Proxy *)&(mENSComponent);
	mCodecFrameInfo.specific_frameinfo.bImmediateRelease = OMX_FALSE;
	pComponent->getParamAndConfig()->resetDecodeImmediateFlag();
}
/* - Changes for CR 399075 */

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecStart()
{
#ifdef NO_HAMAC
#else
    mMpcCodec->start();
#endif
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecStop()
{
#ifdef NO_HAMAC
#else
    mMpcCodec->stop();
#endif
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecConfigure()
{
#ifdef NO_HAMAC
#else
    // Retreive info from TraceBuilderMpc called at ENS_Component::contruct
    TraceInfo_t trace_info = *(mENSComponent.getSharedTraceInfoPtr());
    // Set OST trace infos to MPC, once it has started
    // MPC Interface call
#ifndef __ndk5500_a0__
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("iTraceInit", 5, &mITraceInit)==NMF_OK), OMX_ErrorUndefined);
    mITraceInit.traceInit(trace_info, getId1());
    VFM_NmfHost_ProcessingComponent_SENDCONFIGURATION;
#endif
#endif
    //NMF_LOG("[OST debug]: Id = 0x%x, parentHandle = 0x%08x, traceEnable = 0x%x\n", getId1(), trace_info.parentHandle, trace_info.traceEnable);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecDeInstantiate()
{
    mErrorRecoveryHappened = OMX_FALSE;
#ifdef NO_HAMAC
#else
    RETURN_XXX_IF_WRONG((mCodec->unbindAsynchronous("iTraceInitOut", mMpcCodec ,"iTraceInit")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindAsynchronous("iStartCodec", mMpcCodec ,"iStartCodecMpc")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mMpcCodec->unbindAsynchronous("iEndCodecMpc", mCodec ,"iEndCodec")==NMF_OK), OMX_ErrorUndefined);
#endif

    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_config")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_configuration")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("error_recovery")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "get_config")==NMF_OK), OMX_ErrorUndefined);
    h264dec_assert((mGetConfigCB!=0), __LINE__, OMX_TRUE);
    delete mGetConfigCB;
    mGetConfigCB = 0;

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));

#ifdef NO_HAMAC
#else
    // MPC OST trace
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("iTraceInit")==NMF_OK), OMX_ErrorUndefined);
#endif

    return OMX_ErrorNone;
}


void H264Dec_NmfHost_ProcessingComponent::registerStubsAndSkels()
{
#ifndef NO_HAMAC
#ifndef __ndk5500_a0__
//TODO CHANGE __ndk5500_a0__ for mmte use platform test instead! (-DMMTE_PLATFORM=$(PLATFORM))
    CM_REGISTER_STUBS_SKELS(video_h264dec_ddep_cpp);
    CM_REGISTER_SKELS(video_h264dec_h264dec);
#endif
#endif
}

void H264Dec_NmfHost_ProcessingComponent::unregisterStubsAndSkels()
{
#ifndef NO_HAMAC
#ifndef __ndk5500_a0__
    CM_UNREGISTER_STUBS_SKELS(video_h264dec_ddep_cpp);
    CM_UNREGISTER_SKELS(video_h264dec_h264dec);
#endif
#endif
}

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
#ifdef NO_HAMAC
	mCodec = h264dec_arm_nmf_proxynmf_swCreate();
    if (mCodec) {
        // no domainId usage in the sw version
        return OMX_ErrorNone;
    }
#else
    mCodec = h264dec_arm_nmf_proxynmfCreate();
#ifndef __ndk5500_a0__
	mMpcCodec = h264dec_mpc_composite_h264decmpcCreate();
    if ((mCodec!=NULL)&&(mMpcCodec!=NULL)) {
        ((h264dec_mpc_composite_h264decmpc *)mMpcCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
#endif
        return OMX_ErrorNone;
    }
#endif

    return OMX_ErrorInsufficientResources;
}

void H264Dec_NmfHost_ProcessingComponent::codecDestroy(void)
{
#ifdef NO_HAMAC
    h264dec_arm_nmf_proxynmf_swDestroy((H264_COMPONENT *&)mCodec);
#else
    mMpcCodec->destroy();
    h264dec_arm_nmf_proxynmfDestroy((H264_COMPONENT *&)mCodec);
    h264dec_mpc_composite_h264decmpcDestroy((h264dec_mpc_composite_h264decmpc *&)mMpcCodec);
#endif
}

void H264Dec_NmfHost_ProcessingComponent::getConfigCB(t_frameinfo *pFrameInfo)
{
      H264Dec_Proxy *pProxy = (H264Dec_Proxy *)&(mENSComponent);
     // save what the codec knows
    mCodecFrameInfo = *pFrameInfo;
// +CR324558 CHANGE START FOR
	/*CR324558: Generate portsettings changed event only once for a port*/
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    OMX_U32 nSupportedExtension = pComponent->getParamAndConfig()->getSupportedExtension();
    OMX_BOOL changedSize;
    if ((nSupportedExtension & VFM_SUPPORTEDEXTENSION_SIZE) &&
	            ((pFrameInfo->common_frameinfo.pic_width != pComponent->getFrameWidth(0)) ||
             (pFrameInfo->common_frameinfo.pic_height != pComponent->getFrameHeight(0))))
    {
        changedSize = OMX_TRUE;

        //We check now whether the resolution that was set earlier, using the
        //	container info (pParam->CropWidth) is greater than the info found
        // in the elementary stream. In that case, we cannot consider the old
        // resolution as a crop resolution because crop vectors would go out-
        // side the frame. So we update the width/height with the new width/
        // height found in the elementary stream.
        // Also, in the unlikely case that only one the width or height is
        // invalid, then the following code also takes care of the fact that
        // only that value will be overriden and not the other.
        // So, effectively, we are setting here the cropping vectors to the
        // smaller of the two values, for both width and height.
        if (pProxy->mParam.CropWidth > pFrameInfo->common_frameinfo.pic_width)
        {
            pProxy->mParam.CropWidth = pFrameInfo->common_frameinfo.pic_width;
        }

        if (pProxy->mParam.CropHeight > pFrameInfo->common_frameinfo.pic_height)
        {
            pProxy->mParam.CropHeight = pFrameInfo->common_frameinfo.pic_height;
        }
    }
    else
    {
        changedSize = OMX_FALSE;
    }
// -CR324558 CHANGE END OF

    // Update common arguments
    VFM_getConfigCB(&(mCodecFrameInfo.common_frameinfo));

    // specific_frameinfo to be added

// +CR324558 CHANGE START FOR
	if(nSupportedExtension & VFM_SUPPORTEDEXTENSION_DPBSIZE) {
        H264Dec_Proxy *pProxy = (H264Dec_Proxy *)&(mENSComponent);
        VFM_Port *pOutputPort=((VFM_Port *)(pComponent->getPort(1)));
        OMX_PARAM_PORTDEFINITIONTYPE *paramPort = pOutputPort->getParamPortDefinition();
        OMX_U16 curDPBSize = pProxy->subtractExtraPipeBuffers(paramPort->nBufferCountMin);
        OstTraceFiltInst2(TRACE_FLOW,"\nH264Dec_NmfHost_ProcessingComponent::getConfigCB pFrameInfo->common_frameinfo.dpb_size=%d,curDPBSize=%d",pFrameInfo->common_frameinfo.dpb_size, curDPBSize);
        if(pFrameInfo->common_frameinfo.dpb_size!=curDPBSize){
			paramPort->nBufferCountMin = pFrameInfo->common_frameinfo.dpb_size;
            paramPort->nBufferCountMin = pProxy->addExtraPipeBuffers(paramPort->nBufferCountMin);
            if(!changedSize) {
                eventHandler(OMX_EventPortSettingsChanged, 1, OMX_IndexParamPortDefinition);
            }
        }
    }
// -CR324558 CHANGE END OF
}

void H264Dec_NmfHost_ProcessingComponent::sendConfigToCodec()
{
    VFM_NmfHost_ProcessingComponent_SENDCONFIGTOCODEC;
}

void H264Dec_NmfHost_ProcessingComponent::codecRetrieveConfig(t_frameinfo *pFrameInfo)
{
    memset(pFrameInfo, 0, sizeof(t_frameinfo));

    VFM_retrieveConfig(&pFrameInfo->common_frameinfo);

    // specific_frameinfo to be added
    H264Dec_Proxy *pComponent = (H264Dec_Proxy *)&(mENSComponent);

// +CR324558 CHANGE START FOR
    VFM_Port *pPort = ((VFM_Port *)(pComponent->getPort(1)));
    t_specific_frameinfo *pt_specific = (t_specific_frameinfo *)(&pFrameInfo->specific_frameinfo);

    OMX_PARAM_PORTDEFINITIONTYPE *paramPort = pPort->getParamPortDefinition();
    pFrameInfo->common_frameinfo.dpb_size = pComponent->subtractExtraPipeBuffers(paramPort->nBufferCountMin);
    OstTraceInt1(TRACE_FLOW,"\nH264Dec_NmfHost_ProcessingComponent::codecRetrieveConfig pFrameInfo->common_frameinfo.dpb_size=%d",pFrameInfo->common_frameinfo.dpb_size);
// -CR324558 CHANGE END OF

    if (!pComponent->mParam.isDeblockingOn()) {
        pt_specific->nParallelDeblocking = 0;
    } else if (pComponent->mParam.getParallelDeblocking()) {
        pt_specific->nParallelDeblocking = 3;
    } else {
        pt_specific->nParallelDeblocking = 1;
    }
    pt_specific->pErrorMap = (t_uint8 *)pComponent->mParam.getErrorMap();
    pt_specific->xFramerate = mENSComponent.getPort(1)->getVideoPortDefinition()->xFramerate;
    pt_specific->bThumbnailGeneration = pComponent->getParamAndConfig()->getThumbnailGeneration();
    pt_specific->bImmediateRelease = pComponent->getParamAndConfig()->getImmediateRelease();

    pt_specific->restrictMaxLevel = pComponent->mParam.getLevelCapabilities();
}

OMX_BOOL H264Dec_NmfHost_ProcessingComponent::frameInfoDifferent(t_frameinfo *pFrameInfo1, t_frameinfo *pFrameInfo2)
{
    _VFM_CHECKDIFF_(VFM_frameInfoDifferent(&pFrameInfo1->common_frameinfo, &pFrameInfo2->common_frameinfo));

    // specific_frameinfo to be added
    _VFM_CHECKDIFF_(pFrameInfo1->specific_frameinfo.nParallelDeblocking != pFrameInfo2->specific_frameinfo.nParallelDeblocking);
    _VFM_CHECKDIFF_(pFrameInfo1->specific_frameinfo.xFramerate != pFrameInfo2->specific_frameinfo.xFramerate);
     _VFM_CHECKDIFF_(pFrameInfo1->specific_frameinfo.bImmediateRelease != pFrameInfo2->specific_frameinfo.bImmediateRelease);
// +CR324558 CHANGE START FOR
    OstTraceInt1(TRACE_FLOW,"\nH264Dec_NmfHost_ProcessingComponent::frameInfoDifferent diff=%d",(pFrameInfo1->common_frameinfo.dpb_size!=pFrameInfo2->common_frameinfo.dpb_size));
    _VFM_CHECKDIFF_( pFrameInfo1->common_frameinfo.dpb_size!=pFrameInfo2->common_frameinfo.dpb_size);
// -CR324558 CHANGE END OF
    return OMX_FALSE;
}

EXPORT_C OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes,
        OMX_U8 **ppData,
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
	H264Dec_Proxy *pProxyComponent = (H264Dec_Proxy *)&mENSComponent;
	VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

    switch (nPortIndex) {
    case 0:
    case 1:
#ifdef _CACHE_OPT_
        return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo, OMX_TRUE);
#else
        return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo, OMX_FALSE);
#endif
    default:
        h264dec_assert((0==1),  __LINE__, OMX_TRUE);
        break;
    }
    return OMX_ErrorUndefined;
}

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    // H264Dec may changes NalUnitFormat into StartCode
    H264Dec_Proxy *pComponent = (H264Dec_Proxy *)&(mENSComponent);
    if (pComponent->mParam.getNalFormat() == OMX_NaluFormatFourByteInterleavedLength) {
        unsigned char *pt = pBuffer->pBuffer;
        unsigned int i=0;
        while (i+3<pBuffer->nFilledLen) {
            unsigned int len = (pt[3]<<24) + (pt[2]<<16) + (pt[1]<<8) + pt[0];//Change putting parenthesis to remove warning
            pt[0] = pt[1] = pt[2] = 0;
            pt[3] = 1;
            pt += len+4;
            i += len+4;
        }
        if (i!=pBuffer->nFilledLen) {
            OstTraceInt0(TRACE_WARNING, "H264Dec: H264Dec_NmfHost_ProcessingComponent: emptyThisBuffer: Nal Unit Format is not regulat\n");
            return OMX_ErrorBadParameter;
        }
    }
    return VFM_NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    // H264Dec requires MMHWBuffers on output
    if (!pBuffer->pOutputPortPrivate) {
        OstTraceInt0(TRACE_WARNING, "H264Dec: H264Dec_NmfHost_ProcessingComponent: fillThisBuffer: Physical information is not there\n");
        return OMX_ErrorBadParameter;
    }
    return VFM_NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}


/*
  General error recovery: destroy the full NMF network, ARM and DSP
*/
OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::errorRecoveryDestroyAll(void)
{
    //NMF_LOG("H264Dec_NmfHost_ProcessingComponent::errorRecoveryDestroyAll\n");
    mCodec->stop();
#ifdef NO_HAMAC
#else
    mMpcCodec->stop();
#endif

    codecDeInstantiate();
    EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "proxy");
    mCodec->unbindFromUser("sendcommand");
    mCodec->unbindFromUser("fsminit");

    mCodec->destroy();
#ifdef NO_HAMAC
    h264dec_arm_nmf_proxynmf_swDestroy((H264_COMPONENT *&)mCodec);
#else
    mMpcCodec->destroy();
    h264dec_mpc_composite_h264decmpcDestroy((h264dec_mpc_composite_h264decmpc *&)mMpcCodec);
    h264dec_arm_nmf_proxynmfDestroy((H264_COMPONENT *&)mCodec);
#endif

    // deinstantiate generic interfaces
    unregisterStubsAndSkels();
    return OMX_ErrorNone;
}

/*
  Error recovery step 1: destroy MPC components if a panic happened on DSP
  */
OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::errorRecoveryKillMpc(t_cm_domain_id & aMpcCodecDomainId)
{
    //NMF_LOG("H264Dec_NmfHost_ProcessingComponent::errorRecoveryKillMpc\n");
#ifdef NO_HAMAC
#else
    aMpcCodecDomainId = ((h264dec_mpc_composite_h264decmpc *)mMpcCodec)->domainSVACodec;
    mMpcCodec->stop();
    RETURN_XXX_IF_WRONG((mCodec->unbindAsynchronous("iTraceInitOut", mMpcCodec ,"iTraceInit")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindAsynchronous("iStartCodec", mMpcCodec ,"iStartCodecMpc")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mMpcCodec->unbindAsynchronous("iEndCodecMpc", mCodec ,"iEndCodec")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mMpcCodec->destroy()==NMF_OK), OMX_ErrorUndefined);
    h264dec_mpc_composite_h264decmpcDestroy((h264dec_mpc_composite_h264decmpc *&)mMpcCodec);
#endif
    mErrorRecoveryHappened = OMX_TRUE;
    return OMX_ErrorNone;
}

/*
  Error recovery step 2: restart MPC components and continue !!!
  */
OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::errorRecoveryRestartMpc(t_cm_domain_id aMpcCodecDomainId)
{
    //NMF_LOG("H264Dec_NmfHost_ProcessingComponent::errorRecoveryRestartMpc\n");
    if((mErrorRecoveryHappened == OMX_FALSE) || (mCodec==NULL) || (mMpcCodec!=NULL))
      return OMX_ErrorNone;

    mErrorRecoveryHappened = OMX_FALSE;
#ifdef NO_HAMAC
#else
   	mMpcCodec = h264dec_mpc_composite_h264decmpcCreate();
    ((h264dec_mpc_composite_h264decmpc *)mMpcCodec)->domainSVACodec = aMpcCodecDomainId;
    RETURN_XXX_IF_WRONG((mMpcCodec->construct()==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindAsynchronous("iTraceInitOut",4, mMpcCodec ,"iTraceInit")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindAsynchronous("iStartCodec",4, mMpcCodec ,"iStartCodecMpc")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mMpcCodec->bindAsynchronous("iEndCodecMpc",4, mCodec ,"iEndCodec")==NMF_OK), OMX_ErrorUndefined);
    mMpcCodec->start();
#endif
    mIErrorRecovery.error_recovery();
    return OMX_ErrorNone;
}


// +CR324558 CHANGE START FOR
OMX_U32 H264Dec_NmfHost_ProcessingComponent::getInternalDPBSize(){
    return mCodecFrameInfo.common_frameinfo.dpb_size;
}

void H264Dec_NmfHost_ProcessingComponent::setInternalDPBSize(OMX_U32 dpbSize){
    mCodecFrameInfo.common_frameinfo.dpb_size = dpbSize;
}


EXPORT_C OMX_ERRORTYPE H264Dec_NmfHost_ProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler)
{
    switch(event) {
        case OMX_EventCmdComplete:
            {
                switch(nData1)
                {
                    case OMX_CommandStateSet:
                        switch(nData2)
                        {
                            case OMX_StateLoaded:
                            case OMX_StateIdle:
                                    setInternalDPBSize(0);
                                    break;
                            case OMX_StateExecuting:
                            case OMX_StatePause:
                                    break;
                        }
                        break;
                    case OMX_CommandPortDisable:
                        {
                            if(nData2==0) {
                                setInternalDPBSize(0);
                            }
                        }
                        break;
                    case OMX_CommandPortEnable:
                    case OMX_CommandFlush:
                        break;
                    default:
                        break;
                }
            }
            break;
        case OMX_EventPortSettingsChanged:
            break;
        default:
            break;
    }
    return VFM_NmfHost_ProcessingComponent::doSpecificEventHandler_cb(event,nData1,nData2,deferEventHandler);
}


// -CR324558 CHANGE END OF