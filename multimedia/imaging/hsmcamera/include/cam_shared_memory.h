/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __CAM_SHARED_MEMORY_H__
#define __CAM_SHARED_MEMORY_H__

#include "mmhwbuffer.h"         // Shared memory allocation and address translation on ARM side.
#include "TraceObject.h"
#include "MMIO_Camera.h"        // Shared memory mapping at ISP side.

/* Chunk sizes are defined statically as the memory pool is pre-allocated.
 * They must correspond to an integer number of words to guarantee that each chunk is word-aligned.
 * They must be adapted in case of memory need increase.
 */
#define CAM_SHARED_MEM_CHUNK_3A_STATS_SIZE      30000
#define CAM_SHARED_MEM_CHUNK_SENSOR_NVM_SIZE     5000

/** NB: Must be checked against g_ReadLLAConfig_Status.u32_sensor_Output_Mode_data_size to prevent AOB write.
 *      Assume a maximum of 20 different sensor output modes.
 *      Should eventually be based on sizeof(Sensor_Output_Mode_ts).
 */
#define CAM_SHARED_MEM_CHUNK_SENSOR_MODES_SIZE (50*28)
#define CAM_SHARED_MEM_CHUNK_FW_CONFIG_SIZE      5000

#define CAM_SHARED_MEM_CHUNK_GAMMA_SIZE 128*4

typedef unsigned long csm_uint32_t;
typedef unsigned char csm_uint8_t;

typedef enum {
    CAM_SHARED_MEM_CHUNK_3A_STATS,
    CAM_SHARED_MEM_CHUNK_SENSOR_NVM,
    CAM_SHARED_MEM_CHUNK_SENSOR_MODES,
    CAM_SHARED_MEM_CHUNK_FW_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG,
    CAM_SHARED_MEM_CHUNK_MAX
} camSharedMemChunkId_t;

typedef enum {
    CAM_SHARED_MEM_ERR_NONE = 0,
    CAM_SHARED_MEM_ERR_BAD_STATE,               // The operation is not allowed according to buffer allocation status.
    CAM_SHARED_MEM_ERR_BAD_PRECONDITION,        // The targeted chunk is not available for this operation.
    CAM_SHARED_MEM_ERR_BAD_PARAMETER,           // Wrong chunk ID.
    CAM_SHARED_MEM_ERR_NO_RESOURCE,             // Issue calling system allocator and services.
	CAM_SHARED_MEM_ERR_TRACE                    // Issue passing trace buffer.
} camSharedMemError_t;

typedef struct {
    csm_uint8_t *armLogicalAddress;
    csm_uint8_t *ispLogicalAddress;
    csm_uint32_t size;                    // In bytes.
} camSharedMemChunk_t;


class MMHwBuffer; // References for opaque members

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CCamSharedMemory);
#endif

class CCamSharedMemory
{
    public:
        CCamSharedMemory(TraceObject *traceobj);
        camSharedMemError_t create();
        camSharedMemError_t destroy();
        camSharedMemError_t getChunk(camSharedMemChunk_t *pChunk, camSharedMemChunkId_t chunkId);
        camSharedMemError_t releaseChunk(camSharedMemChunkId_t chunkId);
        camSharedMemError_t cacheInvalidate(camSharedMemChunkId_t chunkId);
        camSharedMemError_t cacheClean(camSharedMemChunkId_t chunkId);
        camSharedMemError_t getBufHnd(csm_uint32_t *pBufHnd);
		camSharedMemError_t getXp70TraceAddress(csm_uint32_t *addr);
		camSharedMemError_t getXp70TracePhyAddress(csm_uint32_t *addr);
        camSharedMemError_t getXp70TraceLogAddress(csm_uint32_t *addr);


    private:
        typedef enum {
            CAM_SHARED_MEM_STATE_CLOSED,
            CAM_SHARED_MEM_STATE_OPEN
        } camSharedMemState_t;

        typedef struct {
            csm_uint8_t sw3AChunk[CAM_SHARED_MEM_CHUNK_3A_STATS_SIZE];
            csm_uint8_t sensorNVMChunk[CAM_SHARED_MEM_CHUNK_SENSOR_NVM_SIZE];
            csm_uint8_t sensorModesChunk[CAM_SHARED_MEM_CHUNK_SENSOR_MODES_SIZE];
            csm_uint8_t fwConfigChunk[CAM_SHARED_MEM_CHUNK_FW_CONFIG_SIZE];

	     csm_uint8_t SharpGreenLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t SharpRedLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t SharpBlueLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpGreenLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpRedLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpBlueLutAddress_LR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];

	     csm_uint8_t SharpGreenLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t SharpRedLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t SharpBlueLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpGreenLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpRedLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
	     csm_uint8_t UnSharpBlueLutAddress_HR[CAM_SHARED_MEM_CHUNK_GAMMA_SIZE];
        } camSharedMemBuffer_t;

        void reset();

        camSharedMemState_t state;
        camSharedMemChunk_t aChunks[CAM_SHARED_MEM_CHUNK_MAX];
        bool abBusyChunks[CAM_SHARED_MEM_CHUNK_MAX];
        camSharedMemBuffer_t *pSharedBuffer;
        MMHwBuffer::TCacheAttribute cacheMode;
        MMHwBuffer * sharedBufferPoolId;
        csm_uint32_t sharedBufferSize;
		csm_uint32_t xp70TraceAddress; // one that is given to ISP FW
		csm_uint32_t xp70TracePhyAddress; // one that is given to MMIO
		csm_uint32_t xp70TraceLogAddress; // one that is given to MMIO
    public:
	    TraceObject* mTraceObject;

};
#endif /* __CAM_SHARED_MEMORY_H__ */
