/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __FLASH_LIB_H__
#define __FLASH_LIB_H__

#include <flash_api.h>
#include <signal.h> 
#include <pthread.h>

class LinuxCFlashDriver : public CFlashDriver {
    public:
        LinuxCFlashDriver();
        virtual ~LinuxCFlashDriver();
        virtual void Close();
        virtual void GetSupportedFlashModes(TFlashMode& aModes, TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode GetFlashModeDetails(TFlashMode aFlashMode, TFlashDetails& aDetails,
                TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode EnableFlashMode(TFlashMode aFlashMode, TCallbackFn aCallback,
                void *apContext,
                TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode ConfigureFlashMode(TFlashMode aFlashMode, uint32_t aDuration,
                uint32_t aIntensity, uint32_t aTimeout, TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode Strobe(TFlashMode aFlashMode, bool Enable, TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode GetStatus(TFlashMode aFlashMode, TFlashStatus& aFlashStatus, TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode GetXenonLifeCounter(uint32_t &aCounter, TCameraId aCameraId = EPrimary);
        virtual void GetSupportedSelfTests(TFlashSelftest& aSelfTests,
                TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode DoSelfTest (TFlashMode aSelfTest,
                TSelfTestCallbackFn aSelfTestCb,
                void *apContext,
                TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode GetFaultRegisters( TFlashMode aFlashModes,
                flash_uint32_t *apFaultRegisters,
                TCameraId aCameraId = EPrimary);
        virtual TFlashReturnCode GetNVM(
                TFlashMode aFlashModes,
                flash_uint32_t BufferSize,
                void* NVMBuffer,
                TGetNVMCallbackFn cb,
                void *apContext,
                TCameraId aCameraId = EPrimary);		
		
        int openfd();
public:
        int fd;
        TSelfTestCallbackFn aSelfTestCallback;
        int aSelfTestContext;
        /* To count number of threads(clients) using the flash device driver. */
        /* Intialized from 0, Increment on every Open() call, Decrement on evey close() call*/
        /* Purpose: Only one file descriptor would be created for multiple clients*/
        static int mCount;
        /** The mutex to protect mDriverClientCount*/
        static pthread_mutex_t mMutex;
};
#endif
