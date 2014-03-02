#define LOG_TAG "CameraHal"

#include <binder/MemoryHeapBase.h>

#include "STECamTrace.h"
#include "STECamMemoryHeapBase.h"

#include "mmhwbuffers/STECamMMHwBuffer.h"

//#define DBGT_LAYER 0
//#define DBGT_PREFIX "B2R2Utils"

#include <dlfcn.h>
#include <OMX_Core.h>
#include <OMX_Component.h>

#include "STECamOmxBuffInfo.h"


static void* mDlHandle = NULL;
static OMX_ERRORTYPE (*CreateBuffers)(const MMHwBuffer::TBufferPoolCreationAttributes&, OMX_HANDLETYPE, MMHwBuffer*&) = NULL;
static MMNativeBuffer* (*CreateMMNativeBuf)(void*) = NULL;
static OMX_ERRORTYPE (*FreeBuffers)(MMHwBuffer*&) = NULL;

using namespace android;
/*static*/ int STECamMMHwBuffer::init() {
    const char HWBuflibPath[] = "/system/lib/libste_ensloader.so";

    mDlHandle = dlopen(HWBuflibPath, RTLD_LAZY);
    if(NULL == mDlHandle) {
        DBGT_CRITICAL("dlopen failed");
        DBGT_EPILOG("");
        return -1;
    }

    *(void **)(&CreateBuffers) = dlsym(mDlHandle, "MMHwBuffer_Create_CWrapper");
    *(void **)(&CreateMMNativeBuf) = dlsym(mDlHandle, "create_mmnative_buffer");
    *(void **)(&FreeBuffers) = dlsym(mDlHandle, "MMHwBuffer_Destroy_CWrapper");

    return 0;
}

/*static*/ void STECamMMHwBuffer::deinit() {
    if(mDlHandle != NULL) {
        dlclose(mDlHandle);
    }
}

/*static*/ OMX_ERRORTYPE STECamMMHwBuffer::allocateHwBuffer(OMX_U32 aCount, OMX_U32 aSize, bool aCached,
                                                            OmxBuffInfo* aOwnerOmxBuffInfo, int aOwnerPort, sp<MemoryHeapBase>* aMemoryHeapBase,
                                                            OmxBuffInfo* aShareOmxBuffInfo, int aSharePort) {
    DBGT_PROLOG("aCount: %d, aSize: %d, aCached: %d, aShareOmxBuffInfo: %#x",(int)aCount, (int)aSize, aCached, (unsigned int)aShareOmxBuffInfo);

    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

    MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
    poolAttrs.iBuffers = aCount;                        // Number of buffers in the pool
    poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;  // Memory type
    poolAttrs.iDomainID = 0;                            // MPC Domain ID (only requested for MPC memory type)
    poolAttrs.iSize = aSize;                            // Size (in byte) of a buffer
    poolAttrs.iAlignment = 256;                         // Alignment applied to the base address of each buffer in the pool
                                                        // 1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
    if(aCached)
        poolAttrs.iCacheAttr = MMHwBuffer::ENormalCached;  // Chached or Unchached
    else
        poolAttrs.iCacheAttr = MMHwBuffer::ENormalUnCached;  // Chached or Unchached

    DBGT_PTRACE("poolAttrs-> iBuffers : %d, iDeviceType: %d, iSize: %d, iCacheAttr: %d",
        (int)poolAttrs.iBuffers,(int)poolAttrs.iDeviceType,(int)poolAttrs.iSize,(int)poolAttrs.iCacheAttr);

    aOwnerOmxBuffInfo[0].m_pMMHwBuffer = NULL;
    omxerr = (*CreateBuffers)(poolAttrs, 0, aOwnerOmxBuffInfo[0].m_pMMHwBuffer);
    DBGT_PTRACE("poolid : %#x",(unsigned int) aOwnerOmxBuffInfo[0].m_pMMHwBuffer);
    if(OMX_ErrorNone != omxerr) {
        DBGT_CRITICAL("CreateBuffers failed");
        DBGT_EPILOG("");
        return omxerr;
    }

    aOwnerOmxBuffInfo[0].mBufferOwner = true;

    if(aMemoryHeapBase != NULL) {
        omxerr = aOwnerOmxBuffInfo[0].m_pMMHwBuffer->BufferInfo(0, aOwnerOmxBuffInfo[0].mMMHwBufferInfo);

        omxerr = aOwnerOmxBuffInfo[0].m_pMMHwBuffer->GetConfigExtension((OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                                                                         &aOwnerOmxBuffInfo[0].mMMHwChunkMetaData, aOwnerPort);
#ifndef USE_HWMEM
        int error;
        struct pmem_region region;
        error = ioctl((int)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nFd, PMEM_GET_TOTAL_SIZE, &region);
#endif
        aMemoryHeapBase->clear();

#ifdef USE_HWMEM
        *aMemoryHeapBase = new CamMemoryHeapBase(
                            (int)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nFd,
                            (unsigned int)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nBaseLogicalAddr,
                            (size_t)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nChunkSize);
#else
        *aMemoryHeapBase = new CamMemoryHeapBase(
                            (int)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nFd,
                            (unsigned int)aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nBaseLogicalAddr,
                            (size_t)region.len);
#endif
    }

    for (unsigned int i = 0; i < aCount; i++) {
        aOwnerOmxBuffInfo[i].m_pMMHwBuffer = aOwnerOmxBuffInfo[0].m_pMMHwBuffer;

        omxerr = aOwnerOmxBuffInfo[i].m_pMMHwBuffer->BufferInfo(i, aOwnerOmxBuffInfo[i].mMMHwBufferInfo);

        omxerr = aOwnerOmxBuffInfo[i].m_pMMHwBuffer->GetConfigExtension((OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                                                                         &aOwnerOmxBuffInfo[i].mMMHwChunkMetaData, aOwnerPort);

        aOwnerOmxBuffInfo[i].mHeapOffset = (unsigned int)aOwnerOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr
                                           - (unsigned int)aOwnerOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr;

        if(aMemoryHeapBase != NULL) {
            aOwnerOmxBuffInfo[i].clear();
            aOwnerOmxBuffInfo[i].mCamMemoryBase = new CamMemoryBase(*aMemoryHeapBase, aOwnerOmxBuffInfo[i].mHeapOffset, aSize);
        }

        if(aShareOmxBuffInfo) {
            aShareOmxBuffInfo[i].m_pMMHwBuffer = aOwnerOmxBuffInfo[i].m_pMMHwBuffer;

            omxerr = aShareOmxBuffInfo[i].m_pMMHwBuffer->BufferInfo(i, aShareOmxBuffInfo[i].mMMHwBufferInfo);

            omxerr = aShareOmxBuffInfo[i].m_pMMHwBuffer->GetConfigExtension((OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                                                                         &aShareOmxBuffInfo[i].mMMHwChunkMetaData, aSharePort);

            aShareOmxBuffInfo[i].mHeapOffset = aOwnerOmxBuffInfo[i].mHeapOffset;

            if(aMemoryHeapBase != NULL) {
                aShareOmxBuffInfo[i].clear();
                aShareOmxBuffInfo[i].mCamMemoryBase = new CamMemoryBase(*aMemoryHeapBase, aShareOmxBuffInfo[i].mHeapOffset, aSize);
            }
        }

    }

    DBGT_EPILOG("");
    return omxerr;
}

/*static*/ OMX_ERRORTYPE STECamMMHwBuffer::freeHwBuffer(OmxBuffInfo* aOwnerOmxBuffInfo, bool aNativeWindowFlag) {
    DBGT_PROLOG("aOwnerOmxBuffInfo: %#x", (unsigned int)aOwnerOmxBuffInfo);

    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

    for(unsigned int i = 0; i < aOwnerOmxBuffInfo[0].mMMHwChunkMetaData.nBufferCount; i++) {

        if(aOwnerOmxBuffInfo[i].mBufferOwner && aOwnerOmxBuffInfo[i].m_pMMHwBuffer) {
            omxerr = (*FreeBuffers)(aOwnerOmxBuffInfo[i].m_pMMHwBuffer);
        }
        aOwnerOmxBuffInfo[i].m_pMMHwBuffer = NULL;
        memset(&aOwnerOmxBuffInfo[i].mMMHwBufferInfo, 0, sizeof(aOwnerOmxBuffInfo[i].mMMHwBufferInfo));
        memset(&aOwnerOmxBuffInfo[i].mMMHwChunkMetaData, 0, sizeof(aOwnerOmxBuffInfo[i].mMMHwChunkMetaData));
        aOwnerOmxBuffInfo[i].mBufferOwner = false;
        if(aNativeWindowFlag) {
            aOwnerOmxBuffInfo[i].mFlags = aOwnerOmxBuffInfo[i].mFlags & OmxBuffInfo::ESentToNativeWindow;
        } else {
            aOwnerOmxBuffInfo[i].mFlags = OmxBuffInfo::ENone;
        }
        aOwnerOmxBuffInfo[i].clear();
    }

    DBGT_EPILOG("");
    return omxerr;
}

/* static */
OMX_ERRORTYPE STECamMMHwBuffer::allocateMMNativeBuf(MMNativeBuffer** aOwnerMMNativeBuf,
                                                    void* aBuf)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

    *aOwnerMMNativeBuf = (MMNativeBuffer*)(*CreateMMNativeBuf)(aBuf);
    if ((*aOwnerMMNativeBuf == NULL) ||
        ((*aOwnerMMNativeBuf)->init() != 0)) {
        DBGT_PTRACE("MM Native Buffer Init failed");
        omxerr = OMX_ErrorUndefined;
    }
    DBGT_EPILOG("");
    return omxerr;
}

