/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define CAM_SHARED_MEM_LESSER_ALLOC
// #define CAM_SHARED_MEM_DEBUG

#define DBGT_PREFIX "CAMSHAREDMEMORY"
#include "cam_shared_memory.h"



#include "osi_trace.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_cam_shared_memoryTraces.h"
#endif

// Public methods

CCamSharedMemory::CCamSharedMemory(TraceObject *traceobj): mTraceObject(traceobj)
{
    reset();
    state = CCamSharedMemory::CAM_SHARED_MEM_STATE_CLOSED;
#if 0 /*There is problem in cacheablity of buffers*/
    cacheMode = MMHwBuffer::ENormalCached;
#else
    cacheMode = MMHwBuffer::ENormalUnCached;
#endif
    xp70TraceAddress = 0;
    xp70TracePhyAddress = 0;
}

camSharedMemError_t CCamSharedMemory::create()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::create", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_CLOSED) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
          return CAM_SHARED_MEM_ERR_BAD_STATE;
    }

    csm_uint32_t allocSize = 0;
    sharedBufferSize = CAM_SHARED_MEM_CHUNK_3A_STATS_SIZE
            + CAM_SHARED_MEM_CHUNK_SENSOR_NVM_SIZE
            + CAM_SHARED_MEM_CHUNK_SENSOR_MODES_SIZE
            + CAM_SHARED_MEM_CHUNK_FW_CONFIG_SIZE;
            sharedBufferSize += CAM_SHARED_MEM_TRACE_BUFFER_SIZE;
            sharedBufferSize += CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG
            +CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG
            +CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG;

             sharedBufferSize += CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG
            +CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG
            +CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG
            +CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG;

    // Align buffer size on word boundaries.
    sharedBufferSize +=  4 - (sharedBufferSize % 4);

#ifdef CAM_SHARED_MEM_LESSER_ALLOC
     allocSize = 2 * sharedBufferSize;
     if (allocSize > 1*1024*1024) { // Ensure that 1Mb alignment is possible.
        MSG1("CCamSharedMemory::create - Error: Cannot guarantee address alignment of a %u bytes buffer.\n", (unsigned int) sharedBufferSize);
        return CAM_SHARED_MEM_ERR_NO_RESOURCE;
     }
#else /* CAM_SHARED_MEM_LESSER_ALLOC */
     // To avoid any risk of ARM memory corruption by the FW, 1 aligned Mbyte had better be allocated.
     const uint32_t alignedBufferSize = 1*1024*1024;
     uint32_t allocSize = 2 * alignedBufferSize;
     if (sharedBufferSize > alignedBufferSize) { // Ensure that 1Mb alignment is possible.
        MSG1("CCamSharedMemory::create - Error: Cannot guarantee address alignment of a %d bytes buffer.\n", (unsigned int) sharedBufferSize);
        return CAM_SHARED_MEM_ERR_NO_RESOURCE;
     }
#endif /* CAM_SHARED_MEM_LESSER_ALLOC */

     OMX_ERRORTYPE err;
     MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
     poolAttrs.iBuffers = 1;
     poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;  // Memory type
     poolAttrs.iDomainID = 0;                            // FIXME: pass a valid ID, with reserved resources! .MPC Domain ID (only requested for MPC memory type)
     poolAttrs.iSize = allocSize;                        // Size (in byte) of a buffer
     poolAttrs.iAlignment = -1;                          // Alignment applied to the base address of each buffer in the pool // 1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
     poolAttrs.iCacheAttr = cacheMode;

     err = MMHwBuffer::Create(poolAttrs, NULL, sharedBufferPoolId);
     if (err != OMX_ErrorNone) {
         OUT0("\n");
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NO_RESOURCE);
         return CAM_SHARED_MEM_ERR_NO_RESOURCE;
     }

     MMHwBuffer::TBufferInfo bufferInfo;
     err = sharedBufferPoolId->BufferInfo(0, bufferInfo);
     if (err != OMX_ErrorNone) {
         MSG1("Cannot allocate sharedBufferPoolId (err=%d)\n", err);
         OstTraceFiltStatic1(TRACE_DEBUG, "Cannot allocate sharedBufferPoolId (err=%d)", (mTraceObject), err);
         OMX_ERRORTYPE omx_error = MMHwBuffer::Destroy(sharedBufferPoolId);
         if (omx_error != OMX_ErrorNone) {
             MSG1("Cannot free sharedBufferPoolId(err=%d)\n", omx_error);
             OstTraceFiltStatic1(TRACE_DEBUG, "Cannot free sharedBufferPoolId(err=%d)", (mTraceObject), omx_error);
         }
         OUT0("\n");
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NO_RESOURCE);
         return CAM_SHARED_MEM_ERR_NO_RESOURCE;
     }

     OMX_U8 *pData = (OMX_U8*) bufferInfo.iLogAddr;

     csm_uint32_t offset = 0;
 #ifdef CAM_SHARED_MEM_LESSER_ALLOC
     // Only ensure that the shared buffers fits in the same 1Mbyte physical page.
     if ((bufferInfo.iPhyAddr & 0xFFF00000) != ((bufferInfo.iPhyAddr + sharedBufferSize) & 0xFFF00000)) {
         offset = sharedBufferSize;
     }
 #else /* CAM_SHARED_MEM_LESSER_ALLOC */
     // Force physical 1 Mbyte boundary alignment.
     offset = alignedBufferSize - (bufferInfo.iPhyAddr & 0x000FFFFF);
 #endif /* CAM_SHARED_MEM_LESSER_ALLOC */

     pData += offset;
     csm_uint32_t ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(offset + bufferInfo.iPhyAddr);

  #define CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(_a_) ((csm_uint8_t *)(ISPBufferBaseAddress + (csm_uint32_t) (_a_) - (csm_uint32_t) pData));
     pSharedBuffer = (camSharedMemBuffer_t *) pData;

     aChunks[CAM_SHARED_MEM_CHUNK_3A_STATS].armLogicalAddress       = pSharedBuffer->sw3AChunk;
     aChunks[CAM_SHARED_MEM_CHUNK_3A_STATS].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->sw3AChunk);
     aChunks[CAM_SHARED_MEM_CHUNK_3A_STATS].size                    = CAM_SHARED_MEM_CHUNK_3A_STATS_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_NVM].armLogicalAddress     = pSharedBuffer->sensorNVMChunk;
     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_NVM].ispLogicalAddress     = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->sensorNVMChunk);
     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_NVM].size                  = CAM_SHARED_MEM_CHUNK_SENSOR_NVM_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_MODES].armLogicalAddress   = pSharedBuffer->sensorModesChunk;
     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_MODES].ispLogicalAddress   = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->sensorModesChunk);
     aChunks[CAM_SHARED_MEM_CHUNK_SENSOR_MODES].size                = CAM_SHARED_MEM_CHUNK_SENSOR_MODES_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_FW_CONFIG].armLogicalAddress      = pSharedBuffer->fwConfigChunk;
     aChunks[CAM_SHARED_MEM_CHUNK_FW_CONFIG].ispLogicalAddress      = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->fwConfigChunk);
     aChunks[CAM_SHARED_MEM_CHUNK_FW_CONFIG].size                   = CAM_SHARED_MEM_CHUNK_FW_CONFIG_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpGreenLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpGreenLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpRedLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpRedLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpBlueLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpBlueLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpGreenLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpGreenLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpRedLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpRedLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpBlueLutAddress_HR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpBlueLutAddress_HR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpGreenLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpGreenLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpRedLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpRedLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->SharpBlueLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->SharpBlueLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpGreenLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpGreenLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpRedLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpRedLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;

     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG].armLogicalAddress      = pSharedBuffer->UnSharpBlueLutAddress_LR;
    aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG].ispLogicalAddress       = CAM_SHARED_MEM_ARM_TO_ISP_TRANSLATE(pSharedBuffer->UnSharpBlueLutAddress_LR);
     aChunks[CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG].size                    = CAM_SHARED_MEM_CHUNK_GAMMA_SIZE;
     state = CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN;

#ifdef CAM_SHARED_MEM_DEBUG
     // Reset the allocated buffer contents
     for (csm_uint32_t i = 0; i < (allocSize - offset); i++) {
         pData[i] = 0xcd;
     }
     camSharedMemError_t c_e[CAM_SHARED_MEM_CHUNK_MAX];
     for (int i=0; i < CAM_SHARED_MEM_CHUNK_MAX; i++) {
         c_e[i] = cacheClean((camSharedMemChunkId_t) i);
         if (c_e[i] != CAM_SHARED_MEM_ERR_NONE) {
             MSG0("Error while cleaning data cache after initialization.\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Error while cleaning data cache after initialization.", (mTraceObject));
             return CAM_SHARED_MEM_ERR_NO_RESOURCE;
         }
     }
#endif /* CAM_SHARED_MEMORY_DEBUG */

     xp70TraceAddress = ISPBufferBaseAddress + sizeof(camSharedMemBuffer_t);
     xp70TracePhyAddress = offset + bufferInfo.iPhyAddr + sizeof(camSharedMemBuffer_t);
     xp70TraceLogAddress = offset + bufferInfo.iLogAddr + sizeof(camSharedMemBuffer_t);

     OUT0("\n");
     OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
     return CAM_SHARED_MEM_ERR_NONE;
}

camSharedMemError_t CCamSharedMemory::destroy()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::destroy", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    for (int i=0; i < CAM_SHARED_MEM_CHUNK_MAX; i++) {
        if (abBusyChunks[i]) {
            OUT0("\n");
            OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PRECONDITION);
            return CAM_SHARED_MEM_ERR_BAD_PRECONDITION;
        }
    }

    // Reset the T3 xp70 IS space mapping as the buffer is to be freed from the shared memory.
    if (state == CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        (void) MMIO_Camera::mapStatisticsMemArea(0);
    }

     //Need to reset the trace buffer to finish the trace thread responsible for dumping FW traces.
    OMX_ERRORTYPE omx_error = MMIO_Camera::resetTraceBuffer();
    if ( omx_error != OMX_ErrorNone) {
        MSG1("Problem resetting trace buffer (err=%d)\n", omx_error);
    }
    omx_error = MMHwBuffer::Destroy(sharedBufferPoolId);
    if (omx_error != OMX_ErrorNone) {
        MSG1("Problem freeing sharedBufferPoolId (err=%d)\n", omx_error);
        OstTraceFiltStatic1(TRACE_DEBUG, "Problem freeing sharedBufferPoolId (err=%d)", (mTraceObject), omx_error);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NO_RESOURCE);
        return CAM_SHARED_MEM_ERR_NO_RESOURCE;
    }
    reset();
    state = CCamSharedMemory::CAM_SHARED_MEM_STATE_CLOSED;
    xp70TraceAddress = 0;
    xp70TracePhyAddress = 0;
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::create (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

camSharedMemError_t CCamSharedMemory::getChunk(camSharedMemChunk_t *pChunk, camSharedMemChunkId_t chunkId)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::getChunk", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    if (chunkId >= CAM_SHARED_MEM_CHUNK_MAX) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PARAMETER);
        return CAM_SHARED_MEM_ERR_BAD_PARAMETER;
    }
    if (abBusyChunks[chunkId]) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PRECONDITION);
        return CAM_SHARED_MEM_ERR_BAD_PRECONDITION;
    }
    abBusyChunks[chunkId] = true;
    *pChunk = aChunks[chunkId];
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

camSharedMemError_t CCamSharedMemory::releaseChunk(camSharedMemChunkId_t chunkId)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::releaseChunk", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::releaseChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    if (chunkId >= CAM_SHARED_MEM_CHUNK_MAX) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::releaseChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PARAMETER);
        return CAM_SHARED_MEM_ERR_BAD_PARAMETER;
    }
    if (! abBusyChunks[chunkId]) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::releaseChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PRECONDITION);
        return CAM_SHARED_MEM_ERR_BAD_PRECONDITION;
    }
    abBusyChunks[chunkId] = false;
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::releaseChunk (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

camSharedMemError_t CCamSharedMemory::cacheInvalidate(camSharedMemChunkId_t chunkId)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::cacheInvalidate", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheInvalidate (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    if (chunkId >= CAM_SHARED_MEM_CHUNK_MAX) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheInvalidate (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PARAMETER);
        return CAM_SHARED_MEM_ERR_BAD_PARAMETER;
    }
    if (! abBusyChunks[chunkId]) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheInvalidate (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PRECONDITION);
        return CAM_SHARED_MEM_ERR_BAD_PRECONDITION;
    }
    // Flush the data that have been written by the ISP FW
    OMX_U32 aPhysAddr; // out
    OMX_ERRORTYPE e_ret = sharedBufferPoolId->CacheSync(MMHwBuffer::ESyncAfterWriteHwOperation,
            (OMX_U32) aChunks[chunkId].armLogicalAddress,
            aChunks[chunkId].size, aPhysAddr);
    if (e_ret != OMX_ErrorNone) {
        MSG1("Problem invalidating data cache (err=%d)\n", e_ret);
        OstTraceFiltStatic1(TRACE_DEBUG, "Problem invalidating data cache (err=%d)", (mTraceObject), e_ret);
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheInvalidate (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NO_RESOURCE);
        return CAM_SHARED_MEM_ERR_NO_RESOURCE;
    }
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheInvalidate (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

camSharedMemError_t CCamSharedMemory::cacheClean(camSharedMemChunkId_t chunkId)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::cacheClean", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheClean (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    if (chunkId >= CAM_SHARED_MEM_CHUNK_MAX) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheClean (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PARAMETER);
        return CAM_SHARED_MEM_ERR_BAD_PARAMETER;
    }
#ifndef CAM_SHARED_MEM_DEBUG
    /* Exceptionally allow cleaning before the chunk has been requested. For memory debug purpose. */
    if (! abBusyChunks[chunkId]) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheClean (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_PRECONDITION);
        return CAM_SHARED_MEM_ERR_BAD_PRECONDITION;
    }
#endif /* CAM_SHARED_MEM_DEBUG */
    OMX_U32 aPhysAddr; // out
    /* OMX_ERRORTYPE e_ret = sharedBufferPoolId->CacheSync(MMHwBuffer::ESyncBeforeReadHwOperation,
     * After the switch to the 2.6.35 kernel version, MMHwBuffer cache synchronization has changed
     * (switch from pmem to hwmem driver), with an impact on the API => sync before read AND before write are mandatory. */
    OMX_ERRORTYPE e_ret1 = sharedBufferPoolId->CacheSync(MMHwBuffer::ESyncBeforeReadHwOperation,
            (OMX_U32) aChunks[chunkId].armLogicalAddress,
            aChunks[chunkId].size, aPhysAddr);

    OMX_ERRORTYPE e_ret2 = sharedBufferPoolId->CacheSync(MMHwBuffer::ESyncBeforeWriteHwOperation,
            (OMX_U32) aChunks[chunkId].armLogicalAddress,
            aChunks[chunkId].size, aPhysAddr);
    if ((e_ret1 != OMX_ErrorNone) || (e_ret2 != OMX_ErrorNone)){
        MSG2("Problem cleaning data cache (err=%d);%d\n", e_ret1, e_ret2);
        OstTraceFiltStatic2(TRACE_DEBUG, "Problem cleaning data cache (err=%d);%d", (mTraceObject), e_ret1, e_ret2);
        OUT0("");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheClean (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NO_RESOURCE);
        return CAM_SHARED_MEM_ERR_NO_RESOURCE;
    }
    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::cacheClean (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

/** *pBufHnd is written zero in case the memory pool is not cacheable and therefore
 * does not need to be synchronized.
 */
camSharedMemError_t CCamSharedMemory::getBufHnd(csm_uint32_t *pBufHnd)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::getBufHnd", (mTraceObject));
    if (state != CCamSharedMemory::CAM_SHARED_MEM_STATE_OPEN) {
        OUT0("\n");
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getBufHnd (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_BAD_STATE);
        return CAM_SHARED_MEM_ERR_BAD_STATE;
    }
    if (cacheMode == MMHwBuffer::ENormalCached) {
        *pBufHnd = (csm_uint32_t) sharedBufferPoolId;
    } else {
        *pBufHnd = 0;
    }

    OUT0("\n");
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CCamSharedMemory::getBufHnd (%d)", (mTraceObject), CAM_SHARED_MEM_ERR_NONE);
    return CAM_SHARED_MEM_ERR_NONE;
}

// Private methods

void CCamSharedMemory::reset()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CCamSharedMemory::reset", (mTraceObject));
    for (int i=0; i < CAM_SHARED_MEM_CHUNK_MAX; i++) {
        aChunks[i].armLogicalAddress = 0;
        aChunks[i].armLogicalAddress = 0;
        aChunks[i].size = 0;
        abBusyChunks[i] = false;
    }
    pSharedBuffer = NULL;
    sharedBufferPoolId = NULL;
    sharedBufferSize = 0;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CCamSharedMemory::reset", (mTraceObject));
}

camSharedMemError_t CCamSharedMemory::getXp70TraceAddress(csm_uint32_t *addr)
{
  if (xp70TraceAddress) {
    if (addr)
      *addr = xp70TraceAddress;
    return CAM_SHARED_MEM_ERR_NONE;
  } else {
    return CAM_SHARED_MEM_ERR_NO_RESOURCE;
  }
}

camSharedMemError_t CCamSharedMemory::getXp70TracePhyAddress(csm_uint32_t *addr)
{
  if (xp70TracePhyAddress) {
    if (addr)
      *addr = xp70TracePhyAddress;
    return CAM_SHARED_MEM_ERR_NONE;
  } else {
    return CAM_SHARED_MEM_ERR_NO_RESOURCE;
  }
}

camSharedMemError_t CCamSharedMemory::getXp70TraceLogAddress(csm_uint32_t *addr)
{
  if (xp70TraceLogAddress) {
    if (addr)
      *addr = xp70TraceLogAddress;
    return CAM_SHARED_MEM_ERR_NONE;
  } else {
    return CAM_SHARED_MEM_ERR_NO_RESOURCE;
  }
}


