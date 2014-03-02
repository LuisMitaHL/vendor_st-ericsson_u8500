/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ VFM

#include "ENS_Component.h"
#include "VFM_Component.h"
#include "VFM_Port.h"
#include "VFM_NmfHost_ProcessingComponent.h"
#include "eventhandler.hpp"
#include "VFM_DDepUtility.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_NmfHost_ProcessingComponentTraces.h"
#endif

// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

#ifdef ANDROID
#define PRINTPG
#else
#define PRINTPG
#endif


inline void VFM_NmfHost_ProcessingComponent::vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        OstTraceInt3(TRACE_ERROR, "VFM: VFM_NmfHost_ProcessingComponent: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

EXPORT_C VFM_NmfHost_ProcessingComponent::VFM_NmfHost_ProcessingComponent(ENS_Component &enscomp):
    NmfHost_ProcessingComponent(enscomp), mCodec(0),
    mCodecHasConfig(OMX_FALSE), mCodecIsInstantiated(OMX_FALSE), mGoToLoaded(OMX_FALSE)
{
    memset(&mMemoryStatus, 0, sizeof(mMemoryStatus));
}

EXPORT_C VFM_NmfHost_ProcessingComponent::~VFM_NmfHost_ProcessingComponent()
{
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::setPortCount(OMX_U32 nPortCount)
{
    IN0("");
    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::instantiate()
{
    registerStubsAndSkels();
    RETURN_OMX_ERROR_IF_ERROR(codecCreate(((VFM_Component &)mENSComponent).getNmfComponentDomain()));
    RETURN_XXX_IF_WRONG((mCodec->construct()==NMF_OK), OMX_ErrorUndefined);

    // instantiate the generic bindings
    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("fsminit", 1, &mIfsmInit)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("sendcommand", 2, &mIsendCommand)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, "proxy", (eventhandlerDescriptor*)this, 4)==NMF_OK), OMX_ErrorUndefined);

    // instantiate the specific interfaces of the component
    RETURN_OMX_ERROR_IF_ERROR(codecInstantiate());

    mCodecIsInstantiated = OMX_TRUE;
    mCodecHasConfig = OMX_FALSE;
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::start()
{
    mCodec->start();
    RETURN_OMX_ERROR_IF_ERROR(codecStart());
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::configure()
{
    RETURN_OMX_ERROR_IF_ERROR(codecConfigure());
    sendConfigToCodec();
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::stop()
{
	/* replacing stop() call with stop_flush to avoid syncronization issues
	   where we have pending commands in nmf queue and we are stopping the component */
    mCodec->stop_flush();
    RETURN_OMX_ERROR_IF_ERROR(codecStop());
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::deInstantiate()
{
	VFM_Component *pComponent = (&(VFM_Component &)mENSComponent);
	OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();
    mCodecIsInstantiated = OMX_FALSE;
    mCodecHasConfig = OMX_FALSE;

    RETURN_OMX_ERROR_IF_ERROR(codecDeInstantiate());

    /* +Change start from CR 399938 */
//	if (isMetaDataUsed && (pComponent->isEncoder()))
//	{
//		pComponent->pParam->destroyMetaDataList();
//	}
	/* -Change end from CR 399938 */


    RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, "proxy")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("sendcommand")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("fsminit")==NMF_OK), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG((mCodec->destroy()==NMF_OK), OMX_ErrorUndefined);
    codecDestroy();

    // deinstantiate generic interfaces
    unregisterStubsAndSkels();

    /* +Change start from CR 399938 / ER 410767 */
	if (isMetaDataUsed && (pComponent->isEncoder()))
	{
		pComponent->pParam->destroyMetaDataList();
	}
	/* -Change end from CR 399938 / ER 410767 */

    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::createPortInterface(VFM_Port *port, const char *nameInput, const char *nameCB, OMX_U32 given_fifo_size)
{
    OMX_DIRTYPE direction = port->getDirection();
    OMX_U32 fifo_size = port->getBufferCountActual();
    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    if (given_fifo_size != 0) {
        if (given_fifo_size < fifo_size) {
            vfm_assert_static((0==1), __LINE__, OMX_FALSE, given_fifo_size, fifo_size);
        } else {
            fifo_size = given_fifo_size;
        }
    }
    if (direction == OMX_DirInput) {
        RETURN_XXX_IF_WRONG((mCodec->bindFromUser(nameInput, fifo_size, getEmptyThisBufferInterface(port->getPortIndex()))==NMF_OK), OMX_ErrorInsufficientResources);
        RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, nameCB, getEmptyBufferDoneCB(), fifo_size)==NMF_OK), OMX_ErrorInsufficientResources);
    } else {
        RETURN_XXX_IF_WRONG((mCodec->bindFromUser(nameInput, fifo_size, getFillThisBufferInterface(port->getPortIndex()))==NMF_OK), OMX_ErrorInsufficientResources);
        RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, nameCB, getFillBufferDoneCB(), fifo_size)==NMF_OK), OMX_ErrorInsufficientResources);
    }
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::destroyPortInterface(VFM_Port *port, const char *nameInput, const char *nameCB)
{
    RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec, nameCB)==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser(nameInput)==NMF_OK), OMX_ErrorUndefined) ;
    return OMX_ErrorNone;
}



EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes,
        OMX_U8 **ppData,
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    // checks the maximum number of buffers
    VFM_Component   *vfm_component = (&(VFM_Component &)mENSComponent);
    OMX_S32 maxBuffers = vfm_component->getMaxBuffers(nPortIndex);
    if (maxBuffers>0) {
        if (((VFM_Port *)(vfm_component->getPort(nPortIndex)))->getParamPortDefinition()->nBufferCountActual
                > (OMX_U32)maxBuffers) {
            return OMX_ErrorBadParameter;
        }
    }

    return allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBufferHdr)
{
    return useBufferHeaderVisual(dir, pBufferHdr);
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::freeBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated,
        void *bufferAllocInfo,
        void *portPrivateInfo)
{
    /* +Change start for CR332521 IOMX UseBuffer */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

    /* +Changes start for CR 399938 */
    OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();
    if (isMetaDataUsed && pComponent->isEncoder() && (0 == nPortIndex))
    {
		PRINTPG("In use buffer and for metaData so DO NOTHING HERER \n");
		return OMX_ErrorNone;
	}
	PRINTPG("In use buffer and NORMAL PROCESSING \n");
	/* -Changes end for CR 399938 */

    iOMXType *piOMX = pComponent->pParam->getiOMX();
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    piOMX->free(nPortIndex, nBufferIndex);
    /* -Change end for CR332521 IOMX UseBuffer */

    return freeBufferVisual(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
}


EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::useBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BUFFERHEADERTYPE* pBufferHdr,
        void **portPrivateInfo)
{
    /* +Change start for CR332521 IOMX UseBuffer */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

    /* +Changes start for CR 399938 */
    OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();
    if (isMetaDataUsed && pComponent->isEncoder() && (0 == nPortIndex))
    {
		*portPrivateInfo = 0;
		PRINTPG("In use buffer and for metaData so DO NOTHING HERER \n");
		return OMX_ErrorNone;
	}
	PRINTPG("In use buffer and NORMAL PROCESSING \n");
	/* -Changes end for CR 399938 */


    iOMXType *piOMX = pComponent->pParam->getiOMX();
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (OMX_ErrorNone != piOMX->allocate(pComponent, sharedChunk, nPortIndex, nBufferIndex, pBufferHdr->nAllocLen, pBufferHdr)) {
        return OMX_ErrorInsufficientResources;
    }
    /* -Change end for CR332521 IOMX UseBuffer */

   return useBufferVisual(nPortIndex, nBufferIndex, pBufferHdr, portPrivateInfo);
}

EXPORT_C void VFM_NmfHost_ProcessingComponent::VFM_getConfigCB(t_common_frameinfo *pFrameInfo)
{
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    OMX_U32 nSupportedExtension = pComponent->pParam->getSupportedExtension();
    vfm_assert_static((pComponent->isDecoder()), __LINE__, OMX_TRUE);

    // Profile and Level: no event is sent!
    OMX_U32 profile, level;
    pComponent->pParam->getProfileLevel(&profile, &level);
    if (profile!=pFrameInfo->eProfile || level!=pFrameInfo->eLevel) {
        pComponent->pParam->setProfileLevel(pFrameInfo->eProfile, pFrameInfo->eLevel);
    }

    // PAR
    if (nSupportedExtension & VFM_SUPPORTEDEXTENSION_PAR) {
        OMX_U32 parWidth, parHeight;
        pComponent->pParam->getPAR(&parWidth, &parHeight);
        if (parWidth!=pFrameInfo->nPixelAspectRatioWidth || parHeight!=pFrameInfo->nPixelAspectRatioHeight) {
            pComponent->pParam->setPAR(pFrameInfo->nPixelAspectRatioWidth, pFrameInfo->nPixelAspectRatioHeight);
            eventHandler(OMX_EventPortSettingsChanged, 1, OMX_IndexParamPixelAspectRatio);
        }
    }

    // Color Primary
    if (nSupportedExtension & VFM_SUPPORTEDEXTENSION_PRIMARYCOLOR) {
        OMX_COLORPRIMARYTYPE colorprimary = pComponent->pParam->getColorPrimary();
        if (pFrameInfo->nColorPrimary!=colorprimary) {
            pComponent->pParam->setColorPrimary((OMX_COLORPRIMARYTYPE)pFrameInfo->nColorPrimary);
        }
    }

    // Frame size, with slaving from master port to the slaved port
    if (nSupportedExtension & VFM_SUPPORTEDEXTENSION_SIZE) {
        if ((pFrameInfo->pic_width != pComponent->getFrameWidth(0)) ||
            (pFrameInfo->pic_height != pComponent->getFrameHeight(0))) {
                OMX_ERRORTYPE omxerror;
                VFM_Port *pMasterPort = ((VFM_Port *)(pComponent->getPort(0)))->getMasterPort();
                OMX_PARAM_PORTDEFINITIONTYPE local_param = *(pMasterPort->getParamPortDefinition());
                local_param.format.video.nFrameWidth = pFrameInfo->pic_width;
                local_param.format.video.nFrameHeight = pFrameInfo->pic_height;
                omxerror = ((ENS_Port *)pMasterPort)->setParameter(OMX_IndexParamPortDefinition, &local_param);
                if ( omxerror != OMX_ErrorNone){
                       OstTraceInt0(TRACE_ERROR,"VFM::VFM_getConfigCB  setParameter returned ERROR when portSettingChanged is detected");
                }
        }
    }

    // Crop
    if (nSupportedExtension & VFM_SUPPORTEDEXTENSION_CROP) {
        if (pComponent->pParam->isChangedCropping_And_Set(pFrameInfo)) {
            eventHandler(OMX_EventPortSettingsChanged, 1, OMX_IndexConfigCommonOutputCrop);
        }
    }
}

EXPORT_C void VFM_NmfHost_ProcessingComponent::VFM_retrieveConfig(t_common_frameinfo *pFrameInfo)
{
    VFM_Component *comp = (VFM_Component *)(&mENSComponent);

    /* Restore the normal debug mode, i.e. DBG_MODE_NORMAL */
    OMX_U32 profile, level;
    comp->pParam->getProfileLevel(&profile, &level);

    pFrameInfo->pic_width = comp->getFrameWidth(0);
    pFrameInfo->pic_height = comp->getFrameHeight(0);
    pFrameInfo->eProfile = profile;
    pFrameInfo->eLevel = level;
    pFrameInfo->nColorPrimary = comp->pParam->getColorPrimary();

    OMX_U32 parWidth, parHeight;
    comp->pParam->getPAR(&parWidth, &parHeight);
    pFrameInfo->nPixelAspectRatioWidth = parWidth;
    pFrameInfo->nPixelAspectRatioHeight = parHeight;

    comp->pParam->setCroppingInFrameinfo(pFrameInfo);

    // Get the supported extension
    pFrameInfo->nSupportedExtension = comp->pParam->getSupportedExtension();
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure)
{
    sendConfigToCodec();
    return OMX_ErrorNone;
};

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    bDeferredCmd = OMX_FALSE;
    /* +Change start for CR333616 Delay recycling */
    /* +Change start for CR330198 Revert to DTS from PTS */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
    RecyclingDelayType *pRecyclingDelay = pComponent->pParam->getRecyclingDelay();
    OMX_BUFFERHEADERTYPE *pBuffer;
    /* -Change end for CR330198 Revert to DTS from PTS */
    /* -Change end for CR333616 Delay recycling */

    switch (eCmd) {
    case OMX_CommandStateSet:
        mGoToLoaded = ((nData==OMX_StateLoaded) ? OMX_TRUE : OMX_FALSE);
        /* +Change start for CR333616 Delay recycling */
        if (nData==OMX_StateIdle) {
            pDisplayOrder->nextSaveMustReset();
            while (0 != (pBuffer=pRecyclingDelay->flushOneBuffer())) {
                pBuffer->nFilledLen=0;
                NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
            }
        }
        /* -Change end for CR333616 Delay recycling */
        break;

    /* +Change start for CR330198 Revert to DTS from PTS */
    case OMX_CommandFlush:
    case OMX_CommandPortDisable:
        if (eCmd==OMX_CommandFlush) {
            // nothing to do on disable as in this case, buffers that
            // have been sent should be processed anyway
            pDisplayOrder->nextSaveMustReset();
        }

        if ((nData==1) || (nData==OMX_ALL)) {
            while (0 != (pBuffer=pRecyclingDelay->flushOneBuffer())) {
                pBuffer->nFilledLen=0;
                mENSComponent.fillBufferDone(pBuffer);
            }
        }
        break;
    /* -Change end for CR330198 Revert to DTS from PTS */

    case OMX_CommandPortEnable:
        // send the new configuration, if it changed, to the codec
        sendConfigToCodec();
        break;
    default:
        break;
    }
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler)
{
    deferEventHandler = OMX_FALSE;
    return OMX_ErrorNone;
}


EXPORT_C
void VFM_NmfHost_ProcessingComponent::getMemoryCB(VFM_MemoryStatus *pMemoryStatus)
{
    mMemoryStatus = *pMemoryStatus;
}

EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst2(TRACE_API,"VFM_NmfHost_ProcessingComponent::emptyThisBuffer ENTER pBuffer 0x%x pBuffer->pBuffer 0x%x\n",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
    if (VFM_Component::skipEmptyThisBuffer(pBuffer)) {
        doSpecificEmptyBufferDone_cb(pBuffer);
        mENSComponent.emptyBufferDone(pBuffer);
        return OMX_ErrorNone;
    }

    /* +Change start for CR330198 Revert to DTS from PTS */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

    /* +Change start for CR 399938 */
    OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();
    if (isMetaDataUsed && pComponent->isEncoder())
    {
		OMX_U32 bufPhysicalAddr;
		OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::emptyThisBuffer > extraProcessingInputBuffers \n");
    	pComponent->pParam->extraProcessingInputBuffers(OMX_DirInput,&pBuffer);
    	bufPhysicalAddr = pComponent->pParam->getBufferPhysicalAddress((OMX_U32)pBuffer->pBuffer);
    	PRINTPG("VFM_NmfHost_ProcessingComponent::Physical addess of input buffer : 0x%x",bufPhysicalAddr);
    	OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::emptyThisBuffer < extraProcessingInputBuffers \n");
	}
	/* -Change end for CR 399938 */

    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
#ifdef __ndk5500_a0__
    /* + change for ER 350391 */
    if (pComponent->isDecoder())
	{
		pDisplayOrder->set(OMX_TRUE);
	}
	/* - change for ER 350391 */
#endif
    pDisplayOrder->save(pBuffer);
    /* -Change end for CR330198 Revert to DTS from PTS */

	if (!isMetaDataUsed) //changes for CR 399938
	{
		OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::emptyThisBuffer Doing iOMX buffer processing \n");
    	/* +Change start for CR332521 IOMX UseBuffer */
    	iOMXType *piOMX = pComponent->pParam->getiOMX();
    	piOMX->getiOMXBuffer(0, pBuffer);    // port is 0 as this is a EmptyThisBuffer
    	/* -Change end for CR332521 IOMX UseBuffer */
	}
	OstTraceFiltInst2(TRACE_API,"VFM_NmfHost_ProcessingComponent::emptyThisBuffer EXIT pBuffer 0x%x pBuffer->pBuffer 0x%x\n",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
    return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

/* +Change start for CR333616 Delay recycling */
EXPORT_C OMX_ERRORTYPE VFM_NmfHost_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst2(TRACE_API,"VFM_NmfHost_ProcessingComponent::fillThisBuffer pBuffer 0x%x pBuffer->pBuffer 0x%x",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    pBuffer = pComponent->pParam->getRecyclingDelay()->recycleBuffer(pBuffer);
    if (pBuffer) {
        /* +Change start for CR332521 IOMX UseBuffer */
        iOMXType *piOMX = pComponent->pParam->getiOMX();
        piOMX->getiOMXBuffer(1, pBuffer);    // port is 1 as this is a FillThisBuffer
        /* -Change end for CR332521 IOMX UseBuffer */
        return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
    } else {
        return OMX_ErrorNone;
    }
}
/* -Change end for CR333616 Delay recycling */

EXPORT_C void VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst2(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb ENTER pBuffer 0x%x pBuffer->pBuffer 0x%x",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
    /* +Change start for CR332521 IOMX UseBuffer */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

    /* +Change start for CR 399938 */
    OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();
    if (isMetaDataUsed && pComponent->isEncoder())
    {
		OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb > extraProcessingInputBuffers \n");
    	pComponent->pParam->extraProcessingInputBuffers(OMX_DirOutput,&pBuffer);
    	OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb < extraProcessingInputBuffers \n");
	}
	/* -Change end for CR 399938 */

	if (!isMetaDataUsed) //changes for CR 399938
	{
		OstTraceFiltInst0(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb doing iOMX buffer processing \n");
	    iOMXType *piOMX = pComponent->pParam->getiOMX();
	    piOMX->getApplicationBuffer(0, pBuffer);    // port is 0 as this is a EmptyBufferDone
	    /* -Change end for CR332521 IOMX UseBuffer */
	}
    OstTraceFiltInst2(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb EXIT pBuffer 0x%x pBuffer->pBuffer 0x%x",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
}

EXPORT_C void VFM_NmfHost_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst2(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificFillBufferDone_cb ENTER pBuffer 0x%x pBuffer->pBuffer 0x%x",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
    VFM_Component *pComponent = (&(VFM_Component &)mENSComponent);
    OMX_U32 constructTime = pComponent->getConstructTime();
    if (constructTime != 0) {
        //OstTraceInt1(TRACE_WARNING, "VFM: VFM_NmfHost_ProcessingComponent: doSpecificFillBufferDone_cb: VIDEOTRACE Startup Time is %dms\n", (VFM_HostPerformances::getTime() - constructTime)/1000);
        // NMF_LOG("VFM: VFM_NmfHost_ProcessingComponent: doSpecificFillBufferDone_cb: VIDEOTRACE Startup Time is %dms\n", (VFM_HostPerformances::getTime() - constructTime)/1000);
        pComponent->resetConstructTime();
    }

    /* +Change start for CR330198 Revert to DTS from PTS */
    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
    pBuffer->nTimeStamp = pDisplayOrder->get(pBuffer);

    /* +Change start for CR332521 IOMX UseBuffer */
    iOMXType *piOMX = pComponent->pParam->getiOMX();
    piOMX->getApplicationBuffer(1, pBuffer);    // port is 1 as this is a FillBufferDone
    /* -Change end for CR332521 IOMX UseBuffer */

    if (pComponent->pParam->getSharedchunkInPlatformprivate()) {
        pBuffer->pPlatformPrivate = pBuffer->pOutputPortPrivate;
    }
    /* -Change end for CR330198 Revert to DTS from PTS */
    OstTraceFiltInst2(TRACE_FLOW,"VFM_NmfHost_ProcessingComponent::doSpecificFillBufferDone_cb EXIT pBuffer 0x%x pBuffer->pBuffer 0x%x",(OMX_U32)pBuffer,(OMX_U32)pBuffer->pBuffer);
}

