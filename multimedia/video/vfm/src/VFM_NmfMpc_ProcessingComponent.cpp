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
#include "VFM_NmfMpc_ProcessingComponent.h"
#include "SharedBuffer.h"
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_NmfMpc_ProcessingComponentTraces.h"
#endif

// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

#define PRINTPG

inline void VFM_NmfMpc_ProcessingComponent::vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        OstTraceInt3(TRACE_ERROR, "VFM: VFM_NmfMpc_ProcessingComponent: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

EXPORT_C  VFM_NmfMpc_ProcessingComponent::VFM_NmfMpc_ProcessingComponent(ENS_Component &enscomp)
            : NmfMpc_ProcessingComponent(enscomp)
{
	enscomp.setProcessingComponent(this);
}

EXPORT_C  OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::setPortCount(OMX_U32 nPortCount)
{
    mPortPoolid = new OMX_U32[nPortCount];
	if(mPortPoolid == 0)
		return OMX_ErrorInsufficientResources;
	return OMX_ErrorNone;
}

EXPORT_C  VFM_NmfMpc_ProcessingComponent::~VFM_NmfMpc_ProcessingComponent()
{
	IN0("");
	if(mPortPoolid){
		delete[] mPortPoolid;
	}
	OUT0("");
}


EXPORT_C
OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::doBufferAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **ppBufferMetaData)
{
    MMHwBuffer *sharedChunk=0;
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    OMX_ERRORTYPE error;
#ifdef _CACHE_OPT_
    OMX_BOOL isCached = OMX_TRUE;
#else
    OMX_BOOL isCached = OMX_FALSE;
#endif
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
	if (nBufferIndex == 0)
	{
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        poolAttrs.iBuffers = port->getBufferCountActual();      // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
        poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
        poolAttrs.iSize = nSizeBytes;                           // Size (in byte) of a buffer
        poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool
		                                                        //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        error = MMHwBuffer::Create(poolAttrs, mENSComponent.getOMXHandle(), sharedChunk);
		if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;
        port->setSharedChunk(sharedChunk);


	}

	// Let's now retrieve the "current" buffer
    sharedChunk = port->getSharedChunk();
    MMHwBuffer::TBufferInfo bufferInfo;
    error = sharedChunk->BufferInfo(nBufferIndex, bufferInfo);
	if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;

    *ppData = (OMX_U8 *)bufferInfo.iLogAddr;

    *ppBufferMetaData = sharedChunk;

	 OstTraceInt2(TRACE_FLOW,"pBuffer = 0x%x and pBufferMetaData = 0x%x",(OMX_U32)*ppData,(OMX_U32)*ppBufferMetaData);
    //OUTR("",error);
    return OMX_ErrorNone;
}

EXPORT_C
OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::doBufferDeAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, void * pBufferMetaData)
{
    OMX_ERRORTYPE error;
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (sharedChunk)
    {
         error = MMHwBuffer::Destroy(sharedChunk);
         vfm_assert_static((error == OMX_ErrorNone), __LINE__, OMX_TRUE);
         port->setSharedChunk(0);
    }
    return OMX_ErrorNone;
}

EXPORT_C
OMX_U32 VFM_NmfMpc_ProcessingComponent::getBufferMpcAddress(void *bufferAllocInfo) {
	// buffer is not visible from DSP
	return 0;
}

EXPORT_C
OMX_U32 VFM_NmfMpc_ProcessingComponent::getBufferPhysicalAddress(void * bufferAllocInfo, OMX_U8* aLogicalAddr, OMX_U32 aSize)
{
	OstTraceInt0(TRACE_API,"Entering VFM_NmfMpc_ProcessingComponent getBufferPhysicalAddress\n");
		OMX_U32 bufPhysicalAddr;

		OMX_ERRORTYPE error = ((MMHwBuffer *)bufferAllocInfo)->PhysAddress((OMX_U32)aLogicalAddr, aSize,bufPhysicalAddr);
        vfm_assert_static((error == OMX_ErrorNone), __LINE__, OMX_TRUE);

		return bufPhysicalAddr;
}

EXPORT_C  OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::bindPortForProprietaryCom(ENS_Port *port)
{
	return OMX_ErrorNone;
}

EXPORT_C  OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::unBindPortForProprietaryCom(ENS_Port *port)
{
	return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::freeBuffer(
		OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated,
        void *bufferAllocInfo,
        void *portPrivateInfo)
{
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    OMX_BOOL isMetaDataUsed = pComponent->pParam->getStoreMetadataInBuffers();

	if (isMetaDataUsed && pComponent->isEncoder() && (0 == nPortIndex))
    {
		PRINTPG("In use buffer and for metaData and freeing SHARED buf hereDO NOTHING HERER \n");
        SharedBuffer *sharedBuff = (SharedBuffer *)portPrivateInfo;
        if(sharedBuff)
        {
	        PRINTPG("Value of shared buffer in VFM_FREE 0x%x \n",sharedBuff);
	        delete sharedBuff;
	        sharedBuff = NULL;
		}
		else
		{
			PRINTPG("Value ofshredbuf in NULL ONLY \n");
		}
		return OMX_ErrorNone;
	}
	NmfMpc_ProcessingComponent::freeBuffer(nPortIndex,nBufferIndex,bBufferAllocated,bufferAllocInfo,portPrivateInfo);
        return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    if (VFM_Component::skipEmptyThisBuffer(pBuffer)) {
        mENSComponent.emptyBufferDone(pBuffer);
        return OMX_ErrorNone;
    }

	/* +Change start for CR330198 Revert to DTS from PTS */
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
    pDisplayOrder->save(pBuffer);
    /* -Change end for CR330198 Revert to DTS from PTS */

    /* +Change start for CR332521 IOMX UseBuffer */
    iOMXType *piOMX = pComponent->pParam->getiOMX();
    piOMX->getiOMXBuffer(0, pBuffer);    // port is 0 as this is a EmptyThisBuffer
    /* -Change end for CR332521 IOMX UseBuffer */

    return NmfMpc_ProcessingComponent::emptyThisBuffer(pBuffer);
}

/* +Change start for CR333616 Delay recycling */
EXPORT_C OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    pBuffer = pComponent->pParam->getRecyclingDelay()->recycleBuffer(pBuffer);
    if (pBuffer) {
        /* +Change start for CR332521 IOMX UseBuffer */
        iOMXType *piOMX = pComponent->pParam->getiOMX();
        piOMX->getiOMXBuffer(1, pBuffer);    // port is 1 as this is a FillThisBuffer
        /* -Change end for CR332521 IOMX UseBuffer */
        return NmfMpc_ProcessingComponent::fillThisBuffer(pBuffer);
    } else {
        return OMX_ErrorNone;
    }
}
/* -Change end for CR333616 Delay recycling */

/* +Change start for CR332521 IOMX UseBuffer */
EXPORT_C void VFM_NmfMpc_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    iOMXType *piOMX = pComponent->pParam->getiOMX();
    piOMX->getApplicationBuffer(0, pBuffer);    // port is 0 as this is a EmptyBufferDone
}
/* -Change end for CR332521 IOMX UseBuffer */


/* +Change start for CR330198 Revert to DTS from PTS */
EXPORT_C void VFM_NmfMpc_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
    VFM_Component *pComponent = (&(VFM_Component &)mENSComponent);
    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
    pBuffer->nTimeStamp = pDisplayOrder->get(pBuffer);

    /* +Change start for CR332521 IOMX UseBuffer */
    iOMXType *piOMX = pComponent->pParam->getiOMX();
    piOMX->getApplicationBuffer(1, pBuffer);    // port is 1 as this is a FillBufferDone
    /* -Change end for CR332521 IOMX UseBuffer */

    if (pComponent->pParam->getSharedchunkInPlatformprivate()) {
        pBuffer->pPlatformPrivate = pBuffer->pOutputPortPrivate;
    }
}
/* -Change end for CR330198 Revert to DTS from PTS */

/* +Change start for CR330198 Revert to DTS from PTS */
/* +Change start for CR333616 Delay recycling */
EXPORT_C OMX_ERRORTYPE VFM_NmfMpc_ProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    bDeferredCmd = OMX_FALSE;
    VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);
    DisplayOrderType *pDisplayOrder = pComponent->pParam->getDisplayOrder();
    RecyclingDelayType *pRecyclingDelay = pComponent->pParam->getRecyclingDelay();
    OMX_BUFFERHEADERTYPE *pBuffer;

    switch (eCmd) {
    case OMX_CommandStateSet:
        if (nData==OMX_StateIdle) {
            pDisplayOrder->nextSaveMustReset();
            while (0 != (pBuffer=pRecyclingDelay->flushOneBuffer())) {
                pBuffer->nFilledLen=0;
                NmfMpc_ProcessingComponent::fillThisBuffer(pBuffer);
            }
        }
        break;

    case OMX_CommandFlush:
    case OMX_CommandPortDisable:
        if (eCmd==OMX_CommandFlush) {
            // nothing to do on disable as in this case, buffers that
            // have been sent should be processed anyway
            pDisplayOrder->nextSaveMustReset();
        }

        if (nData==1) {
            while (0 != (pBuffer=pRecyclingDelay->flushOneBuffer())) {
                pBuffer->nFilledLen=0;
                mENSComponent.fillBufferDone(pBuffer);
            }
        }
        break;

    default:
        break;
    }
    return OMX_ErrorNone;
}
/* -Change end for CR330198 Revert to DTS from PTS */
/* -Change end for CR333616 Delay recycling */

