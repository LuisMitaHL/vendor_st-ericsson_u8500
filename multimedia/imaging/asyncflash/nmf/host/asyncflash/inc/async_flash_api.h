/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ASYNC_FLASH_API_H
#define __ASYNC_FLASH_API_H

#include <flash_api.h>

struct GetSupportedFlashModes_t
{
        TFlashMode aModes;
        TCameraId aCameraId;
};
struct GetFlashModeDetails_t
{
        TFlashMode aFlashMode;
        TFlashDetails& aDetails;
        TCameraId aCameraId;
};
struct EnableFlashMode_t
{
        TFlashMode aFlashMode;
        TCallbackFn aCallback;
        void *apContext;
        TCameraId aCameraId;
};
struct ConfigureFlashMode_t
{
        TFlashMode aFlashMode;
        uint32_t aDuration;
        uint32_t aIntensity;
        uint32_t aTimeout;
        TCameraId aCameraId;
};
struct Strobe_t
{
        TFlashMode aFlashMode;
        bool aEnable;
        TCameraId aCameraId;
};
struct GetStatus_t
{
        TFlashMode aFlashMode;
        TFlashStatus& aFlashStatus;
        TCameraId aCameraId;
};
struct GetXenonLifeCounter_t
{
        flash_uint32_t &aCounter;
        TCameraId aCameraId;
};
struct GetSupportedSelfTests_t
{
        TFlashSelftest& aSelfTests;
        TCameraId aCameraId;
};
struct DoSelfTest_t
{
        TFlashSelftest aSelfTest;
        TCameraId aCameraId;
};
struct GetFaultRegisters_t
{
        TFlashMode aFlashModes;
        flash_uint32_t *apFaultRegisters;
        TCameraId aCameraId;
};
struct GetNVM_t
{
        TFlashMode aFlashModes;
        flash_uint32_t BufferSize;
        void* NVMBuffer;
        TCameraId aCameraId;
};


#endif
