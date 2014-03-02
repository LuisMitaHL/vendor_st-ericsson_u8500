/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define LOG_TAG "MMNativeBuffer"
#include "linux_utils.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/hwmem.h>
#include "MMNativeBuffer.h"

#ifdef ANDROID
  #include <hardware/gralloc.h>
  #include <gralloc_stericsson_ext.h>
  #include "media/hardware/HardwareAPI.h"

  using namespace android;

  static const struct gralloc_module_t *gralloc = NULL;
#endif

#ifndef WORKSTATION
  #include <linux/hwmem.h>
  static const char hwmem_files_full_name[] = "/dev/" HWMEM_DEFAULT_DEVICE_NAME;
#endif

int MMNativeBuffer::Open(int *nativefd, int *name, int *pBufferSize)
{
#ifdef WORKSTATION
    return -1;
#else
    *nativefd = open(hwmem_files_full_name, O_RDWR);
    if (*nativefd < 0) {
        ALOGE("Open: failed to open " HWMEM_DEFAULT_DEVICE_NAME " device\n");
        return -1;
    }

    if (name == NULL) {
        return 0;
    }

    if (*name <= 0) {
        ALOGE("Open: invalid export name\n");
        return -1;
    }

    if (ioctl(*nativefd, HWMEM_IMPORT_FD_IOC, *name) != 0) {
        ALOGE("Open: failed to import HWMEM (%d) : %s\n", *name, strerror(errno));
        return -1;
    }

    // retreive total allocation size
    struct hwmem_get_info_request hwmem_info_req;
    hwmem_info_req.id = 0;

    if (ioctl(*nativefd, HWMEM_GET_INFO_IOC, &hwmem_info_req) < 0) {
        ALOGE("Open: failed to retreive HWMEM info : %s\n", strerror(errno));
        return -1;
    }
    if (pBufferSize != NULL) {
        *pBufferSize = hwmem_info_req.size;
    }

    return 0;
#endif  // #ifdef WORKSTATION
}

void *MMNativeBuffer::Map(int nativefd, size_t size)
{
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, nativefd, 0);
}

int MMNativeBuffer::UnMap(void *addr, size_t length)
{
    return munmap(addr, length);
}

int MMNativeBuffer::Pin(int nativefd, unsigned long *physAddr)
{
#ifdef WORKSTATION
    return -1;
#else
    struct hwmem_pin_request hwmem_pin_req;
    hwmem_pin_req.id = 0;
    if (ioctl(nativefd, HWMEM_PIN_IOC, &hwmem_pin_req) < 0) {
        return -1;
    }
    *physAddr = hwmem_pin_req.phys_addr;
    return 0;
#endif
}

int MMNativeBuffer::UnPin(int nativefd)
{
#ifdef WORKSTATION
    return -1;
#else
    return ioctl(nativefd, HWMEM_UNPIN_IOC);
#endif
}

////////////////////////////////////////////////////////////////////////////////

MMNativeBuffer::MMNativeBuffer(void *pBuffer)
    : mNativeBuffer((unsigned long)pBuffer),
      mExportName(-1),
      mHwmemFd(-1),
      mBufferSize(0),
      mBufferWidth(0),
      mBufferHeight(0),
      mBufferFormat(0),
      mBufferType(0),
      mBufferOffset(0),
      mLogicalAddress(0),
      mPhysicalAddress(0),
      mInitialized(false)
{ }

int MMNativeBuffer::getBuffer()
{
    return mNativeBuffer;
}

int MMNativeBuffer::getHandle()
{
    if (mInitialized) {
        return mHwmemFd;
    } else {
        return -1;
    }
}

int MMNativeBuffer::getExportName()
{
    if (!mInitialized) {
        return -1;
    }
    return mExportName;
}

unsigned long MMNativeBuffer::getLogicalAddress()
{
    if (!mInitialized) {
        return 0;
    }
    return mLogicalAddress;
}

unsigned long MMNativeBuffer::getPhysicalAddress()
{
    if (!mInitialized) {
        return 0;
    }
    return mPhysicalAddress;
}

int MMNativeBuffer::getBufferSize()
{
    if (!mInitialized) {
        return 0;
    }
    return mBufferSize;
}

int MMNativeBuffer::getBufferWidth()
{
    if (!mInitialized) {
        return -1;
    }
    return mBufferWidth;
}

int MMNativeBuffer::getBufferHeight()
{
    if (!mInitialized) {
        return -1;
    }
    return mBufferHeight;
}

int MMNativeBuffer::getBufferOffset()
{
    if (!mInitialized) {
        return -1;
    }
    return mBufferOffset;
}

int MMNativeBuffer::getBufferFormat()
{
    if (!mInitialized) {
        return -1;
    }
    return mBufferFormat;
}

int MMNativeBuffer::getBufferType()
{
    if (!mInitialized) {
        return -1;
    }
    return mBufferType;
}

int MMNativeBuffer::getFileHandle(void *pBuffer, int *pBufferSize)
{
#ifdef ANDROID
    if (gralloc == NULL) {
        if (hw_get_module(GRALLOC_HARDWARE_MODULE_ID,
                          (const struct hw_module_t **)&gralloc)) {
            ALOGE("%s: Error opening gralloc module", __func__);
            return -1;
        }
    }

    buffer_handle_t bHandle = (buffer_handle_t)pBuffer;
    int exportName
        = gralloc->perform(gralloc,
                           GRALLOC_MODULE_PERFORM_GET_BUF_ALLOCATOR_HANDLE,
                           bHandle);

    if (exportName < 0) {
        ALOGE("%s: Could not get allocator handle from gralloc!", __func__);
        return -1;
    }

    int fd = open(hwmem_files_full_name, O_RDWR);
    if (fd < 0) {
        ALOGE("%s: failed to open "
             HWMEM_DEFAULT_DEVICE_NAME
             " device", __func__);
        return -1;
    }

    if (ioctl(fd, HWMEM_IMPORT_FD_IOC, exportName) != 0) {
        ALOGE("%s: failed to import HWMEM (%d) : %s\n",
             __func__, exportName, strerror(errno));
        return -1;
    }

    // retreive total allocation size
    struct hwmem_get_info_request hwmem_info_req;
    hwmem_info_req.id = 0;
    if (ioctl(fd, HWMEM_GET_INFO_IOC, &hwmem_info_req) < 0) {
        ALOGE("%s: failed to retreive HWMEM info : %s\n",
             __func__, strerror(errno));
        return -1;
    }

    if (pBufferSize) {
        *pBufferSize = hwmem_info_req.size;
    }

    return fd;
#else
    return -1;
#endif
}

int MMNativeBuffer::init()
{
    if (mInitialized) {
        return 0;
    }
#ifdef ANDROID
    if (gralloc == NULL) {
        if (hw_get_module(GRALLOC_HARDWARE_MODULE_ID,
                          (const struct hw_module_t **)&gralloc)) {
            ALOGE("%s: Error opening gralloc module", __func__);
            return -1;
        }
    }

    buffer_handle_t bHandle = (buffer_handle_t)mNativeBuffer;
    mExportName = gralloc->perform(gralloc,
                                   GRALLOC_MODULE_PERFORM_GET_BUF_ALLOCATOR_HANDLE,
                                   bHandle);

    if (mExportName < 0) {
        ALOGE("%s: Could not get allocator handle from gralloc!", __func__);
        return -1;
    }

    mBufferSize = gralloc->perform(gralloc,
                                GRALLOC_MODULE_PERFORM_GET_BUF_SIZE,
                                bHandle);
    if (mBufferSize < 0) {
        ALOGE("%s: Could not get buffer size from gralloc!", __func__);
        return -1;
    }

    mBufferOffset = gralloc->perform(gralloc,
                                  GRALLOC_MODULE_PERFORM_GET_BUF_OFFSET,
                                  bHandle);
    if (mBufferOffset < 0) {
        ALOGE("%s: Could not get buffer offset from gralloc!", __func__);
        return -1;
    }

    mBufferType = gralloc->perform(gralloc,
                                GRALLOC_MODULE_PERFORM_GET_BUF_TYPE,
                                bHandle);
    if (mBufferType < 0) {
        ALOGE("%s: Could not get buffer type from gralloc!", __func__);
        return -1;
    }

    if (mBufferType != GRALLOC_BUF_TYPE_HWMEM || mBufferOffset != 0) {
        ALOGE("%s: got non null offset and type buffer:"
             " size=%d offset=%d type=%d",
             __func__, mBufferSize, mBufferOffset, mBufferType);
        return -1;
    }

    mBufferWidth = gralloc->perform(gralloc,
                                GRALLOC_MODULE_PERFORM_GET_BUF_WIDTH,
                                bHandle);
    if (mBufferWidth < 0) {
        ALOGE("%s: Could not get buffer type from gralloc!", __func__);
        return -1;
    }

    mBufferHeight = gralloc->perform(gralloc,
                                GRALLOC_MODULE_PERFORM_GET_BUF_HEIGHT,
                                bHandle);
    if (mBufferHeight < 0) {
        ALOGE("%s: Could not get buffer type from gralloc!", __func__);
        return -1;
    }

    mBufferFormat = gralloc->perform(gralloc,
                                GRALLOC_MODULE_PERFORM_GET_BUF_FORMAT,
                                bHandle);
    if (mBufferFormat < 0) {
        ALOGE("%s: Could not get buffer type from gralloc!", __func__);
        return -1;
    }

    if (MMNativeBuffer::Open(&mHwmemFd, &mExportName, &mBufferSize) != 0) {
        return -1;
    }

    mLogicalAddress =  (unsigned long)MMNativeBuffer::Map(mHwmemFd, mBufferSize);
    if (mLogicalAddress == (unsigned long)MAP_FAILED) {
        mLogicalAddress = 0;
        ALOGE("%s: mmap() failed", __func__);
        return -1;
    }

    // pin buffer to get physical address
    if (MMNativeBuffer::Pin(mHwmemFd, &mPhysicalAddress) < 0) {
        ALOGE("%s: Pin failed", __func__);
        if (MMNativeBuffer::UnMap((void *)mLogicalAddress, mBufferSize) < 0) {
            ALOGE("%s: munmap failed", __func__);
        }
        mLogicalAddress = 0;
        return -1;
    }
#endif //#ifdef ANDROID

    mInitialized = true;
    return 0;
}

MMNativeBuffer::~MMNativeBuffer()
{
    if (mInitialized) {
        MMNativeBuffer::UnMap((void *)mLogicalAddress, mBufferSize);
        MMNativeBuffer::UnPin(mHwmemFd);
        close(mHwmemFd);
    }
}
