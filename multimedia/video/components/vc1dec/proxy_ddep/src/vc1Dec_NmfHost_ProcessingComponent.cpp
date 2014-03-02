/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Contains functions that are called to instantiate / deinstantiate
 *          the Processing Component associated to the OMX component vc1dec.
 * \author  ST-Ericsson
 */


#define _CNAME_ vc1Dec_Proxy
#include "osi_trace.h"
#include "vc1Dec_Proxy.h"
#include "vc1Dec_NmfHost_ProcessingComponent.h"
#include "vc1Dec_Port.h"
#include "VFM_DDepUtility.h"

// include vc1dec arm nmf component interfaces
#include <api/vc1dec.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_proxy_ddep_src_vc1Dec_NmfHost_ProcessingComponentTraces.h"
#endif

class vc1Dec_GetConfigCB : public vc1dec_arm_nmf_api_get_configDescriptor
{
    friend class vc1Dec_NmfHost_ProcessingComponent;
    public:
        vc1Dec_GetConfigCB(vc1Dec_NmfHost_ProcessingComponent *pProcessingComponent) : mProcessingComponent(pProcessingComponent) { }

    private:
        virtual void get_config(t_frameinfo frame_info) { mProcessingComponent->getConfigCB(&frame_info); };

        /// @brief Reference to the ENS component it belongs to
        vc1Dec_NmfHost_ProcessingComponent *mProcessingComponent;
};


OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecInstantiate()
{
    IN0("");

    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecInstantiate()\n");
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_config", 1, &mISetConfig)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("set_configuration", 1, &mISetConfiguration)==NMF_OK), OMX_ErrorUndefined);

    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    mGetConfigCB = new vc1Dec_GetConfigCB(this);
    RETURN_XXX_IF_WRONG((mGetConfigCB!=0), OMX_ErrorInsufficientResources);
    RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, "get_config", mGetConfigCB, 1)==NMF_OK), OMX_ErrorUndefined);

    OUT0("");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecStart()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecStart()\n");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecStop()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecStop()\n");
    mCodec->stop();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecConfigure()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecConfigure()\n");
    VFM_NmfHost_ProcessingComponent_SENDCONFIGURATION;
    //sendConfigToCodec();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecDeInstantiate()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecDeInstantiate()\n");
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_config")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("set_configuration")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "get_config")==NMF_OK), OMX_ErrorUndefined);
    DBC_ASSERT(mGetConfigCB);
    delete mGetConfigCB;
    mGetConfigCB = 0;

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));
    return OMX_ErrorNone;
}

/*void vc1Dec_NmfHost_ProcessingComponent::updateEsram()
{
    //OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: updateEsram()\n");
    VFM_Component *pComp = &((VFM_Component &)mENSComponent);
    mISetConfig.updateDomain(pComp->getAllocDdrMemoryDomain(), pComp->getAllocEsramMemoryDomain());
}*/
 /* +ER 341788 CHANGE START FOR */
OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::errorRecoveryDestroyAll(void)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: errorRecoveryDestroyAll()\n");
   // NMF_LOG("\n INSIDE vc1Dec_NmfHost_ProcessingComponent::errorRecoveryDestroyAll \n");
    mISetConfig.flush();
    mCodec->stop();

    codecDeInstantiate();
    EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "proxy");
    mCodec->unbindFromUser("sendcommand");
    mCodec->unbindFromUser("fsminit");

    mCodec->destroy();

#ifdef NO_HAMAC
    vc1dec_arm_nmf_proxynmf_swDestroy((VC1_COMPONENT *&)mCodec);
#else
    vc1dec_arm_nmf_proxynmfDestroy((VC1_COMPONENT *&)mCodec);
#endif

    // deinstantiate generic interfaces
    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

 /* +ER 341788 CHANGE START FOR */
void vc1Dec_NmfHost_ProcessingComponent::registerStubsAndSkels()
{
#ifndef NO_HAMAC
    CM_REGISTER_STUBS_SKELS(video_vc1dec_ddep_cpp);
    CM_REGISTER_SKELS(video_vc1dec_vc1dec);
#endif
}

void vc1Dec_NmfHost_ProcessingComponent::unregisterStubsAndSkels()
{
#ifndef NO_HAMAC
    CM_UNREGISTER_STUBS_SKELS(video_vc1dec_ddep_cpp);
    CM_UNREGISTER_SKELS(video_vc1dec_vc1dec);
#endif
}

OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecCreate()\n");
#ifdef NO_HAMAC
	mCodec = vc1dec_arm_nmf_proxynmf_swCreate();
    if (mCodec) {
        // no domainId usage in the sw version
        return OMX_ErrorNone;
    }
#else
	mCodec = vc1dec_arm_nmf_proxynmfCreate();
    if (mCodec) {
        ((VC1_COMPONENT *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
        return OMX_ErrorNone;
    }
#endif
 OstTraceInt0(TRACE_ERROR,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecCreate -> OMX_ErrorInsufficientResources");
    return OMX_ErrorInsufficientResources;
}

void vc1Dec_NmfHost_ProcessingComponent::codecDestroy(void)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecDestroy()\n");
#ifdef NO_HAMAC
    vc1dec_arm_nmf_proxynmf_swDestroy((VC1_COMPONENT *&)mCodec);
#else
    vc1dec_arm_nmf_proxynmfDestroy((VC1_COMPONENT *&)mCodec);
#endif
}

void vc1Dec_NmfHost_ProcessingComponent::getConfigCB(t_frameinfo *pFrameInfo)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: getConfigCB()\n");
    // save what the codec knows
    mCodecFrameInfo = *pFrameInfo;

    // Update common arguments
    VFM_getConfigCB(&(mCodecFrameInfo.common_frameinfo));

    // specific_frameinfo to be added
}

void vc1Dec_NmfHost_ProcessingComponent::sendConfigToCodec()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: sendConfigToCodec()\n");
    VFM_NmfHost_ProcessingComponent_SENDCONFIGTOCODEC;
}

void vc1Dec_NmfHost_ProcessingComponent::codecRetrieveConfig(t_frameinfo *pFrameInfo)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecRetrieveConfig()\n");
    memset(pFrameInfo, 0, sizeof(t_frameinfo));

    VFM_retrieveConfig(&pFrameInfo->common_frameinfo);
    // specific_frameinfo to be added
    //+ CR 399075 CHANGE START FOR
	vc1Dec_Proxy *pComponent = (vc1Dec_Proxy *)&(mENSComponent);
    pFrameInfo->specific_frameinfo.bImmediateRelease = pComponent->getParamAndConfig()->getImmediateRelease();
    OstTraceInt1(TRACE_FLOW,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: codecRetrieveConfig() Value of mCodecFrameInfo.specific_frameinfo.bImmediateRelease : %d ",(OMX_U32)mCodecFrameInfo.specific_frameinfo.bImmediateRelease);
  // -CR399075 CHANGE END OF
    OUT0("");
}

OMX_BOOL vc1Dec_NmfHost_ProcessingComponent::frameInfoDifferent(t_frameinfo *pFrameInfo1, t_frameinfo *pFrameInfo2)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: frameInfoDifferent()\n");
    _VFM_CHECKDIFF_(VFM_frameInfoDifferent(&pFrameInfo1->common_frameinfo, &pFrameInfo2->common_frameinfo));

    // specific_frameinfo to be added
  _VFM_CHECKDIFF_(pFrameInfo1->specific_frameinfo.bImmediateRelease != pFrameInfo2->specific_frameinfo.bImmediateRelease);
    return OMX_FALSE;
}

EXPORT_C OMX_ERRORTYPE vc1Dec_NmfHost_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes,
        OMX_U8 **ppData,
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: allocateBuffer()\n");
    switch (nPortIndex) {
    case 0:
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
     OstTraceInt0(TRACE_ERROR,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: allocateBuffer() OMX_ErrorUndefined\n");
    return OMX_ErrorUndefined;
}

 void vc1Dec_NmfHost_ProcessingComponent:: resetDecodeImmediateRelease()
{
	vc1Dec_Proxy *pComponent = (vc1Dec_Proxy *)&(mENSComponent);
   	NMF_LOG("\nABC\n");
	mCodecFrameInfo.specific_frameinfo.bImmediateRelease = OMX_FALSE;
	pComponent->getParamAndConfig()->resetDecodeImmediateFlag();
	OstTraceInt0(TRACE_FLOW,  "VC1Dec: proxy_ddep : vc1Dec_NmfHost_ProcessingComponent: resetDecodeImmediateRelease()  \n");
}
