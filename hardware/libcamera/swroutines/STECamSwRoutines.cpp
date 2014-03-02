/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamSwRoutines.h"


#define STECAMSWROUTINES_SO_FILE "/system/lib/libste_aiq_tools.so"

namespace android {

/* static */ void* CamSwRoutines::mLibHandle;
/* static */ swRoutines CamSwRoutines::mSwRoutines[CamSwRoutines::EMaxType];

/* static */ const char* CamSwRoutines::mSwRoutinesNames[] = {
    "AIQ_UYVY422Itld_to_YUV420P", //EConUYVY422ItldToYUV420P
    "AIQ_UYVY422Itld_to_YUV420SPNV12", //EConUYVY422ItldToYUV420SPNV12
    "AIQ_UYVY422Itld_to_YUV420SPNV21", //EConUYVY422ItldToYUV420SPNV21
    "AIQ_RGB565_to_YUV420P", //EConRGB565ToYUV420P
    "AIQ_RGB565_to_YUV420SPNV12", //EConRGB565ToYUV420SPNV12
    "AIQ_RGB565_to_YUV420SPNV21", //EConRGB565ToYUV420SPNV21
    "AIQ_YUV420MB_180_rotation_inplace_neon", //ERotYUV420MB180DEG
    "AIQ_UYVY422Itld_90_rotation_inplace", //ERotUYVY422Itld90DEG
    "AIQ_UYVY422Itld_270_rotation_inplace" //ERotUYVY422Itld270DEG
};

status_t CamSwRoutines::init()
{
    DBGT_PROLOG("");
    status_t err = NO_ERROR;

    mLibHandle = dlopen(STECAMSWROUTINES_SO_FILE, RTLD_NOW);
    if(NULL == mLibHandle) {
        DBGT_CRITICAL("Could not open handle to the SwRoutines ");
        DBGT_EPILOG("");
        return (int)UNKNOWN_ERROR;
    }

    //check all name exist
    CAM_ASSERT_COMPILE(EMaxType == ARRAYCOUNT(CamSwRoutines::mSwRoutinesNames));

    for(AIQ_U32 loop = 0; loop < EMaxType; loop++){
       DBGT_PTRACE("Type: %lu SwRoutine: %s", loop, CamSwRoutines::mSwRoutinesNames[loop]);

        mSwRoutines[loop] = (swRoutines)dlsym(mLibHandle, CamSwRoutines::mSwRoutinesNames[loop]);

        if(NULL == mSwRoutines[loop]) {
            DBGT_CRITICAL("Could not get symbols from SW Routines");
            DBGT_EPILOG("");
            return (int)UNKNOWN_ERROR;
        }
        }

        DBGT_EPILOG("");
        return (int)err;
}

/* static */ void CamSwRoutines::UYVY422Itld_to_YUV420P(AIQ_U8 *aInBuffer,
                                                        AIQ_U8 *aOutBuffer,
                                                        AIQ_U32 aWidth,
                                                        AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConUYVY422ItldToYUV420P, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::UYVY422Itld_to_YUV420SP_NV12(AIQ_U8 *aInBuffer,
                                                              AIQ_U8 *aOutBuffer,
                                                              AIQ_U32 aWidth,
                                                              AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConUYVY422ItldToYUV420SPNV12, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::UYVY422Itld_to_YUV420SP_NV21(AIQ_U8 *aInBuffer,
                                                              AIQ_U8 *aOutBuffer,
                                                              AIQ_U32 aWidth,
                                                              AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConUYVY422ItldToYUV420SPNV21, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}


/* static */ void CamSwRoutines::RGB565_to_YUV420P(AIQ_U8 *aInBuffer,
                                                   AIQ_U8 *aOutBuffer,
                                                   AIQ_U32 aWidth,
                                                   AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConRGB565ToYUV420P, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::RGB565_to_YUV420SP_NV12(AIQ_U8 *aInBuffer,
                                                         AIQ_U8 *aOutBuffer,
                                                         AIQ_U32 aWidth,
                                                         AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConRGB565ToYUV420SPNV12, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::RGB565_to_YUV420SP_NV21(AIQ_U8 *aInBuffer,
                                                         AIQ_U8 *aOutBuffer,
                                                         AIQ_U32 aWidth,
                                                         AIQ_U32 aHeigth)
{
    DBGT_PROLOG("In: %p Out: %p Width: %lu Height: %lu", aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aOutBuffer, "Out buffer NULL");

    process(EConRGB565ToYUV420SPNV21, aInBuffer, aOutBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}


/* static */ void CamSwRoutines::YUV420MB_180_rotation_inplace(AIQ_U8 *aInBuffer,
                                                               AIQ_U8 *aDummyBuffer,
                                                               AIQ_U32 aWidth,
                                                               AIQ_U32 aHeigth)
{
    DBGT_PROLOG("InOut: %p Dummy: %p Width: %lu Height: %lu", aInBuffer, aDummyBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL == aDummyBuffer, "Dummy buffer not NULL");

    process(ERotYUV420MB180DEG, aInBuffer, aDummyBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::UYVY422Itld_90_rotation_inplace (AIQ_U8 *aInBuffer,
                                                                  AIQ_U8 *aTempBuffer,
                                                                  AIQ_U32 aWidth,
                                                                  AIQ_U32 aHeigth)
{
    DBGT_PROLOG("InOut: %p Temp: %p Width: %lu Height: %lu", aInBuffer, aTempBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aTempBuffer, "Out buffer NULL");

    process(ERotUYVY422Itld90DEG, aInBuffer, aTempBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

/* static */ void CamSwRoutines::UYVY422Itld_270_rotation_inplace (AIQ_U8 *aInBuffer,
                                                                   AIQ_U8 *aTempBuffer,
                                                                   AIQ_U32 aWidth,
                                                                   AIQ_U32 aHeigth)
{
    DBGT_PROLOG("InOut: %p Temp: %p Width: %lu Height: %lu", aInBuffer, aTempBuffer, aWidth, aHeigth);

    DBGT_ASSERT(NULL != aTempBuffer, "Out buffer NULL");

    process(ERotUYVY422Itld270DEG, aInBuffer, aTempBuffer, aWidth, aHeigth);

    DBGT_EPILOG("");
    return;
}

}; // namespace android
