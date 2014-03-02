/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//#define LOG_NDEBUG 0
#define LOG_TAG "MMHwBufferPool"
#include "linux_utils.h"

#include "MMHwBufferPool.h"
#include "OMX_Component.h"

#ifndef WORKSTATION
#include <linux/hwmem.h>
static const char hwmem_files_full_name[] = "/dev/" HWMEM_DEFAULT_DEVICE_NAME;
#endif

#define HWMEM_ALIGN_SIZE 4096
#define MAX(a, b) ((int)a > (int)b ? a : b)

#define ALIGN(x, i) ALIGN_MASK(x, (typeof(x))(i) - 1)
#define ALIGN_MASK(x, m) (((x) + (m)) & ~(m))

pthread_mutex_t MMHwBufferPool::globalMutex = PTHREAD_MUTEX_INITIALIZER;
OMX_U32 MMHwBufferPool::mTotalVideoDecoder = 0;

MMHwBufferPool::MMHwBufferPool()
{
    mNativeBuffer = NULL;
    hwmemfd = -1;
    mHandleName = -1;
    mapDone = false;
    mTotalSize = 0;
    mBaseOffset = 0;
    mLogicalAddress = 0;
    mPhysicalAddress = 0;
    mVideoDecoderSize = 0;
    list_elem.mPool = this;
}

MMHwBufferPool::MMHwBufferPool(void *pNativeBuffer)
{
#ifdef ANDROID
    mNativeBuffer = new MMNativeBuffer(pNativeBuffer);
    if (mNativeBuffer->init() < 0) {
        ALOGE("%s: MMNativeBuffer::init() failed", __func__);
    }
    hwmemfd = mNativeBuffer->getHandle();
    mHandleName = mNativeBuffer->getExportName();

    // setup the attributes
    mBuffPoolAttrs.iBuffers    = 1;
    mBuffPoolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
    mBuffPoolAttrs.iSize       = mNativeBuffer->getBufferSize();
    mBuffPoolAttrs.iAlignment  = 256;
    mBuffPoolAttrs.iCacheAttr  = MMHwBuffer::ENormalUnCached;

    mBufferSize = ALIGN(MAX(mBuffPoolAttrs.iAlignment, mBuffPoolAttrs.iSize),
                        mBuffPoolAttrs.iAlignment);
    mapDone = false;
    mTotalSize = mBufferSize;
    mLogicalAddress = mNativeBuffer->getLogicalAddress();
    mPhysicalAddress = mNativeBuffer->getPhysicalAddress();
    mBaseOffset = ALIGN(mLogicalAddress, mBuffPoolAttrs.iAlignment) - mLogicalAddress;
    if ((ALIGN(mPhysicalAddress, mBuffPoolAttrs.iAlignment) - mPhysicalAddress) != mBaseOffset) {
        ALOGE("map: Physical and Logical address does not have the same alignement\n");
    }
#else
    mNativeBuffer = NULL;
    hwmemfd = -1;
    mHandleName = -1;
    mBuffPoolAttrs = MMHwBuffer::TBufferPoolCreationAttributes::TBufferPoolCreationAttributes();
    mapDone = false;
    mTotalSize = 0;
    mBaseOffset = 0;
    mLogicalAddress = 0;
    mPhysicalAddress = 0;
#endif
    mVideoDecoderSize = 0;
    list_elem.mPool = this;
}

MMHwBufferPool::~MMHwBufferPool()
{
    ALOGV("~MMHwBufferPool bPool=0x%08x)", (unsigned int)this);
    unmap();
    Close();
}

OMX_ERRORTYPE MMHwBufferPool::Open()
{
    if (MMNativeBuffer::Open((int*)&hwmemfd, NULL, NULL) < 0) {
        return OMX_ErrorUndefined;
    } else {
        return OMX_ErrorNone;
    }
}

OMX_ERRORTYPE MMHwBufferPool::Open(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *sharedConf)
{
    if (MMNativeBuffer::Open((int*)&hwmemfd, (int*)&sharedConf->nHandleName, (int*)&mTotalSize) < 0) {
        return OMX_ErrorUndefined;
    }

    // update the buffer pool attributes
    mBuffPoolAttrs.iBuffers    = sharedConf->nBufferCount;
    mBuffPoolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
    mBuffPoolAttrs.iSize       = sharedConf->nBufferSize;
    mBuffPoolAttrs.iAlignment  = sharedConf->nAlignment;
    mBuffPoolAttrs.iCacheAttr  = (MMHwBuffer::TCacheAttribute)sharedConf->nCacheAttr;

    mBufferSize = ALIGN(MAX(mBuffPoolAttrs.iAlignment, mBuffPoolAttrs.iSize),
                        mBuffPoolAttrs.iAlignment);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBufferPool::prepare(const MMHwBuffer::TBufferPoolCreationAttributes &
                                      aBuffPoolAttrs, OMX_HANDLETYPE aComponent)
{
    OMX_ERRORTYPE error = Open();

    if (error) {
        return error;
    }
    error = allocate(aBuffPoolAttrs, aComponent);
    if (error) {
        Close();
        return error;
    }
    error = map();
    if (error) {
        Close();
        return error;
    }
    return error;
}

#ifdef WORKSTATION

OMX_ERRORTYPE MMHwBufferPool::allocate(const MMHwBuffer::TBufferPoolCreationAttributes &
                                       aBuffPoolAttrs, OMX_HANDLETYPE aComponent)
{
    mBufferSize = ALIGN(MAX(mBuffPoolAttrs.iAlignment, mBuffPoolAttrs.iSize),
                        mBuffPoolAttrs.iAlignment);
    mTotalSize = mBufferSize * mBuffPoolAttrs.iBuffers;
    mLogicalAddress = (OMX_U32)malloc(mTotalSize);
    return OMX_ErrorNone;
}

#else

OMX_ERRORTYPE MMHwBufferPool::allocate(const MMHwBuffer::TBufferPoolCreationAttributes &
                                       aBuffPoolAttrs, OMX_HANDLETYPE aComponent)
{
    if (hwmemfd < 0) {
        ALOGE("allocate: called on non open device\n");
        return OMX_ErrorUndefined;
    }
    if (mTotalSize) {
        ALOGE("allocate: failed as buffer pool already allocated\n");
        return OMX_ErrorUndefined;
    }
    mBuffPoolAttrs = aBuffPoolAttrs;
    if (mBuffPoolAttrs.iAlignment <= 0) {
        mBuffPoolAttrs.iAlignment = HWMEM_ALIGN_SIZE;
    }
    /* Compute the amount of memory to allocate based on the alignment constraints:
     *  iAlignment => alignment applied to the base address of each buffer in the pool
     *  iSize      => Size in byte of a buffer */
    mBufferSize = ALIGN(MAX(mBuffPoolAttrs.iAlignment, mBuffPoolAttrs.iSize),
                        mBuffPoolAttrs.iAlignment);
    mTotalSize = mBufferSize * mBuffPoolAttrs.iBuffers;
    if (mBuffPoolAttrs.iAlignment > HWMEM_ALIGN_SIZE) {
        // Allocate an extra buffer to make sure we'll have space to align
        mTotalSize += mBufferSize;
    }

    ALOGV("allocate(mTotalSize=%u, cacheAttr=%d)\n",
         (unsigned int)mTotalSize, (int)mBuffPoolAttrs.iCacheAttr);

    if(!isAllowedToAllocate(aComponent)) {
        return OMX_ErrorInsufficientResources;
    }

    struct hwmem_alloc_request alloc_request;

    switch (mBuffPoolAttrs.iCacheAttr) {
    case MMHwBuffer::ENormalUnCached:
        alloc_request.flags = HWMEM_ALLOC_HINT_UNCACHED | HWMEM_ALLOC_HINT_WRITE_COMBINE;
        break;

    case MMHwBuffer::EFullyBlocking:
        alloc_request.flags = HWMEM_ALLOC_HINT_NO_WRITE_COMBINE | HWMEM_ALLOC_HINT_UNCACHED;
        break;

    default:
        alloc_request.flags = HWMEM_ALLOC_HINT_CACHED;
    }

    alloc_request.size           = mTotalSize;
    alloc_request.default_access = HWMEM_ACCESS_READ | HWMEM_ACCESS_WRITE | HWMEM_ACCESS_IMPORT;
    alloc_request.mem_type       = HWMEM_MEM_CONTIGUOUS_SYS;

    // Perform the allocation
    if (ioctl(hwmemfd, HWMEM_ALLOC_FD_IOC, &alloc_request) != 0) {
        Close();
        return OMX_ErrorUndefined;
    }

    //get handle name of buffer to be exported to another processes
    mHandleName = ioctl(hwmemfd, HWMEM_EXPORT_IOC, 0);
    if (mHandleName < 0) {
        ALOGE("allocate: HWMEM_EXPORT_IOC failed\n");
        mHandleName = 0;
        Close();
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;
}

#endif  // #ifdef WORKSTATION

OMX_ERRORTYPE MMHwBufferPool::Close()
{
    if (hwmemfd < 0) {
        ALOGE("Close: called on non open device\n");
        return OMX_ErrorUndefined;
    }
    if (mTotalSize == 0) {
        ALOGE("Close: called on non allocated buffer pool\n");
        return OMX_ErrorUndefined;
    }
    if (mapDone) {
        unmap();
    }
    if (mNativeBuffer) {
        delete mNativeBuffer;
        mNativeBuffer = NULL;
        hwmemfd = -1;
        mPhysicalAddress = 0;
        mLogicalAddress = 0;
    } else {
        close(hwmemfd);
        hwmemfd = -1;
    }
    mTotalSize = 0;
    if (mVideoDecoderSize) {
        Lock();
        mTotalVideoDecoder -= mVideoDecoderSize;
        ALOGD("mTotalVideoDecoder = 0x%08x", (unsigned int)mTotalVideoDecoder);
        mVideoDecoderSize = 0;
        Unlock();
    }
    return OMX_ErrorNone;
}

#ifdef WORKSTATION

OMX_ERRORTYPE MMHwBufferPool::map() { return OMX_ErrorNone; }

OMX_ERRORTYPE MMHwBufferPool::unmap() { return OMX_ErrorNone; }

OMX_ERRORTYPE MMHwBufferPool::CacheSync(MMHwBuffer::TSyncCacheOperation aOp,
                                        OMX_U32 aLogAddr,
                                        OMX_U32 aSize,
                                        OMX_U32 &aPhysAddr)
{ return OMX_ErrorNone; }

#else

OMX_ERRORTYPE MMHwBufferPool::map()
{
    if (hwmemfd < 0) {
        ALOGE("map: called on non open device\n");
        return OMX_ErrorUndefined;
    }
    if (mTotalSize == 0) {
        ALOGE("map: called while mTotalSize is 0\n");
        return OMX_ErrorUndefined;
    }
    if (mapDone) {
        ALOGE("map: called on mapped buffer pool\n");
        return OMX_ErrorUndefined;
    }

    mLogicalAddress = (OMX_U32)MMNativeBuffer::Map(hwmemfd, mTotalSize);

    if (mLogicalAddress == (OMX_U32)MAP_FAILED) {
        mLogicalAddress = 0;
        ALOGE("map: mmap failed\n");
        return OMX_ErrorUndefined;
    }

    // pin buffer to get physical address
    if (MMNativeBuffer::Pin(hwmemfd, &mPhysicalAddress) < 0) {
        ALOGE("map: Pin failed\n");
        if (MMNativeBuffer::UnMap((void *)mLogicalAddress, mTotalSize) < 0) {
            ALOGE("map: munmap failed\n");
        }
        mLogicalAddress = 0;
        return OMX_ErrorUndefined;
    }

    mBaseOffset = ALIGN(mLogicalAddress, mBuffPoolAttrs.iAlignment) - mLogicalAddress;
    if ((ALIGN(mPhysicalAddress, mBuffPoolAttrs.iAlignment) - mPhysicalAddress) != mBaseOffset) {
        ALOGE("map: Physical and Logical address does not have the same alignement\n");
        return OMX_ErrorUndefined;
    }
    mapDone = true;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MMHwBufferPool::unmap()
{
    if (mNativeBuffer) {
        return OMX_ErrorNone;
    }
    if (hwmemfd < 0) {
        ALOGE("unmap: called on non open device\n");
        return OMX_ErrorUndefined;
    }
    if (!mapDone) {
        ALOGE("unmap: called on non mapped buffer pool\n");
        return OMX_ErrorUndefined;
    }

    if (MMNativeBuffer::UnMap((void *)mLogicalAddress, mTotalSize) < 0) {
        ALOGE("unmap: munmap failed\n");
    }

    if (MMNativeBuffer::UnPin(hwmemfd) != 0) {
        // FIXME: error is always returned ??
        //LOGE("unmap: HWMEM_UNPIN_IOC failed\n");
    }
    mPhysicalAddress = 0;
    mLogicalAddress = 0;
    mapDone = false;
    return OMX_ErrorNone;
}

static inline void prepare_hwmem_set_domain_req(struct hwmem_set_domain_request *req,
                                                OMX_U32 offset,
                                                OMX_U32 size,
                                                OMX_U32 access)
{
    req->id            = 0;
    req->access        = access;
    req->region.offset = offset;
    req->region.count  = 1;
    req->region.start  = 0;
    req->region.end    = size;
    req->region.size   = size;
}

OMX_ERRORTYPE MMHwBufferPool::CacheSync(MMHwBuffer::TSyncCacheOperation aOp,
                                        OMX_U32 aLogAddr,
                                        OMX_U32 aSize,
                                        OMX_U32 &aPhysAddr)
{
    MMHwBuffer::TBufferInfo nInfo;
    OMX_ERRORTYPE error = BufferInfoLog(aLogAddr, aSize, nInfo);
    if (error) {
        return error;
    }
    aPhysAddr = nInfo.iPhyAddr;
    return CacheSync(hwmemfd, aOp, mBaseOffset + getIndexLog(aLogAddr) * mBufferSize, aSize);
}

OMX_ERRORTYPE MMHwBufferPool::CacheSync(OMX_S32 hwmemfd,
                                        MMHwBuffer::TSyncCacheOperation aOp,
                                        OMX_U32 offset,
                                        OMX_U32 size)
{
    ALOGV("CacheSync(fd=%d, aOp=%d, offset=0x%08x, size=%d)\n",
         (int)hwmemfd, (int)aOp, (unsigned int)offset, (int)size);

    struct hwmem_set_domain_request hwmem_set_domain_req;

    switch (aOp) {
    case MMHwBuffer::ESyncBeforeReadHwOperation:
        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_READ | HWMEM_ACCESS_WRITE);

        if (ioctl(hwmemfd, HWMEM_SET_CPU_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }

        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_READ);

        if (ioctl(hwmemfd, HWMEM_SET_SYNC_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }
        break;

    case MMHwBuffer::ESyncBeforeWriteHwOperation:
        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_READ | HWMEM_ACCESS_WRITE);

        if (ioctl(hwmemfd, HWMEM_SET_CPU_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }

        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_WRITE);

        if (ioctl(hwmemfd, HWMEM_SET_SYNC_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }
        break;

    case MMHwBuffer::ESyncAfterWriteHwOperation:
        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_READ | HWMEM_ACCESS_WRITE);

        if (ioctl(hwmemfd, HWMEM_SET_CPU_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }

        prepare_hwmem_set_domain_req(&hwmem_set_domain_req, offset, size,
                                     HWMEM_ACCESS_READ | HWMEM_ACCESS_WRITE);

        if (ioctl(hwmemfd, HWMEM_SET_SYNC_DOMAIN_IOC, &hwmem_set_domain_req) < 0) {
            return OMX_ErrorBadParameter;
        }
        break;

    default:
        return OMX_ErrorBadParameter;
    }
    return OMX_ErrorNone;
}

#endif  // #ifdef WORKSTATION

void MMHwBufferPool::SetConfigExtension(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA *conf)
{
    conf->nFd               = hwmemfd;
    conf->nBaseLogicalAddr  = mLogicalAddress;
    conf->nHandleName       = mHandleName;
    conf->nBufferCount      = mBuffPoolAttrs.iBuffers;
    conf->nBufferSize       = mBuffPoolAttrs.iSize;
    conf->nCacheAttr        = mBuffPoolAttrs.iCacheAttr;
    conf->nChunkSize        = mTotalSize;
    conf->nAlignment        = mBuffPoolAttrs.iAlignment;
}

OMX_ERRORTYPE MMHwBufferPool::BufferInfoInd(OMX_U32 aBufferIndex,
                                            MMHwBuffer::TBufferInfo &aInfo)
{
    if (aBufferIndex >= mBuffPoolAttrs.iBuffers) {
        return OMX_ErrorBadParameter;
    }

    OMX_U32 offset = mBaseOffset + aBufferIndex * mBufferSize;
    aInfo.iLogAddr       = mLogicalAddress + offset;
    aInfo.iPhyAddr       = mPhysicalAddress + offset;
    aInfo.iAllocatedSize = mBuffPoolAttrs.iSize;
    aInfo.iCacheAttr     = mBuffPoolAttrs.iCacheAttr;

    if (aInfo.iLogAddr != ALIGN(aInfo.iLogAddr, mBuffPoolAttrs.iAlignment)) {
        ALOGE("BufferInfoInd: invalid alignement for aBufferIndex=%d "
             "LogAddr=0x%08x iAlignment=%d\n",
             (int)aBufferIndex, (unsigned int)aInfo.iLogAddr,
             (int)mBuffPoolAttrs.iAlignment);
        dump();
        return OMX_ErrorBadParameter;
    }

    if (aInfo.iPhyAddr != ALIGN(aInfo.iPhyAddr, mBuffPoolAttrs.iAlignment)) {
        ALOGE("BufferInfoInd: invalid alignement for aBufferIndex=%d "
             "PhyAddr=0x%08x iAlignment=%d\n",
             (int)aBufferIndex, (unsigned int)aInfo.iPhyAddr,
             (int)mBuffPoolAttrs.iAlignment);
        dump();
        return OMX_ErrorBadParameter;
    }

    return OMX_ErrorNone;
}

#ifdef ANDROID
  #define STE_VIDEO_DECODER_MAX_HWMEM "ste.video.decoder.max.hwmem"
#else
  #define STE_VIDEO_DECODER_MAX_HWMEM "ste_video_decoder_max_hwmem"
#endif

bool MMHwBufferPool::isAllowedToAllocate(OMX_HANDLETYPE aComponent)
{
    GET_PROPERTY(STE_VIDEO_DECODER_MAX_HWMEM, value, "0");
    unsigned long int limit = strtoul(value, NULL, 0);

    if ((aComponent == NULL) || (limit == 0)) {
        return true;
    }

    OMX_PARAM_COMPONENTROLETYPE param;
    param.nSize =  sizeof(OMX_PARAM_COMPONENTROLETYPE);
    param.nVersion.nVersion = OMX_VERSION;

    OMX_GetParameter(aComponent, OMX_IndexParamStandardComponentRole, &param);

    ALOGI("Role of hComponent=0x%08x : %s", (unsigned int)aComponent, param.cRole);

    bool allowed = true;

    if (!strncmp((const char *)param.cRole, "video_decoder.", 14)) {
        Lock();
        if (mTotalVideoDecoder + mTotalSize <= limit) {
            // We are allowed to allocate : accumulate and keep track for close
            mVideoDecoderSize = mTotalSize;
            mTotalVideoDecoder += mVideoDecoderSize;
            ALOGD("mTotalVideoDecoder = 0x%08x", (unsigned int)mTotalVideoDecoder);
        } else {
            ALOGE(STE_VIDEO_DECODER_MAX_HWMEM " limit of 0x%08x reached when requesting 0x%08x "
                 "for video_decoder on top of already allocated 0x%08x bytes",
                 (unsigned int)limit, (unsigned int)mTotalSize, (unsigned int)mTotalVideoDecoder);
            allowed = false;
        }
        Unlock();
    }

    return allowed;
}

OMX_U32 MMHwBufferPool::getTotalSize()
{
    return mTotalSize;
}

OMX_U32 MMHwBufferPool::dump()
{
    ALOGI("  bPool       = 0x%08x", (unsigned int)this);
    ALOGI("  hwmemfd     = %d", (int)hwmemfd);
    ALOGI("  mHandleName = %d", (int)mHandleName);
    ALOGI("  iBuffers    = %d", (int)mBuffPoolAttrs.iBuffers);
    ALOGI("  iSize       = %d", (int)mBuffPoolAttrs.iSize);
    ALOGI("  iAlignment  = %d", (int)mBuffPoolAttrs.iAlignment);
    ALOGI("  mapDone     = %d", (mapDone ? 1 : 0));
    ALOGI("  mBufferSize = %d", (int)mBufferSize);
    ALOGI("  mTotalSize  = %d", (int)mTotalSize);
    ALOGI("  mBaseOffset = %d", (int)mBaseOffset);
    ALOGI("  mLogicalAddress  = 0x%08x", (unsigned int)mLogicalAddress);
    ALOGI("  mPhysicalAddress = 0x%08x", (unsigned int)mPhysicalAddress);
    ALOGI("  ");
    return mTotalSize;
}
