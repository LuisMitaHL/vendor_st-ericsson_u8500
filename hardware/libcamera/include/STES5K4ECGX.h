/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * SteExtIspCamera.h : external Isp camera part
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef DEINTERLACE_H
#define DEINTERLACE_H

#include "STEArmIvProc.h"

namespace android {

class Deinterlace{
public:
    bool deinterlacingYuvThumbnailFromJpeg(
                    char* pJpegBuffer,
                    int*  pJpegBufferSize,
                    char* pYuv422IThumbBuf,
                    int   Yuv422IThumbBufSize,
                    int   embeddedThumbWidth,
                    int   embeddedThumbHeight);
    int DecodeInterleaveData(
                unsigned char *pInterleaveData,
                int *pJpegSize,
                unsigned char *pYuvData,
                int yuvThumbBufSize,
                int yuvThumbWidth,
                int yuvThumbHeight);
private:
    SteArmIvProc*       ArmIvProc; /**< ArmIvProc session */

};

}; // namespace android

#endif




