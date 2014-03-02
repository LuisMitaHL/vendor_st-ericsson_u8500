/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <unistd.h>

//#define LOG_NDEBUG 0
#define LOG_TAG "MMHWBuffer"
#include "linux_utils.h"

#include "OMX_Component.h"
#include "MMHwBufferLinux.h"
#include "mmhwbuffer_ext.h"

#ifdef ANDROID
#include "media/hardware/HardwareAPI.h"
#include "hardware/gralloc.h"
using namespace android;
#endif //ANDROID

// Uncomment below to get buffer list dump after all create/destroy and to get buffer details
//#define DUMP_MMHWBUFFERLINUX 1

pthread_mutex_t MMHwBufferLinux::globalMutex = PTHREAD_MUTEX_INITIALIZER;
MMHwBufferLinux_list_head_t MMHwBufferLinux::mMMHwBufferList;

/********************************************************************************
 * MMHwBuffer class static members implementation
 ********************************************************************************/

OMX_ERRORTYPE MMHwBuffer::Create(const TBufferPoolCreationAttributes &aBuffPoolAttrs,
                                 OMX_HANDLETYPE aComponent,
                                 MMHwBuffer * &apMMHwBufferHandle)
{
    ALOGV("Create (iBuffers=%d, iSize=%d, iAlignment=%d, iCacheAttr=%d, aComponent=0x%08x)",
         (int)aBuffPoolAttrs.iBuffers, (int)aBuffPoolAttrs.iSize,
         (int)aBuffPoolAttrs.iAlignment, (int)aBuffPoolAttrs.iCacheAttr, (unsigned int)aComponent);

    if (apMMHwBufferHandle != 0) {
        ALOGE("Create:%d InsufficientResources", __LINE__);
        return OMX_ErrorBadParameter;
    }

    if (aBuffPoolAttrs.iDeviceType != ESystemMemory) {
        ALOGE("Create:%d InsufficientResources", __LINE__);
        return OMX_ErrorBadParameter;
    }

    MMHwBufferPool *bPool = new MMHwBufferPool();

    if (bPool == NULL) {
        ALOGE("Create:%d InsufficientResources", __LINE__);
        return OMX_ErrorInsufficientResources;
    }

    if (bPool->prepare(aBuffPoolAttrs, aComponent) != OMX_ErrorNone) {
        delete bPool;
        ALOGE("Create:%d InsufficientResources", __LINE__);
        return OMX_ErrorInsufficientResources;
    }

    MMHwBufferLinux *bHandle = new MMHwBufferLinux(bPool, aComponent);
    if (bHandle == NULL) {
        ALOGE("Create:%d InsufficientResources", __LINE__);
        return OMX_ErrorInsufficientResources;
    }

    apMMHwBufferHandle = bHandle;

    ALOGV("Create returns bHandle=0x%08x bPool=0x%08x",
         (unsigned int)bHandle, (unsigned int)bPool);

#if ((LOG_NDEBUG == 0) || (DUMP_MMHWBUFFERLINUX))
    MMHwBufferLinux::dumpMMHwBufferLinux();
#endif

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBuffer::Destroy(MMHwBuffer * &apMMHwBufferHandle)
{
    ALOGV("Destroy (bHandle=0x%08x)", (unsigned int)apMMHwBufferHandle);

    if (apMMHwBufferHandle == 0) {
        return OMX_ErrorBadParameter;
    }

    MMHwBufferLinux *bHandle = static_cast<MMHwBufferLinux *>(apMMHwBufferHandle);

    // Acquire global lock to ensure we delete handle on a exclusive way
    MMHwBufferLinux::GlobalLock();

    if (!MMHwBufferLinux::isValidHandle(bHandle)) {
        ALOGE("Destroy called on invalid handle=0x%08x", (unsigned int)bHandle);
        MMHwBufferLinux::GlobalUnlock();
        return OMX_ErrorBadParameter;
    }

    if (bHandle->decreaseNbRef() == 0) {
        ALOGV("Destroy (0x%08x) - delete bHandle=0x%08x",
             (unsigned int)apMMHwBufferHandle, (unsigned int)bHandle);
        delete bHandle;
    }
    MMHwBufferLinux::GlobalUnlock();

#if ((LOG_NDEBUG == 0) || (DUMP_MMHWBUFFERLINUX))
    MMHwBufferLinux::dumpMMHwBufferLinux();
#endif

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBuffer::Open(OMX_HANDLETYPE aComponent,
                               OMX_INDEXTYPE aIndex,
                               OMX_PTR apConf,
                               MMHwBuffer * &apMMHwBufferHandle)
{
    ALOGV("Open (aComponent=0x%08x, aIndex=0x%08x)",
         (unsigned int)aComponent, (unsigned int)aIndex);

    if (apMMHwBufferHandle != 0) {
        return OMX_ErrorBadParameter;
    }

#ifdef ANDROID
    bool allocateNativeBuffersHandle = false;
    OMX_U32 portIndex = 0;
    // We allocate an MMHwBufferLinux in case:
    // - enableAndroidNativeBuffers: SetParameter done first time we want to use Native Buffers
    // - getAndroidNativeBufferUsage: when StageFright receives a PortSettingChange it
    //     + sends OMX_CommandPortDisable
    //     + Frees all the buffers
    //     + Sends OMX_CommandPortEnable
    //     + GetExtensionIndex OMX.google.android.index.getAndroidNativeBufferUsage
    //     it does not performs a SetParameter OMX.google.android.index.enableAndroidNativeBuffers
    //     as this was already done
    // So in case ENS does not have a MMHwBuffer handle on which to query for a ConfigExtension it
    // will call MMHwBuffer::Open with OMX.google.android.index.getAndroidNativeBufferUsage as index
    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers) {
        struct EnableAndroidNativeBuffersParams *params =
            static_cast<EnableAndroidNativeBuffersParams *>(apConf);
        allocateNativeBuffersHandle = params->enable;
        portIndex = params->nPortIndex;
    }
    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_getAndroidNativeBufferUsage) {
        struct GetAndroidNativeBufferUsageParams *params =
            static_cast<struct GetAndroidNativeBufferUsageParams *>(apConf);
        allocateNativeBuffersHandle = true;
        portIndex = params->nPortIndex;
    }
    if (allocateNativeBuffersHandle) {
        // We create the object and we'll populate it with the gralloc buffers
        MMHwBufferList *bList = new MMHwBufferList();
        if (!bList) {
            ALOGE("Open - allocateNativeBuffersHandle:%d InsufficientResources", __LINE__);
            return OMX_ErrorInsufficientResources;
        }
        OMX_ERRORTYPE error = bList->Init();
        if (error) {
            delete bList;
            return error;
        }
        apMMHwBufferHandle = new MMHwBufferLinux(bList, aComponent, portIndex);
        if (!apMMHwBufferHandle) {
            ALOGE("Open - allocateNativeBuffersHandle:%d InsufficientResources", __LINE__);
            return OMX_ErrorInsufficientResources;
        }
        ALOGV("Open allocateNativeBuffersHandle - "
             "aComponent=0x%08x aIndex=0x%08x => bHandle=0x%08x bList=0x%08x",
             (unsigned int)aComponent, (int)aIndex,
             (unsigned int)apMMHwBufferHandle, (unsigned int)bList);

        if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_getAndroidNativeBufferUsage) {
            apMMHwBufferHandle->GetConfigExtension(aIndex, apConf, 0 /* Not relevant for this index */);
        }

        return OMX_ErrorNone;
    }

    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer) {
        UseAndroidNativeBufferParams *params =
            static_cast<UseAndroidNativeBufferParams *>(apConf);
        MMHwBufferLinux *bHandle = MMHwBufferLinux::getMMHwBufferLinux(aComponent,
                                                                       params->nPortIndex);
        ALOGV("Open useAndroidNativeBuffer - aComponent=0x%08x aIndex=0x%08x => bHandle=0x%08x",
             (unsigned int)aComponent, (int)aIndex, (unsigned int)bHandle);
        if (!bHandle) {
            ALOGE("Open: failed to find AndroidNativeBuffers for aComponent=0x%08x nPortIndex=%d",
                 (unsigned int)aComponent, (int)params->nPortIndex);
            return OMX_ErrorInsufficientResources;
        }
        apMMHwBufferHandle = bHandle;
        MMHwBufferList *tMMHwBufferList = bHandle->getBufferList();
        if (tMMHwBufferList) {
            return tMMHwBufferList->addAndroidNativeBuffer(params);
        } else {
            ALOGE("Open useAndroidNativeBuffer on bHandle=0x%08x : aComponent=0x%08x aIndex=0x%08x "
                 "is not a buffer list",
                 (unsigned int)bHandle, (unsigned int)aComponent, (int)aIndex);
            return OMX_ErrorBadParameter;
        }
    }
#endif //ANDROID

    if (aIndex == (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata) {
        OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *sharedConf =
            static_cast<OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *>(apConf);

        if (sharedConf->nOwnerPID == (OMX_U32)getpid()) {
            // Opening chunk in same process

            if (!sharedConf->nHandleId) {
                ALOGE("Open: NULL nHandleId in SHARED_CHUNK_METADATA");
                return OMX_ErrorBadParameter;
            }
            MMHwBufferLinux *bHandle = (MMHwBufferLinux *)(sharedConf->nHandleId);
            bHandle->increaseNbRef();
            apMMHwBufferHandle = bHandle;

            ALOGV("Open shared chunk from same process (bHandle=0x%08x)", (unsigned int)bHandle);

            return OMX_ErrorNone;
        } else {
            //Opening chunk in different process

            MMHwBufferPool *bPool = new MMHwBufferPool();

            if (bPool == NULL) {
                ALOGE("Open:%d InsufficientResources", __LINE__);
                return OMX_ErrorInsufficientResources;
            }

            if (bPool->Open(sharedConf)) {
                delete bPool;
                ALOGE("Open:%d InsufficientResources", __LINE__);
                return OMX_ErrorInsufficientResources;
            }

            if (bPool->map()) {
                delete bPool;
                ALOGE("Open:%d InsufficientResources", __LINE__);
                return OMX_ErrorInsufficientResources;
            }

            MMHwBufferLinux *bHandle = new MMHwBufferLinux(bPool, aComponent);
            if (bHandle == NULL) {
                return OMX_ErrorInsufficientResources;
            }

            apMMHwBufferHandle = bHandle;

            ALOGV("Open shared chunk from different process (bHandle=0x%08x)",
                 (unsigned int)bHandle);

            return OMX_ErrorNone;
        }
    }
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE MMHwBuffer::Close(MMHwBuffer * &apMMHwBufferHandle)
{
    ALOGV("Close (bHandle=0x%08x)", (unsigned int)apMMHwBufferHandle);
    return Destroy(apMMHwBufferHandle);
}

OMX_ERRORTYPE MMHwBuffer::GetIndexExtension(OMX_STRING aParameterName,
                                            OMX_INDEXTYPE *apIndexType)
{
    ALOGV("GetIndexExtension %s", aParameterName);

    if (strncmp(aParameterName, sOmxOsiSharedChunkMetadata, 40) == 0) {
        *apIndexType = (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata;
        return OMX_ErrorNone;
    }

#ifdef ANDROID
    if (strncmp(aParameterName, "OMX.google.android.index.enableAndroidNativeBuffers", 51) == 0) {
        *apIndexType = (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers;
        return OMX_ErrorNone;
    }

    if (strncmp(aParameterName, "OMX.google.android.index.getAndroidNativeBufferUsage", 52) == 0) {
        *apIndexType = (OMX_INDEXTYPE)OMX_google_android_index_getAndroidNativeBufferUsage;
        return OMX_ErrorNone;
    }

    if (strncmp(aParameterName, "OMX.google.android.index.useAndroidNativeBuffer2", 48) == 0) {
        return OMX_ErrorNotImplemented;
    }

    if (strncmp(aParameterName, "OMX.google.android.index.useAndroidNativeBuffer", 47) == 0) {
        *apIndexType = (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer;
        return OMX_ErrorNone;
    }

    if (strncmp(aParameterName, "OMX.google.android.index.storeMetaDataInBuffers", 47) == 0) {
        *apIndexType = (OMX_INDEXTYPE)OMX_google_android_index_storeMetaDataInBuffers;
        return OMX_ErrorNone;
    }
#endif //ANDROID

    return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE MMHwBuffer::GetPortIndexExtension(OMX_INDEXTYPE aIndex,
                                                OMX_PTR apConf,
                                                OMX_U32 &aPortIndex)
{

    if (aIndex == (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata) {
        struct OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *shared =
            static_cast<struct OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *>(apConf);
        aPortIndex = shared->nPortIndex;
        return OMX_ErrorNone;
    }

#ifdef ANDROID
    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers) {
        struct EnableAndroidNativeBuffersParams *params =
            static_cast<struct EnableAndroidNativeBuffersParams *>(apConf);
        aPortIndex = params->nPortIndex;
        return OMX_ErrorNone;
    }

    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_getAndroidNativeBufferUsage) {
        struct GetAndroidNativeBufferUsageParams *params =
            static_cast<struct GetAndroidNativeBufferUsageParams *>(apConf);
        aPortIndex = params->nPortIndex;
        return OMX_ErrorNone;
    }

    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer) {
        struct UseAndroidNativeBufferParams *params =
            static_cast<struct UseAndroidNativeBufferParams *>(apConf);
        aPortIndex = params->nPortIndex;
        return OMX_ErrorNone;
    }
#endif //ANDROID

    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE MMHwBuffer::GetAllocatedSize(OMX_HANDLETYPE aComponent,
                                           OMX_U32 &aAllocatedSize)
{
    return MMHwBufferLinux::GetAllocatedSize(aComponent, aAllocatedSize);
}

OMX_VERSIONTYPE MMHwBuffer::Version(void)
{
    OMX_VERSIONTYPE version;
    version.s.nVersionMajor = 1;
    version.s.nVersionMinor = 1;
    version.s.nRevision     = 1;
    version.s.nStep         = 0;
    return version;
}

/********************************************************************************
 * MMHwBufferLinux specific members implementation
 ********************************************************************************/

MMHwBufferLinux::MMHwBufferLinux(MMHwBufferPool *aPool, OMX_HANDLETYPE aComponent)
{
    pthread_mutex_init(&mutex, NULL);
    mBufferPool = aPool;
    mBufferList = NULL;
    mNbRef = 1;
    mComponent = aComponent;
    mPortIndex = (OMX_U32)-1;
    addSelfToList();
}

MMHwBufferLinux::MMHwBufferLinux(MMHwBufferList *aList,
                                 OMX_HANDLETYPE aComponent,
                                 OMX_U32 aPortIndex)
{
    pthread_mutex_init(&mutex, NULL);
    mBufferPool = NULL;
    mBufferList = aList;
    mNbRef = 1;
    mComponent = aComponent;
    mPortIndex = aPortIndex;
    addSelfToList();
}

/* !!! Global lock must be held when destroying object !!! */
MMHwBufferLinux::~MMHwBufferLinux()
{
    pthread_mutex_destroy(&mutex);
    if (mBufferPool) {
        delete mBufferPool;
    }
    if (mBufferList) {
        delete mBufferList;
    }
    LIST_REMOVE(&list_elem, list_entry);
}

void MMHwBufferLinux::addSelfToList()
{
    list_elem.mHwBuffer = this;
    GlobalLock();
    LIST_INSERT_HEAD(&mMMHwBufferList, &list_elem, list_entry);
    GlobalUnlock();
}

OMX_ERRORTYPE MMHwBufferLinux::CacheSync(TSyncCacheOperation aOp,
                                         OMX_U32 aLogAddr,
                                         OMX_U32 aSize,
                                         OMX_U32 &aPhysAddr)
{
    ALOGV("bHandle=0x%08x CacheSync(aOp=%d, aLogAddr=0x%08x, aSize=%d)", (unsigned int)this,
         (int)aOp, (unsigned int)aLogAddr, (int)aSize);
    if (mBufferPool) {
        return mBufferPool->CacheSync(aOp, aLogAddr, aSize, aPhysAddr);
    }
    if (mBufferList) {
        return mBufferList->CacheSync(aOp, aLogAddr, aSize, aPhysAddr);
    }
    ALOGE("CacheSync: OMX_ErrorUndefined");
    return OMX_ErrorUndefined;
}

OMX_ERRORTYPE MMHwBufferLinux::AddBufferInfo(OMX_U32 aBufferIndex, OMX_U32 aLogAddr, OMX_U32 aSize)
{
    TBufferInfo nInfo;
    OMX_ERRORTYPE error = OMX_ErrorUndefined;

    ALOGV("bHandle=0x%08x AddBufferInfo(aBufferIndex=%d, aLogAddr=0x%08x, aSize=%d)",
         (unsigned int)this, (int)aBufferIndex, (unsigned int)aLogAddr, (int)aSize);

    if (mBufferPool) {
        error = mBufferPool->BufferInfoInd(aBufferIndex, nInfo);
    }
    if (mBufferList) {
        MMHwBufferPool *first = mBufferList->getLastBufferPool();
        if (first) {
            error = first->BufferInfoInd(0, nInfo);
        }
    }
    if (error) {
        ALOGE("AddBufferInfo error = %d", (int)error);
        return error;
    }
    if (nInfo.iLogAddr != aLogAddr) {
        ALOGE("AddBufferInfo(aBufferIndex=%d, aLogAddr=0x%08x, aSize=%d) != iLogAddr=0x%08x",
             (int)aBufferIndex, (unsigned int)aLogAddr, (int)aSize,
             (unsigned int)nInfo.iLogAddr);
        return OMX_ErrorBadParameter;
    }
    if (nInfo.iAllocatedSize != aSize) {
        ALOGE("AddBufferInfo(aBufferIndex=%d, aLogAddr=0x%08x, aSize=%d) != iAllocatedSize=%d",
             (int)aBufferIndex, (unsigned int)aLogAddr, (int)aSize, (int)nInfo.iAllocatedSize);
        return OMX_ErrorBadParameter;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBufferLinux::SetConfigExtension(OMX_HANDLETYPE aTunneledComponent,
                                                  OMX_U32 aPortIndex)
{
    ALOGV("SetConfigExtension(aTunneledComponent=0x%08x, aPortIndex=%d)",
         (unsigned int)aTunneledComponent, (int)aPortIndex);

    if (mBufferPool) {
        // Set the configuration of the shared chunk to the tunneled component
        OMX_OSI_CONFIG_SHARED_CHUNK_METADATA conf;

        GetConfigExtension((OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                           (OMX_PTR)&conf, aPortIndex);

        OMX_ERRORTYPE error = OMX_SetConfig(aTunneledComponent,
                                            (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                                            &conf);
        if (error) {
            ALOGE("SetConfigExtension(aTunneledComponent=0x%08x, aPortIndex=%d) "
                 "got error %d on call to OMX_SetConfig",
                 (unsigned int)aTunneledComponent, (int)aPortIndex, (int)error);
            return error;
        }
        return OMX_ErrorNone;
    }

    return OMX_ErrorUndefined;
}

OMX_ERRORTYPE MMHwBufferLinux::GetConfigExtension(OMX_INDEXTYPE aIndex,
                                                  OMX_PTR apConf,
                                                  OMX_U32 aPortIndex) const
{
    ALOGV("GetConfigExtension(aIndex=0x%08x, apConf=0x%08x, aPortIndex=%d)",
         (unsigned int)aIndex, (unsigned int)apConf, (int)aPortIndex);

    if (aIndex == (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata) {
        OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *shared =
            static_cast<OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *>(apConf);

        shared->nSize             = sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);
        shared->nVersion          = Version();
        shared->nPortIndex        = aPortIndex;
        shared->nHandleId         = (OMX_U32) this;
        shared->nOwnerPID         = (OMX_U32)getpid();

        if (mBufferPool) {
            mBufferPool->SetConfigExtension(shared);
            return OMX_ErrorNone;
        }
        if (mBufferList) {
            MMHwBufferPool *first = mBufferList->getLastBufferPool();
            //Be carefull that with current implmentation what OSI extension
            //retrieves is the last pushed buffer configuration in case of
            //Native Buffer handles.
            if (first) {
                first->SetConfigExtension(shared);
                return OMX_ErrorNone;
            }
        }
        ALOGE("GetConfigExtension called on NULL buffer pool or list or empty list");
        return OMX_ErrorUndefined;
    }

#ifdef ANDROID
    if (aIndex == (OMX_INDEXTYPE)OMX_google_android_index_getAndroidNativeBufferUsage) {
        struct GetAndroidNativeBufferUsageParams *params =
            static_cast<struct GetAndroidNativeBufferUsageParams *>(apConf);
        params->nUsage =
            GRALLOC_USAGE_SW_READ_OFTEN |
            GRALLOC_USAGE_SW_WRITE_OFTEN |
            GRALLOC_USAGE_HW_TEXTURE |
            GRALLOC_USAGE_HW_RENDER |
            GRALLOC_USAGE_HW_2D;
        return OMX_ErrorNone;
    }
#endif //ANDROID

    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE MMHwBufferLinux::BufferInfo(OMX_U32 aBufferIndex, TBufferInfo &aInfo)
{
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    if (mBufferPool) {
        error = mBufferPool->BufferInfoInd(aBufferIndex, aInfo);
    }
    if (mBufferList) {
        error = mBufferList->BufferInfoInd(aBufferIndex, aInfo);
    }
    if (error) {
        ALOGE("BufferInfo(aBufferIndex=%d) returning with error=%d",
             (int)aBufferIndex, (int)error);
    }
    ALOGV("BufferInfo(aBufferIndex=%d) => iLogAddr=0x%08x iPhyAddr=0x%08x "
         "iAllocatedSize=%d iCacheAttr=%d", (int)aBufferIndex,
         (unsigned int)aInfo.iLogAddr, (unsigned int)aInfo.iPhyAddr,
         (int)aInfo.iAllocatedSize, (int)aInfo.iCacheAttr);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBufferLinux::CacheSync(TSyncCacheOperation aOp,
                                         OMX_U32 aBufferIndex,
                                         OMX_U32 &aPhysAddr)
{
    ALOGV("CacheSync(aOp=%d, aBufferIndex=%d)", (int)aOp, (int)aBufferIndex);

    TBufferInfo info;
    OMX_ERRORTYPE error = BufferInfo(aBufferIndex, info);

    if (error) {
        return error;
    }

    return CacheSync(aOp, info.iLogAddr, info.iAllocatedSize, aPhysAddr);
}

OMX_ERRORTYPE MMHwBufferLinux::PhysAddress(OMX_U32 aLogAddr,
                                           OMX_U32 aSize,
                                           OMX_U32 &aPhyAddr)
{
    TBufferInfo nInfo;
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    if (mBufferPool) {
        error = mBufferPool->BufferInfoLog(aLogAddr, aSize, nInfo);
    }
    if (mBufferList) {
        error = mBufferList->BufferInfoLog(aLogAddr, aSize, nInfo);
    }
    if (error) {
        ALOGE("PhysAddress returning error=%d", (int)error);
        return error;
    }
    aPhyAddr = nInfo.iPhyAddr;
    ALOGV("PhysAddress(aLogAddr=0x%08x, aSize=%d) => aPhyAddr=0x%08x",
         (unsigned int)aLogAddr, (int)aSize, (unsigned int)aPhyAddr);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBufferLinux::LogAddress(OMX_U32 aPhyAddr,
                                          OMX_U32 aSize,
                                          OMX_U32 &aLogAddr)
{
    if (mBufferPool) {
        TBufferInfo nInfo;
        OMX_ERRORTYPE error = mBufferPool->BufferInfoPhy(aPhyAddr, aSize, nInfo);
        if (error) {
            return error;
        }
        aLogAddr = nInfo.iLogAddr;
        ALOGV("LogAddress(aPhyAddr=0x%08x, aSize=%d) => aLogAddr=0x%08x",
             (unsigned int)aPhyAddr, (int)aSize, (unsigned int)aLogAddr);
        return OMX_ErrorNone;
    }
    ALOGE("LogAddress called on NULL buffer pool");
    return OMX_ErrorUndefined;
}

MMHwBufferLinux * MMHwBufferLinux::getMMHwBufferLinux(OMX_HANDLETYPE aComponent, OMX_U32 aPortIndex)
{
    MMHwBufferLinux_list_elem_t *elem;
    MMHwBufferLinux *result = NULL;
    GlobalLock();
    LIST_FOREACH(elem, &mMMHwBufferList, list_entry) {
        if (elem->mHwBuffer->mBufferList &&
            (elem->mHwBuffer->mComponent == aComponent) &&
            (elem->mHwBuffer->mPortIndex == aPortIndex)) {
            result = elem->mHwBuffer;
            break;
        }
    }
    GlobalUnlock();
    return result;
}

OMX_U32 MMHwBufferLinux::getTotalSize()
{
    if (mBufferPool) { return mBufferPool->getTotalSize(); }
    if (mBufferList) { return mBufferList->getTotalSize(); }
    return 0;
}

OMX_U32 MMHwBufferLinux::dump()
{
    ALOGI("  bHandle     = 0x%08x", (unsigned int)this);
    ALOGI("  mComponent  = 0x%08x", (unsigned int)mComponent);
#if DUMP_MMHWBUFFERLINUX
    if (mBufferPool) { return mBufferPool->dump(); }
    if (mBufferList) { return mBufferList->dump(); }
#endif
    return 0;
}

void MMHwBufferLinux::dumpMMHwBufferLinux()
{
    MMHwBufferLinux_list_elem_t *elem;
    OMX_U32 totalSize = 0;
    OMX_U32 nbMMHwBuffer = 0;
    GlobalLock();
    ALOGI("--dumpMMHwBufferLinux:");
    LIST_FOREACH(elem, &mMMHwBufferList, list_entry) {
        totalSize += elem->mHwBuffer->dump();
        nbMMHwBuffer++;
    }
    ALOGI("--Total: nbMMHwBuffer=%d totalSize=%d", (int)nbMMHwBuffer, (int)totalSize);
    GlobalUnlock();
}

OMX_ERRORTYPE MMHwBufferLinux::GetAllocatedSize(OMX_HANDLETYPE aComponent,
                                                OMX_U32 &aAllocatedSize)
{
    MMHwBufferLinux_list_elem_t *elem;
    OMX_ERRORTYPE error = OMX_ErrorBadParameter;

    aAllocatedSize = 0;

    GlobalLock();
    LIST_FOREACH(elem, &mMMHwBufferList, list_entry) {
        if (elem->mHwBuffer->mComponent == aComponent) {
            aAllocatedSize += elem->mHwBuffer->getTotalSize();
            error = OMX_ErrorNone;
        }
    }
    GlobalUnlock();
    return error;
}

bool MMHwBufferLinux::isValidHandle(MMHwBufferLinux *handle)
{
    MMHwBufferLinux_list_elem_t *elem;
    LIST_FOREACH(elem, &mMMHwBufferList, list_entry) {
        if (elem->mHwBuffer == handle) {
            return true;
        }
    }
    return false;
}
