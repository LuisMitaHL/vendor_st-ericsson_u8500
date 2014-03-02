/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "MMHWBufferList"
#include "linux_utils.h"

#include "MMHwBufferList.h"
#include "mmhwbuffer_ext.h"

#ifdef ANDROID
#include "media/hardware/HardwareAPI.h"
using namespace android;
#endif //ANDROID

MMHwBufferList::MMHwBufferList() {}

MMHwBufferList::~MMHwBufferList()
{
    MMHwBufferPool_list_elem *iter_elem;
    MMHwBufferPool_list_elem *next_elem;
    ALOGV("~MMHwBufferList bList=0x%08x", (unsigned int)this);
    CIRCLEQ_FOREACH_SAFE(iter_elem, &mList, list_entry, next_elem) {
        delete iter_elem->mPool;
    }
}

OMX_ERRORTYPE MMHwBufferList::Init()
{
    CIRCLEQ_INIT(&mList, MMHwBufferPool_list_elem);

    return OMX_ErrorNone;
}

MMHwBufferPool * MMHwBufferList::getLastBufferPool()
{
    if (CIRCLEQ_EMPTY(&mList, MMHwBufferPool_list_elem)) {
        return NULL;
    }
    return CIRCLEQ_LAST(&mList)->mPool;
}

OMX_ERRORTYPE MMHwBufferList::BufferInfoLog(OMX_U32 aLogAddr,
                                            OMX_U32 aSize,
                                            MMHwBuffer::TBufferInfo &aInfo)
{
    MMHwBufferPool_list_elem *iter_elem;
    CIRCLEQ_FOREACH(iter_elem, &mList, list_entry) {
        iter_elem->mPool->BufferInfoInd(0, aInfo);
        if (aLogAddr == aInfo.iLogAddr) {
            return OMX_ErrorNone;
        }
    }
    ALOGE("%s: could not find in 0x%08x a match for aLogAddr=%d",
         __func__, (unsigned int)this, (unsigned int)aLogAddr);
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE MMHwBufferList::BufferInfoInd(OMX_U32 aBufferIndex, MMHwBuffer::TBufferInfo &aInfo)
{
    MMHwBufferPool_list_elem *iter_elem;
    unsigned int index = 0;
    CIRCLEQ_FOREACH(iter_elem, &mList, list_entry) {
        if (index++ == aBufferIndex) {
            iter_elem->mPool->BufferInfoInd(0, aInfo);
            return OMX_ErrorNone;
        }
    }
    ALOGE("%s: could not find in 0x%08x a match for aBufferIndex=%d",
         __func__, (unsigned int)this, (int)aBufferIndex);
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE MMHwBufferList::CacheSync(MMHwBuffer::TSyncCacheOperation aOp,
                                        OMX_U32 aLogAddr,
                                        OMX_U32 aSize,
                                        OMX_U32 &aPhysAddr)
{
    MMHwBuffer::TBufferInfo vInfo;
    MMHwBufferPool_list_elem *iter_elem;
    CIRCLEQ_FOREACH(iter_elem, &mList, list_entry) {
        iter_elem->mPool->BufferInfoInd(0, vInfo);
        if (aLogAddr == vInfo.iLogAddr) {
            return iter_elem->mPool->CacheSync(aOp, aLogAddr, aSize, aPhysAddr);
        }
    }
    ALOGE("%s: could not find in 0x%08x a match for aLogAddr=0x%08x",
         __func__, (unsigned int)this, (unsigned int)aLogAddr);
    return OMX_ErrorBadParameter;
}

OMX_U32 MMHwBufferList::getTotalSize()
{
    OMX_U32 totalSize = 0;
    MMHwBufferPool_list_elem *iter_elem;

    CIRCLEQ_FOREACH(iter_elem, &mList, list_entry) {
        totalSize += iter_elem->mPool->getTotalSize();
    }

    return totalSize;
}

OMX_U32 MMHwBufferList::dump()
{
    OMX_U32 totalSize = 0;
    OMX_U32 nbElem = 0;
    MMHwBufferPool_list_elem *iter_elem;
    ALOGI(" ********************");
    ALOGI(" MMHwBufferList 0x%08x :", (unsigned int)this);
    ALOGI(" ++++++++++++++++++++");
    CIRCLEQ_FOREACH(iter_elem, &mList, list_entry) {
        totalSize += iter_elem->mPool->dump();
        nbElem++;
    }
    ALOGI(" ++++++++++++++++++++");
    ALOGI(" MMHwBufferList 0x%08x : nbMMHwBuffer=%d totalSize=%d",
         (unsigned int)this, (int)nbElem, (int)totalSize);
    ALOGI(" ********************");
    return totalSize;
}

#ifdef ANDROID
OMX_ERRORTYPE MMHwBufferList::addAndroidNativeBuffer(UseAndroidNativeBufferParams *params)
{
    buffer_handle_t bHandle = params->nativeBuffer->handle;

    MMHwBufferPool *bPool = new MMHwBufferPool((void *)bHandle);
    if (bPool == NULL) {
        ALOGE("%s: InsufficientResources", __func__);
        return OMX_ErrorInsufficientResources;
    }

    // We are in the context of the ENSwrapper lock so no issue to manipulate the list
    CIRCLEQ_INSERT_TAIL(&mList, &bPool->list_elem, list_entry, MMHwBufferPool_list_elem);

    ALOGV("%s: added AndroidNativeBuffer bPool=0x%08x to bList=0x%08x",
         __func__, (unsigned int)bPool, (unsigned int)this);

    return OMX_ErrorNone;
}
#endif //ANDROID
